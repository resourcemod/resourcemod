//
// Created by Twelvee on 10.11.2023.
//

#ifndef RESOURCEMOD_EXTERNAL_RUNTIME_H
#define RESOURCEMOD_EXTERNAL_RUNTIME_H

#include "libplatform/libplatform.h"
#include "v8.h"
#include "v8_utils.h"
#include "object.h"
#include "../../logger/logger.h"
#include "../Engine.h"

extern Engine *g_Engine;

class ExternalRuntime : public V8Object {
public:
    ExternalRuntime() {}

    ~ExternalRuntime() {}

private:
public:
    static void send(const v8::FunctionCallbackInfo<v8::Value> &args) {
        v8::HandleScope handle_scope(args.GetIsolate());
        v8::String::Utf8Value str(args.GetIsolate(), args[0]);
        const char *event_name = v8str_to_cstr(str);
        v8::String::Utf8Value d(args.GetIsolate(), args[1]);
        const char *data = v8str_to_cstr(d);
        Engine::SendNodeMessage(event_name, data);
    };

    v8::Local<v8::Object> Wrap() override {
        v8::EscapableHandleScope handle_scope(g_Engine->isolate);

        v8::Local<v8::ObjectTemplate> class_t;
        v8::Local<v8::ObjectTemplate> raw_t = v8::ObjectTemplate::New(g_Engine->isolate);
        raw_t->SetInternalFieldCount(1);
        raw_t->Set(
                v8::String::NewFromUtf8(g_Engine->isolate, "send", v8::NewStringType::kNormal).ToLocalChecked(),
                v8::FunctionTemplate::New(g_Engine->isolate, &ExternalRuntime::send));

        class_t = v8::Local<v8::ObjectTemplate>::New(g_Engine->isolate, raw_t);
        v8::Local<v8::Object> result = class_t->NewInstance(g_Engine->isolate->GetCurrentContext()).ToLocalChecked();
        v8::Local<v8::External> ptr = v8::External::New(g_Engine->isolate, this);
        result->SetInternalField(0, ptr);
        return handle_scope.Escape(result);
    }

};


#endif //RESOURCEMOD_EXTERNAL_RUNTIME_H
