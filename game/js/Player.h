//
// Created by Twelvee on 13.11.2023.
//

#ifndef RESOURCEMOD_PLAYER_H
#define RESOURCEMOD_PLAYER_H

#include "../cs2/cbaseplayercontroller.h"
#include "../cs2/ccsplayercontroller.h"
#include "../hooks/LegacyEvents.h"
#include "../../engine/v8/v8_utils.h"
#include <engine/igameeventsystem.h>
#include <igameevents.h>

class Player;

#define GET_PLAYER_FROM_PROPERTY_CB(info)                                                       \
    v8::Local<v8::Object> self = info.Holder();                                                 \
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));    \
    void *ptr = wrap->Value();                                                                  \
    Player *p = static_cast<Player *>(ptr);

#define CREATE_FN_PROPERTY(name, cls)                                                                                   \
        playerClass->Set(                                                                                               \
                v8::String::NewFromUtf8(info.GetIsolate(), name, v8::NewStringType::kInternalized).ToLocalChecked(),    \
                v8::FunctionTemplate::New(info.GetIsolate(), cls)                                                       \
        );

extern IGameEventManager2 *g_gameEventManager;

class Player {
public:
    Player(CCSPlayerController *c) {
        this->controller = c;
    };

    CCSPlayerController *controller;

    static void GetName(const v8::FunctionCallbackInfo<v8::Value> &info) {
        GET_PLAYER_FROM_PROPERTY_CB(info)
        const char *value = p->controller->GetPlayerName();
        v8::Local<v8::String> name = v8::String::NewFromUtf8(info.GetIsolate(), value).ToLocalChecked();
        info.GetReturnValue().Set(name);
    }

    static void GetSteamID(const v8::FunctionCallbackInfo<v8::Value> &info) {
        GET_PLAYER_FROM_PROPERTY_CB(info)
        uint64 value = p->controller->m_steamID;
        v8::Local<v8::BigInt> steamid = v8::BigInt::NewFromUnsigned(info.GetIsolate(), value);
        info.GetReturnValue().Set(steamid);
    }

    static void IsAlive(const v8::FunctionCallbackInfo<v8::Value> &info) {
        GET_PLAYER_FROM_PROPERTY_CB(info)
        bool value = p->controller->GetPawn()->IsAlive();
        v8::Local<v8::Boolean> alive = v8::Boolean::New(info.GetIsolate(), value);
        info.GetReturnValue().Set(alive);
    }

    static void GetTeam(const v8::FunctionCallbackInfo<v8::Value> &info) {
        GET_PLAYER_FROM_PROPERTY_CB(info)
        int value = p->controller->GetPawn()->m_iTeamNum;
        v8::Local<v8::Integer> team = v8::Integer::New(info.GetIsolate(), value);
        info.GetReturnValue().Set(team);
    }

    static void Slap(const v8::FunctionCallbackInfo<v8::Value> &info) {
        GET_PLAYER_FROM_PROPERTY_CB(info)
        if (!p->controller->GetPawn()->IsAlive()) {
            info.GetReturnValue().Set(v8::False(info.GetIsolate()));
            return;
        }
        int hp = info[0]->Int32Value(info.GetIsolate()->GetCurrentContext()).ToChecked();
        p->controller->GetPawn()->Slap(hp);
        info.GetReturnValue().Set(v8::True(info.GetIsolate()));
    }

    // 💅
    static void Slay(const v8::FunctionCallbackInfo<v8::Value> &info) {
        GET_PLAYER_FROM_PROPERTY_CB(info)

        if (!p->controller->GetPawn()->IsAlive()) {
            info.GetReturnValue().Set(v8::False(info.GetIsolate()));
            return;
        }

        p->controller->GetPawn()->CommitSuicide(true, true);
        info.GetReturnValue().Set(v8::True(info.GetIsolate()));
    }

    static void Respawn(const v8::FunctionCallbackInfo<v8::Value> &info) {
        GET_PLAYER_FROM_PROPERTY_CB(info)

        bool result = p->controller->Respawn();
        if (result)
            info.GetReturnValue().Set(v8::True(info.GetIsolate()));
        return info.GetReturnValue().Set(v8::False(info.GetIsolate()));
    }

    static void ChangeTeam(const v8::FunctionCallbackInfo<v8::Value> &info) {
        GET_PLAYER_FROM_PROPERTY_CB(info)
        if (!info[0]->IsInt32()) {
            info.GetIsolate()->ThrowError("Invalid team parameter. 0-3 range expected.");
            return;
        }
        int team = info[0]->Int32Value(info.GetIsolate()->GetCurrentContext()).ToChecked();
        if (team < 0 || team > 3) {
            info.GetIsolate()->ThrowError("Invalid team parameter. 0-3 range expected.");
            return;
        }
        if (info[1].IsEmpty() || info[1]->IsUndefined()) {
            p->controller->SwitchTeam(team);
        } else {
            if (info[1]->IsBoolean() && info[1]->BooleanValue(info.GetIsolate())) {
                p->controller->ChangeTeam(team);
            } else {
                p->controller->SwitchTeam(team);
            }
        }

        return info.GetReturnValue().Set(v8::True(info.GetIsolate()));
    }

    static void GetHP(v8::Local<v8::String> property,
                      const v8::PropertyCallbackInfo<v8::Value> &info) {
        GET_PLAYER_FROM_PROPERTY_CB(info)
        int value = p->controller->GetPawn()->GetHP();
        v8::Local<v8::Integer> hp = v8::Integer::New(info.GetIsolate(), value);
        info.GetReturnValue().Set(hp);
    }

    static void SetHP(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                      const v8::PropertyCallbackInfo<void> &info) {
        GET_PLAYER_FROM_PROPERTY_CB(info)
        int newHp = value->Int32Value(info.GetIsolate()->GetCurrentContext()).ToChecked();
        p->controller->GetPawn()->SetHP(newHp);
        info.GetReturnValue().Set(v8::True(info.GetIsolate()));
    }

    static void GetPlayerObject(const v8::FunctionCallbackInfo<v8::Value> &info) {
        v8::EscapableHandleScope handle_scope(info.GetIsolate());
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(
                self->GetInternalField(1)); // if it's from event - internal field of player class is 1
        void *ptr = wrap->Value();
        Player *p = static_cast<Player *>(ptr);

        v8::Local<v8::ObjectTemplate> playerClass = v8::ObjectTemplate::New(info.GetIsolate());
        playerClass->SetInternalFieldCount(1);

        CREATE_FN_PROPERTY("GetName", &Player::GetName);
        CREATE_FN_PROPERTY("GetSteamID", &Player::GetSteamID);
        CREATE_FN_PROPERTY("IsAlive", &Player::IsAlive);
        CREATE_FN_PROPERTY("Slap", &Player::Slap);
        CREATE_FN_PROPERTY("Slay", &Player::Slay);
        CREATE_FN_PROPERTY("Respawn", &Player::Respawn);
        CREATE_FN_PROPERTY("GetTeam", &Player::GetTeam);
        CREATE_FN_PROPERTY("ChangeTeam", &Player::ChangeTeam);

        // set values that can be changed in JS (public variables)
        playerClass->SetAccessor(
                v8::String::NewFromUtf8(info.GetIsolate(), "hp", v8::NewStringType::kInternalized).ToLocalChecked(),
                &Player::GetHP,
                &Player::SetHP
        );

        v8::Local<v8::Object> obj = playerClass->NewInstance(info.GetIsolate()->GetCurrentContext()).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(info.GetIsolate(), p));
        info.GetReturnValue().Set(handle_scope.Escape(obj));
    }

    static void GetAttackerObject(const v8::FunctionCallbackInfo<v8::Value> &info) {
        v8::EscapableHandleScope handle_scope(info.GetIsolate());
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(
                self->GetInternalField(2)); // if it's from event - internal field of attacker player class is 2
        void *ptr = wrap->Value();
        Player *p = static_cast<Player *>(ptr);

        v8::Local<v8::ObjectTemplate> playerClass = v8::ObjectTemplate::New(info.GetIsolate());
        playerClass->SetInternalFieldCount(1);

        CREATE_FN_PROPERTY("GetName", &Player::GetName);
        CREATE_FN_PROPERTY("GetSteamID", &Player::GetSteamID);
        CREATE_FN_PROPERTY("IsAlive", &Player::IsAlive);
        CREATE_FN_PROPERTY("Slap", &Player::Slap);
        CREATE_FN_PROPERTY("Slay", &Player::Slay);
        CREATE_FN_PROPERTY("Respawn", &Player::Respawn);
        CREATE_FN_PROPERTY("GetTeam", &Player::GetTeam);
        CREATE_FN_PROPERTY("ChangeTeam", &Player::ChangeTeam);

        // set values that can be changed in JS (public variables)
        playerClass->SetAccessor(
                v8::String::NewFromUtf8(info.GetIsolate(), "hp", v8::NewStringType::kInternalized).ToLocalChecked(),
                &Player::GetHP,
                &Player::SetHP
        );

        v8::Local<v8::Object> obj = playerClass->NewInstance(info.GetIsolate()->GetCurrentContext()).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(info.GetIsolate(), p));
        info.GetReturnValue().Set(handle_scope.Escape(obj));
    }

    static void GetAssisterObject(const v8::FunctionCallbackInfo<v8::Value> &info) {
        v8::EscapableHandleScope handle_scope(info.GetIsolate());
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(
                self->GetInternalField(3)); // if it's from event - internal field of attacker player class is 3
        void *ptr = wrap->Value();
        Player *p = static_cast<Player *>(ptr);

        v8::Local<v8::ObjectTemplate> playerClass = v8::ObjectTemplate::New(info.GetIsolate());
        playerClass->SetInternalFieldCount(1);

        CREATE_FN_PROPERTY("GetName", &Player::GetName);
        CREATE_FN_PROPERTY("GetSteamID", &Player::GetSteamID);
        CREATE_FN_PROPERTY("IsAlive", &Player::IsAlive);
        CREATE_FN_PROPERTY("Slap", &Player::Slap);
        CREATE_FN_PROPERTY("Slay", &Player::Slay);
        CREATE_FN_PROPERTY("Respawn", &Player::Respawn);
        CREATE_FN_PROPERTY("GetTeam", &Player::GetTeam);
        CREATE_FN_PROPERTY("ChangeTeam", &Player::ChangeTeam);

        // set values that can be changed in JS (public variables)
        playerClass->SetAccessor(
                v8::String::NewFromUtf8(info.GetIsolate(), "hp", v8::NewStringType::kInternalized).ToLocalChecked(),
                &Player::GetHP,
                &Player::SetHP
        );

        v8::Local<v8::Object> obj = playerClass->NewInstance(info.GetIsolate()->GetCurrentContext()).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(info.GetIsolate(), p));
        info.GetReturnValue().Set(handle_scope.Escape(obj));
    }
};

#endif //RESOURCEMOD_PLAYER_H
