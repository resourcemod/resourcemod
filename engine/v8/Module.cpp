//
// Created by Twelvee on 13.11.2023.
//

#include "Module.h"
#include "Exception.h"
#include "../Engine.h"
#include "../../logger/logger.h"

extern Engine* g_Engine;

std::string Module::ReadFile(std::string filename) {
    std::string fullModuleName = g_Engine->pluginsFolder;
    fullModuleName.append("/").append(filename);
    return Engine::GetFileContent(fullModuleName);
}

v8::MaybeLocal<v8::Module> Module::LoadModule(std::string code, v8::Local<v8::Context> ctx) {
    v8::Local<v8::String> vcode =
            v8::String::NewFromUtf8(ctx->GetIsolate(), code.c_str()).ToLocalChecked();

    v8::ScriptOrigin origin(
            ctx->GetIsolate(), vcode, 0, 0, false, -1, v8::Local<v8::Value>(), false, false, true, v8::Local<v8::Data>()
            );

    v8::Context::Scope context_scope(ctx);
    v8::ScriptCompiler::Source source(vcode, origin);
    v8::MaybeLocal<v8::Module> mod;
    mod = v8::ScriptCompiler::CompileModule(ctx->GetIsolate(), &source);

    return mod;
}

v8::Local<v8::Module> Module::CheckModule(v8::MaybeLocal<v8::Module> maybeModule,
                                          v8::Local<v8::Context> cx) {
    v8::TryCatch try_catch(cx->GetIsolate());
    v8::Local<v8::Module> mod;
    if (!maybeModule.ToLocal(&mod)) {
        printf("Error loading module!\n");
        exit(EXIT_FAILURE);
    }

    v8::Maybe<bool> result = mod->InstantiateModule(cx, Module::CallResolve);
    if (result.IsNothing()) {
        v8Exception::ReportException(cx->GetIsolate(), &try_catch);
        exit(EXIT_FAILURE);
    }

    return mod;
}

v8::Local<v8::Value> Module::ExecModule(v8::Local<v8::Module> mod,
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

v8::MaybeLocal<v8::Module> Module::CallResolve(v8::Local<v8::Context> context,
                                               v8::Local<v8::String> specifier,
                                               v8::Local<v8::FixedArray> arr,
                                               v8::Local<v8::Module> referrer) {

    v8::String::Utf8Value str(context->GetIsolate(), specifier);
    logger::log(*str);
    logger::log(*str);
    logger::log(*str);
    return Module::LoadModule(Module::ReadFile(*str), context);
}

v8::MaybeLocal<v8::Promise> Module::CallDynamic(v8::Local<v8::Context> context, v8::Local<v8::Data> host_defined_options,
                                                v8::Local<v8::Value> resource_name, v8::Local<v8::String> specifier,
                                                v8::Local<v8::FixedArray> import_assertions) {

    v8::Local<v8::Promise::Resolver> resolver =
            v8::Promise::Resolver::New(context).ToLocalChecked();
    v8::MaybeLocal<v8::Promise> promise(resolver->GetPromise());

    v8::String::Utf8Value name(context->GetIsolate(), specifier);
    v8::Local<v8::Module> mod =
            Module::CheckModule(Module::LoadModule(Module::ReadFile(*name), context), context);
    v8::Local<v8::Value> retValue = Module::ExecModule(mod, context, true);

    resolver->Resolve(context, retValue);
    return promise;
}

void Module::CallMeta(v8::Local<v8::Context> context,
                      v8::Local<v8::Module> module,
                      v8::Local<v8::Object> meta) {
    // here you can set the import.meta url
    /*meta->Set(
            context,
            v8::String::NewFromUtf8(context->GetIsolate(), "url").ToLocalChecked(),
            v8::String::NewFromUtf8(context->GetIsolate(), "https://something.sh")
                    .ToLocalChecked());*/
}