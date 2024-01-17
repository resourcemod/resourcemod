//
// Created by Twelvee on 17.01.2024.
//

#ifndef RESOURCEMOD_CONFIG_H
#define RESOURCEMOD_CONFIG_H

#include "libplatform/libplatform.h"
#include "v8.h"
#include "v8_utils.h"
#include "object.h"
#include "../Engine.h"

extern Engine *g_Engine;

class Config : public V8Object {
public:
    std::string plugin = "";

    Config(std::string plugin) {
        this->plugin = plugin;
    }

    ~Config() {}

private:
public:
    static void GetPluginConfig(v8::Local<v8::String> property,
                                const v8::PropertyCallbackInfo<v8::Value> &info) {
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        void *ptr = wrap->Value();
        Config *c = static_cast<Config *>(ptr);
        std::string pluginConfigPath = g_Engine->resourcemodFolder.c_str();
        std::string config_str = g_Engine->GetFileContent(
                pluginConfigPath.append("\\configs\\").append(c->plugin).append(".config.json"));
        v8::Local<v8::String> config_v8 = v8::String::NewFromUtf8(info.GetIsolate(), config_str.c_str(),
                                                                  v8::NewStringType::kInternalized).ToLocalChecked();
        v8::MaybeLocal<v8::Value> res = v8::JSON::Parse(info.GetIsolate()->GetCurrentContext(), config_v8);
        if (res.IsEmpty()) {
            info.GetIsolate()->ThrowError("Config file is empty.");
            return;
        }
        v8::Local<v8::Value> result = res.ToLocalChecked();
        if (!result->IsObject()) {
            info.GetIsolate()->ThrowError("Config file content is not an object.");
            return;
        }
        info.GetReturnValue().Set(result->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
    };

    static void SetPluginConfig(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                                const v8::PropertyCallbackInfo<void> &info) {
        info.GetIsolate()->ThrowError("On-the-fly config editing is not yet possible.");
    };

    static void GetServerConfig(v8::Local<v8::String> property,
                                const v8::PropertyCallbackInfo<v8::Value> &info) {
        std::map<std::string, const char*> string_cvars;
        std::map<std::string, const char*> int_cvars;
        std::map<std::string, const char*> float_cvars;
        ConVarHandle h = g_pCVar->FindFirstConVar();
        while(h.IsValid()) {
            ConVar *cv = g_pCVar->GetConVar(h);
            if (cv->m_eVarType == EConVarType_String) {
                logger::log(logger::format("Cvar: %s = %s", g_pCVar->GetConVar(h)->m_pszName, cv->values));
            }
            h = g_pCVar->FindNextConVar(h);
        }

    };

    static void SetServerConfig(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                                const v8::PropertyCallbackInfo<void> &info) {
        info.GetIsolate()->ThrowError("On-the-fly config editing is not yet possible.");
    };

    v8::Local<v8::Object> Wrap(v8::Isolate *isolate) override {
        v8::EscapableHandleScope handle_scope(isolate);

        v8::Local<v8::ObjectTemplate> data = v8::ObjectTemplate::New(isolate);
        data->SetInternalFieldCount(2);
        data->SetAccessor(
                v8::String::NewFromUtf8(isolate, "plugin", v8::NewStringType::kInternalized).ToLocalChecked(),
                &Config::GetPluginConfig,
                &Config::SetPluginConfig
        );

        data->SetAccessor(
                v8::String::NewFromUtf8(isolate, "server", v8::NewStringType::kInternalized).ToLocalChecked(),
                &Config::GetServerConfig,
                &Config::SetServerConfig
        );

        //create instance
        v8::Local<v8::Object> result = data->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
        //create wrapper
        v8::Local<v8::External> ptr = v8::External::New(isolate, this);
        result->SetInternalField(0, ptr);
        result->SetInternalField(1, v8::External::New(isolate, this));

        return handle_scope.Escape(result);
    }

};

#endif //RESOURCEMOD_CONFIG_H
