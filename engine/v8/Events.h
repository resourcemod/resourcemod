//
// Created by Twelvee on 10.11.2023.
//

#ifndef RESOURCEMOD_EVENTS_H
#define RESOURCEMOD_EVENTS_H

#include "libplatform/libplatform.h"
#include "v8.h"
#include "v8_utils.h"
#include "object.h"
#include "../../logger/logger.h"
#include "../Engine.h"

extern Engine *g_Engine;

class Events : public V8Object {
public:
    Events() {}

    ~Events() {}

private:
public:
    static void subscribe(const v8::FunctionCallbackInfo<v8::Value> &args) {
        v8::HandleScope handle_scope(args.GetIsolate());
        v8::String::Utf8Value str(args.GetIsolate(), args[0]);
        const char *event_name = v8str_to_cstr(str);
        /*v8::String::Utf8Value t(args.GetIsolate(), args[1]);
        logger::log(v8str_to_cstr(t));*/
        v8::Local<v8::Value> callback = args[1];
        if (!callback->IsFunction()) {
            logger::log("Invalid callback provided.");
            return;
        }

        auto subscribeCtxPtr = args.GetIsolate()->GetCurrentContext();
        for (auto &item: g_Engine->plugins) {
            if (subscribeCtxPtr == item->v8context) {
                item->SubscribeOnEvent(event_name, callback);
            }
        }

        return;
    };

    static void unsubscribe(const v8::FunctionCallbackInfo<v8::Value> &args) {
        logger::log("Not implemented.");
    };

    v8::Local<v8::Object> Wrap(v8::Isolate *isolate) override {
        v8::EscapableHandleScope handle_scope(isolate);

        v8::Local<v8::ObjectTemplate> class_t;
        v8::Local<v8::ObjectTemplate> raw_t = v8::ObjectTemplate::New(isolate);
        raw_t->SetInternalFieldCount(1);
        raw_t->Set(
                v8::String::NewFromUtf8(isolate, "subscribe", v8::NewStringType::kNormal).ToLocalChecked(),
                v8::FunctionTemplate::New(isolate, &Events::subscribe));
        raw_t->Set(
                v8::String::NewFromUtf8(isolate, "unsubscribe", v8::NewStringType::kNormal).ToLocalChecked(),
                v8::FunctionTemplate::New(isolate, &Events::unsubscribe));
        class_t = v8::Local<v8::ObjectTemplate>::New(isolate, raw_t);
        //create instance
        v8::Local<v8::Object> result = class_t->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
        //create wrapper
        v8::Local<v8::External> ptr = v8::External::New(isolate, this);
        result->SetInternalField(0, ptr);
        return handle_scope.Escape(result);
    }

};


#endif //RESOURCEMOD_EVENTS_H
