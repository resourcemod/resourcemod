//
// Created by Twelvee on 13.11.2023.
//

#ifndef RESOURCEMOD_PLAYER_H
#define RESOURCEMOD_PLAYER_H

#include "../cs2/cbaseplayercontroller.h"
#include "../hooks/LegacyEvents.h"

class Player {
    CBasePlayerController *controller;
public:
    Player(CBasePlayerController* c) {
        this->controller = c;
    };

    static void GetName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(1));
        void* ptr = wrap->Value();
        const char * value = static_cast<Player*>(ptr)->controller->GetPlayerName();
        v8::Local<v8::String> name = v8::String::NewFromUtf8(info.GetIsolate(), value).ToLocalChecked();
        info.GetReturnValue().Set(name);
    }

    static void GetSteamID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(1));
        void* ptr = wrap->Value();
        int value = static_cast<Player*>(ptr)->controller->m_steamID;
        v8::Local<v8::Integer> steamid = v8::Integer::New(info.GetIsolate(), value);
        info.GetReturnValue().Set(steamid);
    }

    const char* SetName() {
        return this->controller->GetPlayerName();
    }
};


#endif //RESOURCEMOD_PLAYER_H
