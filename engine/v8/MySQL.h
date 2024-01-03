//
// Created by Twelvee on 28.12.2023.
//

#ifndef RESOURCEMOD_CONSOLE_H
#define RESOURCEMOD_CONSOLE_H

#include "libplatform/libplatform.h"
#include "v8.h"
#include "v8_utils.h"
#include "object.h"
#include "../Engine.h"

extern Engine *g_Engine;

class MySQL : public V8Object {
public:
    MySQL() {}

    ~MySQL() {}

private:
public:
    static void connect(const v8::FunctionCallbackInfo<v8::Value> &args) {
        for (int i = 0; i < args.Length(); i++) {
            v8::HandleScope handle_scope(args.GetIsolate());
            v8::String::Utf8Value str(args.GetIsolate(), args[i]);
            fprintf(stdout, "%s\n", v8str_to_cstr(str));
        }
    };

    static void disconnect(const v8::FunctionCallbackInfo<v8::Value> &args) {
        for (int i = 0; i < args.Length(); i++) {
            v8::HandleScope handle_scope(args.GetIsolate());
            v8::String::Utf8Value str(args.GetIsolate(), args[i]);
            fprintf(stderr, "%s\n", v8str_to_cstr(str));
        }
    };

    v8::Local<v8::Object> Wrap(v8::Isolate *isolate) override {
        v8::EscapableHandleScope handle_scope(isolate);

        v8::Local<v8::ObjectTemplate> data = v8::ObjectTemplate::New(isolate);
        data->SetInternalFieldCount(1);
        data->Set(
                v8::String::NewFromUtf8(isolate, "connect", v8::NewStringType::kNormal).ToLocalChecked(),
                v8::FunctionTemplate::New(isolate, &MySQL::connect));
        data->Set(
                v8::String::NewFromUtf8(isolate, "disconnect", v8::NewStringType::kNormal).ToLocalChecked(),
                v8::FunctionTemplate::New(isolate, &MySQL::disconnect));
        //create instance
        v8::Local<v8::Object> result = data->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
        //create wrapper
        v8::Local<v8::External> ptr = v8::External::New(isolate, this);
        result->SetInternalField(0, ptr);

        return handle_scope.Escape(result);
    }

};


#endif //RESOURCEMOD_CONSOLE_H
