#!/usr/bin/env node
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.load_from_package = exports.destroy = exports.discover = exports.clear = exports.initialize = exports.load_from_memory = exports.load_from_file = void 0;
const fs_1 = require("fs");
const Module = require("module");
const os_1 = require("os");
const path = require("path");
const monkey_patch_require = Module.prototype.require;
const node_require = Module.prototype.node_require;
const node_cache = Module.prototype.node_cache || require.cache;
const node_resolve = Module.prototype.node_resolve || require.resolve;
/** Unpatch in order to load TypeScript */
if (node_require) {
    Module.prototype.require = node_require;
}
/** Native node require */
const ts = require("typescript");
/** Define the extensions for requiring with TypeScript */
["ts", "tsx", "jsx"].forEach(ext => {
    /* If we require a TypeScript file from NodeJS, probably we do not need introspection data */
    Module._extensions[`.${ext}`] = (module) => {
        const exp = exports.load_from_file([module.filename], false);
        if (exp !== null) {
            const values = Object.values(exp);
            if (values.length === 1) {
                module.exports = values[0];
            }
        }
    };
});
/** Patch again */
if (node_require) {
    Module.prototype.require = monkey_patch_require;
}
const noop = () => ({});
const discoverTypes = new Map();
/** Logging util */
const log = process.env.METACALL_DEBUG ? console.log : noop;
/** Util: Wraps a function in try / catch and possibly logs */
const safe = (f, def) => (...args) => {
    try {
        log(f.name, "<-", ...args);
        const res = f(...args);
        log(f.name, "->", res);
        return res;
    }
    catch (err) {
        console.log(`Exception in ${f.name}`, err);
        return def;
    }
};
const wrapFunctionExport = (e) => typeof e === "function" ? { [e.name]: e } : e;
const defaultCompilerOptions = {
    target: ts.ScriptTarget.ES2017,
    module: ts.ModuleKind.CommonJS,
    lib: ["lib.es2017.d.ts"],
};
/** Generate diagnostics if any, the return value true means there was an error, false otherwise */
const generateDiagnostics = (program, diagnostics, errors) => {
    const allDiagnostics = ts.getPreEmitDiagnostics(program).concat(diagnostics, errors);
    if (allDiagnostics.length) {
        const formatHost = {
            getCanonicalFileName: (path) => path,
            getCurrentDirectory: ts.sys.getCurrentDirectory,
            getNewLine: () => ts.sys.newLine,
        };
        const message = ts.formatDiagnosticsWithColorAndContext(allDiagnostics, formatHost);
        console.log(message);
    }
};
const getProgramOptions = (paths = []) => {
    const defaultOptions = { options: defaultCompilerOptions, rootNames: paths, configFileParsingDiagnostics: [] };
    const configFile = ts.findConfigFile(process.cwd(), ts.sys.fileExists, "tsconfig.json");
    if (!configFile) {
        return defaultOptions;
    }
    const { config, error } = ts.readConfigFile(configFile, ts.sys.readFile);
    if (error) {
        return Object.assign(Object.assign({}, defaultOptions), { configFileParsingDiagnostics: [error] });
    }
    const { options, fileNames, errors } = ts.parseJsonConfigFileContent(config, ts.sys, process.cwd());
    return { options: errors.length > 0 ? defaultCompilerOptions : options, rootNames: fileNames || paths, configFileParsingDiagnostics: errors };
};
const getTranspileOptions = (moduleName, path) => {
    const programOptions = getProgramOptions([path]);
    return {
        programOptions,
        transpileOptions: {
            compilerOptions: programOptions.options,
            fileName: path,
            reportDiagnostics: true,
            moduleName,
        },
    };
};
const getMetacallExportTypes = (p, paths = [], cb = () => { }) => {
    const exportTypes = {};
    const files = paths.length === 0 ?
        p.getRootFileNames() :
        paths.map(fileResolveNoThrow).filter(file => p.getRootFileNames().map(path.normalize).includes(path.normalize(file)));
    const sourceFiles = files.map((name) => [name, p.getSourceFile(name)]);
    for (const [fileName, sourceFile] of sourceFiles) {
        if (!sourceFile) {
            // TODO: Implement better exception handling
            console.log(`Error: Failed to load ${fileName}`);
            const emitResult = p.emit();
            generateDiagnostics(p, emitResult.diagnostics, []);
            return null;
        }
        const c = p.getTypeChecker();
        const sym = c.getSymbolAtLocation(sourceFile);
        const moduleExports = sym ? c.getExportsOfModule(sym) : [];
        for (const e of moduleExports) {
            const metacallType = (exportTypes[e.name] = exportTypes[e.name] || {});
            const exportType = c.getTypeOfSymbolAtLocation(e, sourceFile);
            const callSignatures = exportType.getCallSignatures();
            if (callSignatures.length === 0) {
                continue;
            }
            for (const signature of callSignatures) {
                const parameters = signature.getParameters();
                metacallType.signature = parameters.map((p) => p.name);
                metacallType.types = parameters.map((p) => c.typeToString(c.getTypeOfSymbolAtLocation(p, p.valueDeclaration)));
                const returnType = signature.getReturnType();
                metacallType.ret = c.typeToString(returnType);
                const flags = ts.getCombinedModifierFlags(e.valueDeclaration);
                metacallType.async = Boolean(flags & ts.ModifierFlags.Async);
            }
        }
        cb(sourceFile, exportTypes);
    }
    return exportTypes;
};
const fileResolve = (p) => {
    try {
        return node_resolve(p);
    }
    catch (ex) {
        if (ex.code !== "MODULE_NOT_FOUND") {
            throw ex;
        }
        // Try global paths
        const paths = (process.env["NODE_PATH"] || "").split(path.delimiter).filter((e) => e.length !== 0);
        for (const r of paths) {
            try {
                return node_resolve(path.join(r, p));
            }
            catch (e) {
                if (e.code !== "MODULE_NOT_FOUND") {
                    throw e;
                }
            }
        }
    }
    throw Object.assign(Error(`Cannot find module '${p}'`), { code: "MODULE_NOT_FOUND" });
};
const fileResolveNoThrow = (p) => {
    try {
        return fileResolve(p);
    }
    catch (_) {
        return p;
    }
};
/** Loads a TypeScript file from disk */
exports.load_from_file = safe(function load_from_file(paths, discover = true) {
    const result = {};
    const options = getProgramOptions(paths.map(p => fileResolve(p)));
    const p = ts.createProgram(options);
    // TODO: Handle the emitSkipped?
    const exportTypes = getMetacallExportTypes(p, paths, (sourceFile, exportTypes) => {
        const { diagnostics /*, emitSkipped */ } = p.emit(sourceFile, (fileName, data) => {
            let _a;
            // @ts-ignore
            const nodeModulePaths = Module._nodeModulePaths(path.dirname(fileName));
            const parent = module.parent;
            const m = new Module(fileName, parent || undefined);
            m.filename = fileName;
            m.paths = nodeModulePaths;
            m._compile(data, fileName);
            const wrappedExports = wrapFunctionExport(m.exports);
            for (const [name, handle] of Object.entries(exportTypes)) {
                handle.ptr = wrappedExports[name];
            }
            if (discover) {
                discoverTypes.set(fileName, Object.assign(Object.assign({}, ((_a = discoverTypes.get(fileName)) !== null && _a !== void 0 ? _a : {})), exportTypes));
            }
            result[fileName] = wrappedExports;
        });
        generateDiagnostics(p, diagnostics, options.configFileParsingDiagnostics);
    });
    return exportTypes === null ? null : result;
}, null);
/** Loads a TypeScript file from memory */
exports.load_from_memory = safe(function load_from_memory(name, data) {
    let _a;
    const extName = `${name}.ts`;
    const { programOptions, transpileOptions } = getTranspileOptions(name, extName);
    const transpileOutput = ts.transpileModule(data, transpileOptions);
    const target = (_a = programOptions.options.target) !== null && _a !== void 0 ? _a : defaultCompilerOptions.target;
    const p = ts.createProgram([extName], programOptions.options, {
        fileExists: (fileName) => fileName === extName,
        getCanonicalFileName: (fileName) => fileName,
        getCurrentDirectory: ts.sys.getCurrentDirectory,
        getDefaultLibFileName: ts.getDefaultLibFileName,
        getNewLine: () => os_1.EOL,
        getSourceFile: (fileName) => {
            if (fileName === extName) {
                return ts.createSourceFile(fileName, data, target);
            }
            if (fileName.endsWith(".d.ts")) {
                try {
                    const tsPath = path.join(path.dirname(node_resolve("typescript")), fileName);
                    return ts.createSourceFile(fileName, fs_1.readFileSync(tsPath, "utf8"), target);
                }
                catch (err) {
                    return ts.createSourceFile(fileName, fs_1.readFileSync(fileName, "utf8"), target);
                }
            }
        },
        readFile: (fileName) => fileName === extName ? data : undefined,
        useCaseSensitiveFileNames: () => true,
        writeFile: () => { },
    });
    const exportTypes = getMetacallExportTypes(p);
    if (exportTypes === null) {
        // TODO: Improve error handling
        return null;
    }
    const m = new Module(name);
    m._compile(transpileOutput.outputText, name);
    const result = {
        [name]: wrapFunctionExport(m.exports),
    };
    for (const [n, handle] of Object.entries(exportTypes)) {
        handle.ptr = result[name][n];
    }
    discoverTypes.set(name, exportTypes);
    return result;
}, null);
/** Registers the TypeScript loader in the correct thread, this loader must always be the child of NodeJS loader */
exports.initialize = safe(function initialize(register_cb, impl) {
    return register_cb(impl);
}, false);
/** Unloads a TypeScript file using handle returned from load_from_file / load_from_memory */
exports.clear = safe(function clear(handle) {
    const names = Object.getOwnPropertyNames(handle);
    for (let i = 0; i < names.length; ++i) {
        const p = names[i];
        discoverTypes.delete(p);
        try {
            const absolute = fileResolve(p);
            if (node_cache[absolute]) {
                delete node_cache[absolute];
            }
        }
        catch (_) { }
    }
}, undefined);
/** Returns type information about exported functions from a given handle */
exports.discover = safe(function discover(handle) {
    const result = Object.keys(handle)
        .reduce((acc, k) => { let _a; return (Object.assign(Object.assign({}, acc), (_a = discoverTypes.get(k)) !== null && _a !== void 0 ? _a : {})); }, {});
    return result;
}, {});
/** Destroy TypeScript loader children in the correct thread, this loader must always be the child of NodeJS loader */
exports.destroy = safe(function destroy(unload_children_cb, impl) {
    return unload_children_cb(impl);
}, false);
/** Unimplemented */
exports.load_from_package = noop;
