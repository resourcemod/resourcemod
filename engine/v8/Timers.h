//
// Created by Twelvee on 17.11.2023.
//

#ifndef RESOURCEMOD_TIMERS_H
#define RESOURCEMOD_TIMERS_H

#include "libplatform/libplatform.h"
#include "v8.h"
#include "v8_utils.h"
#include "object.h"
#include "../Engine.h"
#include "../EventLoop.h"
#include "../../logger/logger.h"

extern Engine *g_Engine;
extern EventLoop* g_EventLoop;

class Timers : public V8Object {
public:
    Timers() {}

    ~Timers() {}

private:
public:
    static void SetTimeout(const v8::FunctionCallbackInfo<v8::Value> &args) {
        v8::HandleScope handle_scope(args.GetIsolate());
        for (const auto &item: g_Engine->plugins) {
            if (item->v8context.Get(args.GetIsolate()) == args.GetIsolate()->GetCurrentContext()) {
                v8::Local<v8::Value> callback = args[0];
                if (!callback->IsFunction()) {
                    logger::log("Invalid callback provided.");
                    return;
                }
                v8::Local<v8::Integer> timeout = args[1].As<v8::Integer>();
                Task *task = new Task(item, TaskType::TIMER, nullptr, callback.As<v8::Function>(), timeout->Int32Value(args.GetIsolate()->GetCurrentContext()).ToChecked());
                g_EventLoop->PushTask(task);
                return;
            }
        }
    };

    static void SetInterval(const v8::FunctionCallbackInfo<v8::Value> &args) {
        v8::HandleScope handle_scope(args.GetIsolate());
        for (const auto &item: g_Engine->plugins) {
            if (item->v8context.Get(args.GetIsolate()) == args.GetIsolate()->GetCurrentContext()) {
                v8::Local<v8::Value> callback = args[0];
                if (!callback->IsFunction()) {
                    logger::log("Invalid callback provided.");
                    return;
                }
                v8::Local<v8::Integer> timeout = args[1].As<v8::Integer>();
                logger::log(logger::format("Interval timeout %d", timeout->Int32Value(args.GetIsolate()->GetCurrentContext()).ToChecked()));
                Task *task = new Task(item, TaskType::INTERVAL, nullptr, callback.As<v8::Function>(), timeout->Int32Value(args.GetIsolate()->GetCurrentContext()).ToChecked());
                g_EventLoop->PushTask(task);
                return;
            }
        }
    };

    v8::Local<v8::Object> Wrap() override {
        v8::EscapableHandleScope handle_scope(g_Engine->isolate);

        v8::Local<v8::ObjectTemplate> data = v8::ObjectTemplate::New(g_Engine->isolate);
        data->SetInternalFieldCount(1);
        data->Set(
                v8::String::NewFromUtf8(g_Engine->isolate, "SetTimeout", v8::NewStringType::kNormal).ToLocalChecked(),
                v8::FunctionTemplate::New(g_Engine->isolate, &Timers::SetTimeout));
        data->Set(
                v8::String::NewFromUtf8(g_Engine->isolate, "SetInterval", v8::NewStringType::kNormal).ToLocalChecked(),
                v8::FunctionTemplate::New(g_Engine->isolate, &Timers::SetInterval));
        //create instance
        v8::Local<v8::Object> result = data->NewInstance(g_Engine->isolate->GetCurrentContext()).ToLocalChecked();
        //create wrapper
        v8::Local<v8::External> ptr = v8::External::New(g_Engine->isolate, this);
        result->SetInternalField(0, ptr);

        return handle_scope.Escape(result);
    }

};


#endif //RESOURCEMOD_TIMERS_H
