//
// Created by Twelvee on 10.11.2023.
//

#ifndef RESOURCEMOD_CONSOLE_H
#define RESOURCEMOD_CONSOLE_H

#include "libplatform/libplatform.h"
#include "v8.h"
#include "v8_utils.h"
#include "object.h"
#include "../Engine.h"

extern Engine *g_Engine;

class Console : public V8Object {
public:
    Console() {}

    ~Console() {}

private:
public:
    static void log(const v8::FunctionCallbackInfo<v8::Value> &args) {
        for (int i = 0; i < args.Length(); i++) {
            v8::HandleScope handle_scope(args.GetIsolate());
            v8::String::Utf8Value str(args.GetIsolate(), args[i]);
            fprintf(stdout, "%s\n", v8str_to_cstr(str));
        }
    };

    static void error(const v8::FunctionCallbackInfo<v8::Value> &args) {
        for (int i = 0; i < args.Length(); i++) {
            v8::HandleScope handle_scope(args.GetIsolate());
            v8::String::Utf8Value str(args.GetIsolate(), args[i]);
            fprintf(stderr, "%s\n", v8str_to_cstr(str));
        }
    };

    v8::Local<v8::Object> Wrap() override {
        v8::EscapableHandleScope handle_scope(g_Engine->isolate);

        v8::Local<v8::ObjectTemplate> data = v8::ObjectTemplate::New(g_Engine->isolate);
        data->SetInternalFieldCount(1);
        data->Set(
                v8::String::NewFromUtf8(g_Engine->isolate, "log", v8::NewStringType::kNormal).ToLocalChecked(),
                v8::FunctionTemplate::New(g_Engine->isolate, &Console::log));
        data->Set(
                v8::String::NewFromUtf8(g_Engine->isolate, "error", v8::NewStringType::kNormal).ToLocalChecked(),
                v8::FunctionTemplate::New(g_Engine->isolate, &Console::error));
        //create instance
        v8::Local<v8::Object> result = data->NewInstance(g_Engine->isolate->GetCurrentContext()).ToLocalChecked();
        //create wrapper
        v8::Local<v8::External> ptr = v8::External::New(g_Engine->isolate, this);
        result->SetInternalField(0, ptr);

        return handle_scope.Escape(result);
    }

};


#endif //RESOURCEMOD_CONSOLE_H
