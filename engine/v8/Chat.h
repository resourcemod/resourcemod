//
// Created by Twelvee on 16.01.2024.
//

#ifndef RESOURCEMOD_CHAT_H
#define RESOURCEMOD_CHAT_H

#include "libplatform/libplatform.h"
#include "v8.h"
#include "v8_utils.h"
#include "object.h"
#include "../Engine.h"
#include "../../game/js/Player.h"
#include "../../game/ResourceMod.h"

extern Engine *g_Engine;
extern ResourceMod *g_ResourceMod;

class Chat : public V8Object {
public:
    Chat() {}

    ~Chat() {}

private:
public:

    static void ClientPrintAll(int hud_dest, const char *msg, ...) {
        va_list args;
                va_start(args, msg);
        char buf[256];
        V_vsnprintf(buf, sizeof(buf), msg, args);
                va_end(args);

        SignatureCall::UTIL_ClientPrintAll(hud_dest, buf, nullptr, nullptr, nullptr, nullptr);
        ConMsg("%s\n", buf);
    }

    static void ClientPrint(CCSPlayerController *player, int dest, const char *text, ...) {
        va_list args;
                va_start(args, text);

        char buf[256];
        V_vsnprintf(buf, sizeof(buf), text, args);
                va_end(args);

        g_ResourceMod->NextFrame([player, buf]() {
            if (player->m_hPawn() && player->m_steamID() > 0)
                SignatureCall::UTIL_ClientPrint(player, HUD_PRINTTALK, buf, nullptr, nullptr, nullptr, nullptr);
        });
    }

    // temporary does not work
    static void SendToPlayer(const v8::FunctionCallbackInfo<v8::Value> &args) {
        v8::HandleScope handle_scope(args.GetIsolate());
        if (args[0]->IsObject()) {
            v8::Local<v8::Object> self = args[0]->ToObject(args.GetIsolate()->GetCurrentContext()).ToLocalChecked();
            v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
            void *ptr = wrap->Value();
            Player *p = static_cast<Player *>(ptr);
            v8::String::Utf8Value str(args.GetIsolate(), args[1]);

            Chat::ClientPrint(p->controller, HUD_PRINTTALK, v8str_to_cstr(str));
        }
    };

    static void SendToAll(const v8::FunctionCallbackInfo<v8::Value> &args) {
        v8::HandleScope handle_scope(args.GetIsolate());
        v8::String::Utf8Value str(args.GetIsolate(), args[0]);
        Chat::ClientPrintAll(HUD_PRINTTALK, v8str_to_cstr(str));
    };

    v8::Local<v8::Object> Wrap(v8::Isolate *isolate) override {
        v8::EscapableHandleScope handle_scope(isolate);

        v8::Local<v8::ObjectTemplate> data = v8::ObjectTemplate::New(isolate);
        data->SetInternalFieldCount(1);

        data->Set(
                v8::String::NewFromUtf8(isolate, "SayToPlayer", v8::NewStringType::kNormal).ToLocalChecked(),
                v8::FunctionTemplate::New(isolate, &Chat::SendToPlayer));

        data->Set(
                v8::String::NewFromUtf8(isolate, "SayToEveryone", v8::NewStringType::kNormal).ToLocalChecked(),
                v8::FunctionTemplate::New(isolate, &Chat::SendToAll));

        v8::Local<v8::Object> result = data->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
        v8::Local<v8::External> ptr = v8::External::New(isolate, this);
        result->SetInternalField(0, ptr);

        return handle_scope.Escape(result);
    }

};


#endif //RESOURCEMOD_CHAT_H
