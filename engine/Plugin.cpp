//
// Created by Twelvee on 22.09.2023.
//

#include "Plugin.h"
#include "Engine.h"
#include "../logger/logger.h"
#include "../game/hooks/EventManager.h"
#include "v8/Console.h"
#include "v8/Events.h"
#include "v8/Module.h"
#include "v8/ExternalRuntime.h"
#include "v8/Timers.h"
#include "v8/Exception.h"
#include <filesystem>

extern Engine *g_Engine;

void Plugin::SubscribeOnEvent(std::string name, v8::Local<v8::Value> cb) {
    eventCallback *evcb = new eventCallback();
    evcb->name = name;
    evcb->cb.Reset(g_Engine->isolate, cb.As<v8::Function>());
    this->events[evcb->name] = evcb;
}

extern EventManager *g_EventManager;

bool Plugin::FireEvent(std::string name, std::string data) {
    std::map<std::string, eventCallback *>::iterator it;
    it = this->events.find(name);
    v8::HandleScope handle_scope(g_Engine->isolate);
    if (it != this->events.end()) {
        v8::Handle<v8::Value> t[] = {
                v8::String::NewFromUtf8(g_Engine->isolate, data.c_str(),
                                        v8::NewStringType::kInternalized).ToLocalChecked()
        };

        auto c = it->second->cb.Get(g_Engine->isolate)->Call(this->v8context.Get(g_Engine->isolate),
                                                             v8::Undefined(g_Engine->isolate), 1, t);
        if (c.IsEmpty()) {
            logger::log("Call is empty.");
            return true;
        }
    }
    return false;
}

void Plugin::AsyncCallback(v8::Global<v8::Function> cb) {
    v8::HandleScope handle_scope(g_Engine->isolate);
    auto c = cb.Get(g_Engine->isolate)->Call(this->v8context.Get(g_Engine->isolate), v8::Undefined(g_Engine->isolate), 0, nullptr);
    if (c.IsEmpty()) {
        logger::log("Call is empty.");
    }
}

bool Plugin::FireGameEvent(IGameEvent *event) {
    std::map<std::string, eventCallback *>::iterator it;
    it = this->events.find(event->GetName());
    v8::HandleScope handle_scope(g_Engine->isolate);
    if (it != this->events.end()) {
        auto ev = g_EventManager->events[event->GetName()](event);
        auto argv = ev->SerializeV8(
                this->v8context.Get(g_Engine->isolate));

        v8::Handle<v8::Value> t[] = {
                argv
        };
        auto c = it->second->cb.Get(g_Engine->isolate)->Call(this->v8context.Get(g_Engine->isolate),
                                                             v8::Undefined(g_Engine->isolate), 1, t);
        if (c.IsEmpty()) {
            logger::log("Call is empty.");
        }
        return ev->Prevent();
    }
    return false;
}

void Plugin::LoadPluginFS() {
    v8::Isolate::Scope isolate_scope(g_Engine->isolate);
    v8::HandleScope handle_scope(g_Engine->isolate);

    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(g_Engine->isolate);
    v8::Local<v8::Context> ctx = v8::Context::New(g_Engine->isolate, nullptr, global);

    v8::Persistent<v8::Context> persistent_context(g_Engine->isolate, ctx);

    this->v8context.Reset(g_Engine->isolate, ctx);
    v8::Context::Scope context_scope(ctx);

    // Create console object
    Console *c = new Console();
    v8::Local<v8::Object> con = c->Wrap(g_Engine->isolate);
    ctx->Global()->Set(
            ctx,
            v8::String::NewFromUtf8(g_Engine->isolate, "console", v8::NewStringType::kNormal).ToLocalChecked(),
            con
    );

    // Create events object
    Events *e = new Events();
    v8::Local<v8::Object> events = e->Wrap(g_Engine->isolate);
    ctx->Global()->Set(
            ctx,
            v8::String::NewFromUtf8(g_Engine->isolate, "events", v8::NewStringType::kNormal).ToLocalChecked(),
            events
    );

    // Create external runtime object
    if (g_Engine->isExternalRuntimeRunning) {
        ExternalRuntime *ert = new ExternalRuntime();
        v8::Local<v8::Object> external = ert->Wrap(g_Engine->isolate);
        ctx->Global()->Set(
                ctx,
                v8::String::NewFromUtf8(g_Engine->isolate, "external_runtime", v8::NewStringType::kNormal).ToLocalChecked(),
                external
        );
    }

    // Create events object
    Timers *t = new Timers();
    v8::Local<v8::Object> timers = t->Wrap(g_Engine->isolate);
    ctx->Global()->Set(
            ctx,
            v8::String::NewFromUtf8(g_Engine->isolate, "timers", v8::NewStringType::kNormal).ToLocalChecked(),
            timers
    );

    // this->LoadExtensions(); // will load extensions for the global object (mb we should replace it with commonjs modules? Or even WASM?)

    std::string mainModuleName = this->name.c_str();
    std::string file_name = mainModuleName.append("/plugin.js");
    auto contents = Module::ReadFile(file_name);

    std::string fullModuleName = g_Engine->pluginsFolder;
    fullModuleName.append("/").append(file_name);

/*    v8::Local<v8::Module> mod =
            Module::CheckModule(Module::LoadModule(contents, file_name, ctx), ctx);
    v8::Local<v8::Value> returned = Module::ExecModule(mod, ctx);
    v8::String::Utf8Value val(g_Engine->isolate, returned);*/

    v8::Local<v8::Script> script;
    v8::Local<v8::String> source =
            v8::String::NewFromUtf8(g_Engine->isolate, contents.c_str()).ToLocalChecked();
    v8::TryCatch try_catch(g_Engine->isolate);

    if (!v8::Script::Compile(ctx, source).ToLocal(&script)) {
        v8Exception::ReportException(g_Engine->isolate, &try_catch);
        logger::log("Failed to run script");
    } else {
        v8::Local<v8::Value> result;
        if (!script->Run(ctx).ToLocal(&result)) {
            v8Exception::ReportException(g_Engine->isolate, &try_catch);
            logger::log("Failed to run script");
        }
    }
}

/*void Plugin::LoadExtensions() {
    std::string f = this->folder.c_str();
    f.append("/").append("plugin.json");
    rana::value meta = rana::from_file(f);
    if (meta["extensions"].is_object()) {
        for(const auto &ext: meta["extensions"].iter_object()) {
            std::string extPath = g_Engine->extensionsFolder.c_str();
            extPath.append("/").append(ext.first).append(".dll"); // todo: linux support
            this->LoadExtension(extPath, g_Engine->isolate);
        }
    }
}*/

// Extensions will be in a few next releases
// @unused
void *Plugin::LoadExtension(std::string path, v8::Isolate *isolate) {
    const auto hd = LoadLibraryA(path.c_str());
    if (!hd) {
        logger::log(logger::format("%lu", GetLastError()));
        return nullptr;
    }

    const auto init = GetProcAddress(hd, "InitExtension");
    if (!init) {
        logger::log("Load function is undefined.");
        return nullptr;
    }

    typedef void(*InitExtension)(Engine*, Plugin*);
    const auto InitJsModule = reinterpret_cast<InitExtension>(init);
    InitJsModule(g_Engine, this);
    return nullptr;
}

v8::Global<v8::Context> Plugin::GetGlobalContext() {
    return this->v8context.Pass();
}