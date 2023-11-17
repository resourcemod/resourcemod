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
            logger::log("C is empty.");
            return true;
        }
    }
    return false;
}

void Plugin::AsyncCallback(v8::Global<v8::Function> cb) {
    v8::HandleScope handle_scope(g_Engine->isolate);
    auto c = cb.Get(g_Engine->isolate)->Call(this->v8context.Get(g_Engine->isolate), v8::Undefined(g_Engine->isolate), 0, nullptr);
    if (c.IsEmpty()) {
        logger::log("C is empty.");
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
            logger::log("C is empty.");
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

    this->v8context.Reset(g_Engine->isolate, ctx);
    v8::Context::Scope context_scope(ctx);

    // Create console object
    Console *c = new Console();
    v8::Local<v8::Object> con = c->Wrap();
    ctx->Global()->Set(
            ctx,
            v8::String::NewFromUtf8(g_Engine->isolate, "console", v8::NewStringType::kNormal).ToLocalChecked(),
            con
    );

    // Create events object
    Events *e = new Events();
    v8::Local<v8::Object> events = e->Wrap();
    ctx->Global()->Set(
            ctx,
            v8::String::NewFromUtf8(g_Engine->isolate, "events", v8::NewStringType::kNormal).ToLocalChecked(),
            events
    );

    // Create external runtime object
    ExternalRuntime *ert = new ExternalRuntime();
    v8::Local<v8::Object> external = ert->Wrap();
    ctx->Global()->Set(
            ctx,
            v8::String::NewFromUtf8(g_Engine->isolate, "external_runtime", v8::NewStringType::kNormal).ToLocalChecked(),
            external
    );

    // Create events object
    Timers *t = new Timers();
    v8::Local<v8::Object> timers = t->Wrap();
    ctx->Global()->Set(
            ctx,
            v8::String::NewFromUtf8(g_Engine->isolate, "timers", v8::NewStringType::kNormal).ToLocalChecked(),
            timers
    );

    std::string mainModuleName = this->name.c_str();
    auto contents = Module::ReadFile(mainModuleName.append("/plugin.js"));
    v8::Local<v8::Module> mod =
            Module::CheckModule(Module::LoadModule(contents, ctx), ctx);
    v8::Local<v8::Value> returned = Module::ExecModule(mod, ctx);
    v8::String::Utf8Value val(g_Engine->isolate, returned);
}