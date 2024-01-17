//
// Created by Twelvee on 13.11.2023.
//

#include "V8Module.h"
#include "Exception.h"
#include "../Engine.h"
#include "../../logger/logger.h"
#include <map>

extern Engine *g_Engine;

enum ModuleType {
    kJavaScript,
    kJSON,
    kInvalid
};

enum { kModuleEmbedderDataIndex, kInspectorClientIndex };

std::string ToSTLString(v8::Isolate* isolate, v8::Local<v8::String> v8_str) {
    v8::String::Utf8Value utf8(isolate, v8_str);
    return *utf8;
}

class ModuleEmbedderData {
private:
    class ModuleGlobalHash {
    public:
        explicit ModuleGlobalHash(v8::Isolate* isolate) : isolate_(isolate) {}
        size_t operator()(const v8::Global<v8::Module>& module) const {
            return module.Get(isolate_)->GetIdentityHash();
        }

    private:
        v8::Isolate* isolate_;
    };

public:
    explicit ModuleEmbedderData(v8::Isolate* isolate)
            : module_to_specifier_map(10, ModuleGlobalHash(isolate)),
              json_module_to_parsed_json_map(10, ModuleGlobalHash(isolate)) {}

    static ModuleType ModuleTypeFromImportAssertions(
            v8::Local<v8::Context> context, v8::Local<v8::FixedArray> import_assertions,
            bool hasPositions) {
        v8::Isolate* isolate = context->GetIsolate();
        const int kV8AssertionEntrySize = hasPositions ? 3 : 2;
        for (int i = 0; i < import_assertions->Length();
             i += kV8AssertionEntrySize) {
            v8::Local<v8::String> v8_assertion_key =
                    import_assertions->Get(context, i).As<v8::String>();
            std::string assertion_key = ToSTLString(isolate, v8_assertion_key);

            if (assertion_key == "type") {
                v8::Local<v8::String> v8_assertion_value =
                        import_assertions->Get(context, i + 1).As<v8::String>();
                std::string assertion_value = ToSTLString(isolate, v8_assertion_value);
                if (assertion_value == "json") {
                    return ModuleType::kJSON;
                } else {
                    // JSON is currently the only supported non-JS type
                    return ModuleType::kInvalid;
                }
            }
        }

        // If no type is asserted, default to JS.
        return ModuleType::kJavaScript;
    }

    // Map from (normalized module specifier, module type) pair to Module.
    std::map<std::pair<std::string, ModuleType>, v8::Global<v8::Module>> module_map;
    // Map from Module to its URL as defined in the ScriptOrigin
    std::unordered_map<v8::Global<v8::Module>, std::string, ModuleGlobalHash>
    module_to_specifier_map;
    // Map from JSON Module to its parsed content, for use in module
    // JSONModuleEvaluationSteps
    std::unordered_map<v8::Global<v8::Module>, v8::Global<v8::Value>, ModuleGlobalHash>
    json_module_to_parsed_json_map;
};

ModuleEmbedderData* GetModuleDataFromContext(v8::Local<v8::Context> context) {
    return static_cast<ModuleEmbedderData*>(
            context->GetAlignedPointerFromEmbedderData(kModuleEmbedderDataIndex));
}

std::string V8Module::ReadFile(std::string filename) {
    std::string fullModuleName = g_Engine->pluginsFolder;
    fullModuleName.append("/").append(filename);
    return Engine::GetFileContent(fullModuleName);
}

template <class T>
v8::MaybeLocal<T> V8Module::CompileString(v8::Isolate* isolate, v8::Local<v8::Context> context,
                                        v8::Local<v8::String> source,
                                   const v8::ScriptOrigin& origin) {
    v8::ScriptCompiler::CachedData* cached_code = nullptr;

    v8::ScriptCompiler::Source script_source(source, origin, cached_code);
    v8::MaybeLocal<T> result =
            V8Module::Compile(context, &script_source, v8::ScriptCompiler::kNoCompileOptions);
    return result;
}

v8::MaybeLocal<v8::Module> V8Module::LoadModule(std::string code, std::string file_name, v8::Local<v8::Context> context) {
    v8::Isolate* isolate = context->GetIsolate();
    v8::Local<v8::String> source_text = v8::String::NewFromUtf8(isolate, code.c_str(), v8::NewStringType::kNormal).ToLocalChecked();

    ModuleEmbedderData* d = GetModuleDataFromContext(context);
    if (source_text.IsEmpty()) {
        isolate->ThrowError(
                v8::String::NewFromUtf8(isolate, "Failed to load module.").ToLocalChecked());
        return v8::MaybeLocal<v8::Module>();
    }
    v8::ScriptOrigin origin(
            isolate, v8::String::NewFromUtf8(isolate, file_name.c_str()).ToLocalChecked(),
            0, 0, false, -1, v8::Local<v8::Value>(), false, false, true);

    v8::Local<v8::Module> module;
    v8::ScriptCompiler::Source source(source_text, origin);
        if (!CompileString<v8::Module>(isolate, context, source_text, origin)
                .ToLocal(&module)) {
            return v8::MaybeLocal<v8::Module>();
        }

    return module;
}

v8::Local<v8::Module> V8Module::CheckModule(v8::MaybeLocal<v8::Module> maybeModule,
                                          v8::Local<v8::Context> cx) {
    v8::TryCatch try_catch(cx->GetIsolate());
    v8::Local<v8::Module> mod;
    if (!maybeModule.ToLocal(&mod)) {
        printf("Error loading module!\n");
        exit(EXIT_FAILURE);
    }

    v8::Maybe<bool> result = mod->InstantiateModule(cx, V8Module::CallResolve);
    if (result.IsNothing()) {
        v8Exception::ReportException(cx->GetIsolate(), &try_catch);
        exit(EXIT_FAILURE);
    }

    return mod;
}

v8::Local<v8::Value> V8Module::ExecModule(v8::Local<v8::Module> mod,
                                        v8::Local<v8::Context> cx,
                                        bool nsObject) {

    // Executing module with return value
    v8::Local<v8::Value> retValue;
    if (!mod->Evaluate(cx).ToLocal(&retValue)) {
        printf("Error evaluating module!\n");
        exit(EXIT_FAILURE);
    }

    if (nsObject)
        return mod->GetModuleNamespace();
    else
        return retValue;
}

v8::MaybeLocal<v8::Module> V8Module::CallResolve(v8::Local<v8::Context> context,
                                               v8::Local<v8::String> specifier,
                                               v8::Local<v8::FixedArray> arr,
                                               v8::Local<v8::Module> referrer) {

    v8::String::Utf8Value str(context->GetIsolate(), specifier);
    return V8Module::LoadModule(V8Module::ReadFile(*str), *str, context);
}

v8::MaybeLocal<v8::Promise>
V8Module::CallDynamic(v8::Local<v8::Context> context, v8::Local<v8::Data> host_defined_options,
                    v8::Local<v8::Value> resource_name, v8::Local<v8::String> specifier,
                    v8::Local<v8::FixedArray> import_assertions) {

    v8::Local<v8::Promise::Resolver> resolver =
            v8::Promise::Resolver::New(context).ToLocalChecked();
    v8::MaybeLocal<v8::Promise> promise(resolver->GetPromise());

    v8::String::Utf8Value name(context->GetIsolate(), specifier);
    v8::Local<v8::Module> mod =
            V8Module::CheckModule(V8Module::LoadModule(V8Module::ReadFile(*name), *name, context), context);
    v8::Local<v8::Value> retValue = V8Module::ExecModule(mod, context, true);

    resolver->Resolve(context, retValue);
    return promise;
}

void V8Module::CallMeta(v8::Local<v8::Context> context,
                      v8::Local<v8::Module> module,
                      v8::Local<v8::Object> meta) {
    // here you can set the import.meta url
    /*meta->Set(
            context,
            v8::String::NewFromUtf8(context->GetIsolate(), "url").ToLocalChecked(),
            v8::String::NewFromUtf8(context->GetIsolate(), "https://something.sh")
                    .ToLocalChecked());*/
}