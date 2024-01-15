//
// Created by Twelvee on 10.10.2023.
//

#ifndef RESOURCEMOD_LEGACYEVENTS_H
#define RESOURCEMOD_LEGACYEVENTS_H

#include "../cs2/cbaseentity.h"
#include "../../logger/logger.h"
#include "../cs2/cbaseplayercontroller.h"
#include <v8.h>
#include "../../engine/Engine.h"
#include "../js/Player.h"

extern Engine *g_Engine;

#define PREVENT_SUPPORT                  \
    bool prevent = false;               \
                                        \
    void PreventSourceEvent() override { \
        this->prevent = true;                   \
    }                                   \
                                    \
    bool Prevent() override {           \
        return this->prevent;           \
    }

#define STRING_GETTER(name, cls, property) \
    static void Get##name(const v8::FunctionCallbackInfo<v8::Value> &info) { \
        v8::Local<v8::Object> self = info.Holder(); \
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0)); \
        void *ptr = wrap->Value(); \
        const char *value = static_cast<cls *>(ptr)->property; \
        v8::Local<v8::String> str = v8::String::NewFromUtf8(info.GetIsolate(), value).ToLocalChecked(); \
        info.GetReturnValue().Set(str); \
    }

#define INT_GETTER(name, cls, property) \
    static void Get##name(const v8::FunctionCallbackInfo<v8::Value> &info) { \
        v8::Local<v8::Object> self = info.Holder(); \
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0)); \
        void *ptr = wrap->Value(); \
        int value = static_cast<cls *>(ptr)->property; \
        v8::Local<v8::Integer> i = v8::Integer::New(info.GetIsolate(), value); \
        info.GetReturnValue().Set(i); \
    }

#define NUMBER_GETTER(name, cls, property) \
    static void Get##name(const v8::FunctionCallbackInfo<v8::Value> &info) { \
        v8::Local<v8::Object> self = info.Holder(); \
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0)); \
        void *ptr = wrap->Value(); \
        float value = static_cast<cls *>(ptr)->property; \
        v8::Local<v8::Number> i = v8::Number::New(info.GetIsolate(), value); \
        info.GetReturnValue().Set(i); \
    }

#define BOOL_GETTER(name, cls, property) \
    static void Get##name(const v8::FunctionCallbackInfo<v8::Value> &info) { \
        v8::Local<v8::Object> self = info.Holder(); \
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0)); \
        void *ptr = wrap->Value(); \
        bool value = static_cast<cls *>(ptr)->property; \
        v8::Local<v8::Boolean> i = v8::Boolean::New(info.GetIsolate(), value); \
        info.GetReturnValue().Set(i); \
    }

#define SET_GETTER(t, getter, cls)                                                       \
        t->Set(                                                                          \
            v8::String::NewFromUtf8(g_Engine->isolate, #getter,                                \
            v8::NewStringType::kInternalized).ToLocalChecked(),                                 \
                    v8::FunctionTemplate::New(g_Engine->isolate, cls) \
        );

class RMEvent {
public:
    const char *event_name;

    virtual v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) = 0;

    virtual void PreventSourceEvent() = 0;

    virtual bool Prevent() = 0;

    static void Cancel(const v8::FunctionCallbackInfo<v8::Value> &info) {
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        void *ptr = wrap->Value();
        static_cast<RMEvent *>(ptr)->PreventSourceEvent();
        info.GetReturnValue().Set(v8::True(info.GetIsolate()));
    }
};

class server_spawn : public RMEvent {
public:
    server_spawn(IGameEvent *event) {
        this->hostname = event->GetString("hostname");
        this->address = event->GetString("address");
        this->port = event->GetInt("port");
        this->game = event->GetString("game");
        this->mapname = event->GetString("mapname");
        this->addonname = event->GetString("addonname");
        this->maxplayers = event->GetInt("maxplayers");
        this->os = event->GetString("os");
        this->dedicated = event->GetBool("dedicated");
        this->password = event->GetBool("password");
    };
    const char *event_name = "server_spawn";
    const char *hostname;
    const char *address;
    int port;
    const char *game;
    const char *mapname;
    const char *addonname;
    int maxplayers;
    const char *os;
    bool dedicated;
    bool password;

    STRING_GETTER(Hostname, server_spawn, hostname);

    STRING_GETTER(Address, server_spawn, address);

    STRING_GETTER(Game, server_spawn, game);

    STRING_GETTER(Map, server_spawn, mapname);

    STRING_GETTER(Addon, server_spawn, addonname);

    STRING_GETTER(Os, server_spawn, os);

    INT_GETTER(Port, server_spawn, port);

    INT_GETTER(MaxPlayers, server_spawn, maxplayers);

    BOOL_GETTER(IsDedicated, server_spawn, dedicated);

    BOOL_GETTER(HasPassword, server_spawn, password);

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetHostname, server_spawn::GetHostname);
        SET_GETTER(t, GetAddress, server_spawn::GetAddress);
        SET_GETTER(t, GetGame, server_spawn::GetGame);
        SET_GETTER(t, GetMap, server_spawn::GetMap);
        SET_GETTER(t, GetAddon, server_spawn::GetAddon);
        SET_GETTER(t, GetOS, server_spawn::GetOs);

        SET_GETTER(t, GetPort, server_spawn::GetPort);
        SET_GETTER(t, GetMaxPlayers, server_spawn::GetMaxPlayers);

        SET_GETTER(t, HasPassword, server_spawn::GetHasPassword);
        SET_GETTER(t, IsDedicated, server_spawn::GetHasPassword);

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};


class server_pre_shutdown : public RMEvent {
public:
    server_pre_shutdown(IGameEvent *event) {
        this->reason = event->GetString("reason");
    };
    const char *event_name = "server_pre_shutdown";
    const char *reason;

    STRING_GETTER(Reason, server_pre_shutdown, reason);

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetReason, server_pre_shutdown::GetReason);

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};


class server_shutdown : public RMEvent {
public:
    server_shutdown(IGameEvent *event) {
        this->reason = event->GetString("reason");
    };
    const char *event_name = "server_shutdown";
    const char *reason;

    STRING_GETTER(Reason, server_shutdown, reason);

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetReason, server_shutdown::GetReason);

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};


class server_message : public RMEvent {
public:
    server_message(IGameEvent *event) {
        this->text = event->GetString("text");
    };
    const char *event_name = "server_message";
    const char *text;

    STRING_GETTER(Text, server_message, text);

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetText, server_message::GetText);

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class server_cvar : public RMEvent {
public:
    server_cvar(IGameEvent *event) {
        this->cvarname = event->GetString("cvarname");
        this->cvarvalue = event->GetString("cvarvalue");
    };
    const char *event_name = "server_cvar";
    const char *cvarname;
    const char *cvarvalue;

    STRING_GETTER(CvarName, server_cvar, cvarname);

    STRING_GETTER(CvarValue, server_cvar, cvarvalue);

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetCvarName, server_cvar::GetCvarName);
        SET_GETTER(t, GetCvarValue, server_cvar::GetCvarValue);

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class player_activate : public RMEvent {
public:
    player_activate(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "player_activate";
    CEntityInstance *userid; //playercontroller

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};


class player_connect_full : public RMEvent {
public:
    player_connect_full(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };

    const char *event_name = "player_connect_full";
    CEntityInstance *userid; //playercontroller

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};


class player_full_update : public RMEvent {
public:
    player_full_update(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "player_full_update";
    CEntityInstance *userid; //playercontroller

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));
        return obj;
    };
};

class player_connect : public RMEvent {
public:
    player_connect(IGameEvent *event) {
        this->name = event->GetString("name");
        this->userid = event->GetPlayerController("userid");
        this->networkid = event->GetString("networkid");
        this->xuid = event->GetInt("xuid");
        this->address = event->GetString("address");
        this->bot = event->GetBool("bot");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "player_connect";
    const char *name;
    CEntityInstance *userid; //playercontroller
    const char *networkid; // i.e. steam id
    int xuid; // steamid
    const char *address; //ip:port
    bool bot;

    Player *player;

    STRING_GETTER(Name, player_connect, name);

    STRING_GETTER(NetworkID, player_connect, networkid);

    STRING_GETTER(Address, player_connect, address);

    INT_GETTER(Xuid, player_connect, xuid);

    BOOL_GETTER(IsBot, player_connect, bot);

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);

        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, IsBot, player_connect::GetIsBot)
        SET_GETTER(t, GetName, player_connect::GetName)
        SET_GETTER(t, GetNetworkID, player_connect::GetNetworkID)
        SET_GETTER(t, GetAddress, player_connect::GetAddress)
        SET_GETTER(t, GetXuid, player_connect::GetXuid)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };

};

class player_disconnect : public RMEvent {
public:
    player_disconnect(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->reason = event->GetInt("reason");
        this->name = event->GetString("name");
        this->networkid = event->GetString("networkid");
        this->xuid = event->GetInt("xuid");
        this->PlayerID = event->GetInt("PlayerID");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "player_disconnect";
    CEntityInstance *userid; //playercontroller
    int reason;
    const char *name;
    const char *networkid; // i.e. steam id
    int xuid; // steamid
    int PlayerID; //?

    Player *player;

    STRING_GETTER(Name, player_disconnect, name);

    STRING_GETTER(NetworkID, player_disconnect, networkid);

    INT_GETTER(Xuid, player_disconnect, xuid);

    INT_GETTER(Reason, player_disconnect, reason);

    INT_GETTER(PlayerID, player_disconnect, PlayerID);

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);

        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetXuid, player_disconnect::GetXuid)
        SET_GETTER(t, GetName, player_disconnect::GetName)
        SET_GETTER(t, GetNetworkID, player_disconnect::GetNetworkID)
        SET_GETTER(t, GetReason, player_disconnect::GetReason)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };

};


class player_info : public RMEvent {
public:
    player_info(IGameEvent *event) {
        this->name = event->GetString("name");
        this->userid = event->GetPlayerController("userid");
        this->steamid = event->GetInt("steamid");
        this->bot = event->GetBool("bot");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "player_info";
    const char *name;
    CEntityInstance *userid;//playercontroller
    int steamid;
    bool bot;

    Player *player;

    STRING_GETTER(Name, player_info, name);

    INT_GETTER(SteamID, player_info, steamid);

    BOOL_GETTER(IsBot, player_info, bot);

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);

        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetSteamID, player_info::GetSteamID)
        SET_GETTER(t, GetName, player_info::GetName)
        SET_GETTER(t, IsBot, player_info::GetIsBot)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class player_spawn : public RMEvent {
public:
    player_spawn(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "player_spawn";
    CEntityInstance *userid; //playercontroller
    CEntityInstance *userid_pawn; //strict_ehandle

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};


class player_team : public RMEvent {
public:
    player_team(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->team = event->GetInt("team");
        this->oldteam = event->GetInt("oldteam");
        this->disconnect = event->GetBool("disconnect");
        this->silent = event->GetBool("silent");
        this->isbot = event->GetBool("isbot");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "player_team";
    CEntityInstance *userid;//playercontroller
    int team;
    int oldteam;
    bool disconnect;
    bool silent;
    bool isbot;
    CEntityInstance *userid_pawn; //strict_ehandle

    INT_GETTER(Team, player_team, team);

    INT_GETTER(OldTeam, player_team, oldteam);

    BOOL_GETTER(IsDisconnected, player_team, disconnect);

    BOOL_GETTER(IsSilent, player_team, silent);

    BOOL_GETTER(IsBot, player_team, isbot);

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetTeam, player_team::GetTeam)
        SET_GETTER(t, GetOldTeam, player_team::GetOldTeam)
        SET_GETTER(t, IsDisconnected, player_team::GetIsDisconnected)
        SET_GETTER(t, IsSilent, player_team::GetIsSilent)
        SET_GETTER(t, IsBot, player_team::GetIsBot)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class player_changename : public RMEvent {
public:
    player_changename(IGameEvent *event) {
        this->oldname = event->GetString("oldname");
        this->newname = event->GetString("newname");
    };
    const char *event_name = "player_changename";
    const char *oldname;
    const char *newname;

    STRING_GETTER(OldName, player_changename, oldname);

    STRING_GETTER(NewName, player_changename, newname);

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetOldName, player_changename::GetOldName)
        SET_GETTER(t, GetNewName, player_changename::GetNewName)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class player_hurt : public RMEvent {
public:
    player_hurt(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->attacker = event->GetPlayerController("attacker");
        this->health = event->GetInt("health");
        this->armor = event->GetInt("armor");
        this->weapon = event->GetString("weapon");
        this->dmg_health = event->GetInt("dmg_health");
        this->dmg_armor = event->GetInt("dmg_armor");
        this->hitgroup = event->GetInt("hitgroup");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
        this->attacker_pawn = event->GetPlayerPawn("attacker_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);

        CBasePlayerController *a_controller = (CBasePlayerController *) this->attacker;
        this->attacker_player = new Player(a_controller);
    };
    const char *event_name = "player_hurt";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *attacker; //playercontroller
    int health;
    int armor;
    const char *weapon;
    int dmg_health;
    int dmg_armor;
    int hitgroup;
    CEntityInstance *userid_pawn; //strict_ehandle
    CEntityInstance *attacker_pawn; //strict_ehandle

    Player *player;
    Player *attacker_player;

    INT_GETTER(HP, player_hurt, health)

    INT_GETTER(Armor, player_hurt, armor)

    STRING_GETTER(Weapon, player_hurt, weapon)

    INT_GETTER(DamageHP, player_hurt, dmg_health)

    INT_GETTER(DamageArmor, player_hurt, dmg_armor)

    INT_GETTER(HitGroup, player_hurt, hitgroup)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(3);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetAttacker, Player::GetAttackerObject)

        SET_GETTER(t, GetHP, player_hurt::GetHP)
        SET_GETTER(t, GetArmor, player_hurt::GetArmor)
        SET_GETTER(t, GetWeapon, player_hurt::GetWeapon)
        SET_GETTER(t, GetDamageHP, player_hurt::GetDamageHP)
        SET_GETTER(t, GetDamageArmor, player_hurt::GetDamageArmor)
        SET_GETTER(t, GetHitGroup, player_hurt::GetHitGroup)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));
        obj->SetInternalField(2, v8::External::New(g_Engine->isolate, this->attacker_player));

        return obj;
    };
};


class player_chat : public RMEvent {
public:
    player_chat(IGameEvent *event) {
        this->teamonly = event->GetBool("teamonly");
        this->userid = event->GetInt("userid");
        this->text = event->GetString("text");
    };
    const char *event_name = "player_chat";

    bool teamonly;
    int userid;
    const char *text;

    BOOL_GETTER(IsTeamOnly, player_chat, teamonly)

    INT_GETTER(UserID, player_chat, userid)

    STRING_GETTER(Text, player_chat, text)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        //SET_GETTER(t, GetPlayer, Player::GetPlayerObject) todo: convert userid to Player class

        SET_GETTER(t, IsTeamOnly, player_chat::GetIsTeamOnly)
        SET_GETTER(t, GetUserID, player_chat::GetUserID)
        SET_GETTER(t, GetText, player_chat::GetText)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};


class teamplay_broadcast_audio : public RMEvent {
public:
    teamplay_broadcast_audio(IGameEvent *event) {
        this->team = event->GetInt("team");
        this->sound = event->GetString("sound");
    };
    const char *event_name = "teamplay_broadcast_audio";
    int team;
    const char *sound;

    INT_GETTER(Team, teamplay_broadcast_audio, team)

    STRING_GETTER(Sound, teamplay_broadcast_audio, sound)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetTeam, teamplay_broadcast_audio::GetTeam)
        SET_GETTER(t, GetSound, teamplay_broadcast_audio::GetSound)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class player_stats_updated : public RMEvent {
public:
    player_stats_updated(IGameEvent *event) {
        this->forceupload = event->GetBool("forceupload");
    };
    const char *event_name = "player_stats_updated";
    bool forceupload;

    BOOL_GETTER(IsForceUpload, player_stats_updated, forceupload)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, IsForceUpload, player_stats_updated::GetIsForceUpload)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class team_info : public RMEvent {
public:
    team_info(IGameEvent *event) {
        this->teamid = event->GetInt("teamid");
        this->teamname = event->GetString("teamname");
    };
    const char *event_name = "team_info";
    int teamid;
    const char *teamname;

    INT_GETTER(TeamID, team_info, teamid)

    STRING_GETTER(TeamName, team_info, teamname)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetTeamID, team_info::GetTeamID)
        SET_GETTER(t, GetTeamName, team_info::GetTeamName)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

// Team score changed
class team_score : public RMEvent {
public:
    team_score(IGameEvent *event) {
        this->teamid = event->GetInt("teamid");
        this->score = event->GetInt("score");
    };
    const char *event_name = "team_score";
    int teamid;
    int score;

    INT_GETTER(TeamID, team_score, teamid)

    INT_GETTER(Score, team_score, score)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetTeamID, team_score::GetTeamID)
        SET_GETTER(t, GetScore, team_score::GetScore)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class map_shutdown : public RMEvent {
public:
    map_shutdown(IGameEvent *event) {
    };
    const char *event_name = "map_shutdown";

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);
        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};


class map_transition : public RMEvent {
public:
    map_transition(IGameEvent *event) {
    };
    const char *event_name = "map_transition";

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);
        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};


class hostname_changed : public RMEvent {
public:
    hostname_changed(IGameEvent *event) {
        this->hostname = event->GetString("hostname");
    };
    const char *event_name = "hostname_changed";
    const char *hostname;

    STRING_GETTER(Hostname, hostname_changed, hostname)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetHostname, hostname_changed::GetHostname)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};


class difficulty_changed : public RMEvent {
public:
    difficulty_changed(IGameEvent *event) {
        this->newDifficulty = event->GetInt("newDifficulty");
        this->oldDifficulty = event->GetInt("oldDifficulty");
        this->strDifficulty = event->GetString("strDifficulty");
    };
    const char *event_name = "difficulty_changed";
    int newDifficulty;
    int oldDifficulty;
    const char *strDifficulty;

    INT_GETTER(NewDifficulty, difficulty_changed, newDifficulty)

    INT_GETTER(OldDifficulty, difficulty_changed, oldDifficulty)

    STRING_GETTER(StrDifficulty, difficulty_changed, strDifficulty)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetStrDifficulty, difficulty_changed::GetStrDifficulty)
        SET_GETTER(t, GetNewDifficulty, difficulty_changed::GetNewDifficulty)
        SET_GETTER(t, GetOldDifficulty, difficulty_changed::GetOldDifficulty)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class game_message : public RMEvent {
public:
    game_message(IGameEvent *event) {
        this->target = event->GetInt("target");
        this->text = event->GetString("text");
    };
    const char *event_name = "game_message";
    int target; //0- console, 1 - hud
    const char *text;

    INT_GETTER(Target, game_message, target)

    STRING_GETTER(Text, game_message, text)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetTarget, game_message::GetTarget)
        SET_GETTER(t, GetText, game_message::GetText)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class game_newmap : public RMEvent {
public:
    game_newmap(IGameEvent *event) {
        this->mapname = event->GetString("mapname");
    };
    const char *event_name = "game_newmap";
    const char *mapname;

    STRING_GETTER(MapName, game_newmap, mapname)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetMapName, game_newmap::GetMapName)
        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class round_start : public RMEvent {
public:
    round_start(IGameEvent *event) {
        this->timelimit = event->GetInt("timelimit");
        this->fraglimit = event->GetInt("fraglimit");
        this->objective = event->GetString("objective");
    };
    const char *event_name = "round_start";
    int timelimit;
    int fraglimit;
    const char *objective;

    INT_GETTER(TimeLimit, round_start, timelimit)

    INT_GETTER(FragLimit, round_start, fraglimit)

    STRING_GETTER(Objective, round_start, objective)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetTimeLimit, round_start::GetTimeLimit)
        SET_GETTER(t, GetFragLimit, round_start::GetFragLimit)
        SET_GETTER(t, GetObjective, round_start::GetObjective)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class round_end : public RMEvent {
public:
    round_end(IGameEvent *event) {
        this->winner = event->GetInt("winner");
        this->reason = event->GetInt("reason");
        this->message = event->GetString("message");
        this->legacy = event->GetInt("legacy");
        this->player_count = event->GetInt("player_count");
        this->nomusic = event->GetInt("nomusic");
    };
    const char *event_name = "round_end";
    int winner;
    int reason;
    const char *message;
    int legacy;
    int player_count;
    int nomusic;

    INT_GETTER(Winner, round_end, winner)

    INT_GETTER(Reason, round_end, reason)

    STRING_GETTER(Message, round_end, message)

    INT_GETTER(Legacy, round_end, legacy)

    INT_GETTER(PlayerCount, round_end, player_count)

    INT_GETTER(NoMusic, round_end, nomusic)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetWinner, round_end::GetWinner)
        SET_GETTER(t, GetReason, round_end::GetReason)
        SET_GETTER(t, GetMessage, round_end::GetMessage)
        SET_GETTER(t, GetLegacy, round_end::GetLegacy)
        SET_GETTER(t, GetPlayerCount, round_end::GetPlayerCount)
        SET_GETTER(t, GetNoMusic, round_end::GetNoMusic)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};


class round_freeze_end : public RMEvent {
public:
    round_freeze_end(IGameEvent *event) {
    };
    const char *event_name = "round_freeze_end";

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);
        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class player_death : public RMEvent {
public:
    player_death(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->attacker = event->GetPlayerController("attacker");
        this->assister = event->GetPlayerController("assister");
        this->assistedflash = event->GetBool("assistedflash");
        this->weapon = event->GetString("weapon");
        this->weapon_itemid = event->GetString("weapon_itemid");
        this->weapon_fauxitemid = event->GetString("weapon_fauxitemid");
        this->weapon_originalowner_xuid = event->GetString("weapon_originalowner_xuid");
        this->headshot = event->GetBool("headshot");
        this->dominated = event->GetInt("dominated");
        this->revenge = event->GetInt("revenge");
        this->wipe = event->GetInt("wipe");
        this->penetrated = event->GetInt("penetrated");
        this->noreplay = event->GetBool("noreplay");
        this->noscope = event->GetBool("noscope");
        this->thrusmoke = event->GetBool("thrusmoke");
        this->attackerblind = event->GetBool("attackerblind");
        this->distance = event->GetFloat("distance");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
        this->attacker_pawn = event->GetPlayerPawn("attacker_pawn");
        this->assister_pawn = event->GetPlayerPawn("assister_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);

        CBasePlayerController *at_controller = (CBasePlayerController *) this->attacker;
        this->attacker_player = new Player(at_controller);

        CBasePlayerController *as_controller = (CBasePlayerController *) this->assister;
        this->assister_player = new Player(as_controller);
    };
    const char *event_name = "player_death";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *attacker;//playercontroller
    CEntityInstance *assister;//playercontroller
    bool assistedflash;
    const char *weapon;
    const char *weapon_itemid;
    const char *weapon_fauxitemid;
    const char *weapon_originalowner_xuid;
    bool headshot;
    int dominated;
    int revenge;
    int wipe;
    int penetrated;
    bool noreplay;
    bool noscope;
    bool thrusmoke;
    bool attackerblind;
    float distance;
    CEntityInstance *userid_pawn;//strict_ehandle
    CEntityInstance *attacker_pawn;//strict_ehandle
    CEntityInstance *assister_pawn;//strict_ehandle

    BOOL_GETTER(IsFlashAssisted, player_death, assistedflash)

    STRING_GETTER(Weapon, player_death, weapon)

    STRING_GETTER(WeaponItemID, player_death, weapon_itemid)

    STRING_GETTER(WeaponFauxItemID, player_death, weapon_fauxitemid)

    STRING_GETTER(WeaponOriginalOwnerXUID, player_death, weapon_originalowner_xuid)

    BOOL_GETTER(IsHeadshot, player_death, headshot)

    INT_GETTER(Dominated, player_death, dominated)

    INT_GETTER(Revenge, player_death, revenge)

    INT_GETTER(Wipe, player_death, wipe)

    INT_GETTER(Penetrated, player_death, penetrated)

    BOOL_GETTER(IsNoReplay, player_death, noreplay)

    BOOL_GETTER(IsNoScope, player_death, noscope)

    BOOL_GETTER(IsThroughSmoke, player_death, thrusmoke)

    BOOL_GETTER(IsAttackerWasBlind, player_death, attackerblind)

    NUMBER_GETTER(Distance, player_death, distance)

    Player *player;
    Player *attacker_player;
    Player *assister_player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(4);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetAttacker, Player::GetAttackerObject)
        SET_GETTER(t, GetAssister, Player::GetAssisterObject)

        SET_GETTER(t, IsFlashAssisted, player_death::GetIsFlashAssisted)
        SET_GETTER(t, GetWeapon, player_death::GetWeapon)
        SET_GETTER(t, GetWeaponItemID, player_death::GetWeaponItemID)
        SET_GETTER(t, GetWeaponFauxItemID, player_death::GetWeaponFauxItemID)
        SET_GETTER(t, GetWeaponOriginalOwnerXUID, player_death::GetWeaponOriginalOwnerXUID)
        SET_GETTER(t, IsHeadshot, player_death::GetIsHeadshot)
        SET_GETTER(t, IsDominated, player_death::GetDominated)
        SET_GETTER(t, IsRevenge, player_death::GetRevenge)
        SET_GETTER(t, IsWipe, player_death::GetWipe)
        SET_GETTER(t, IsPenetrated, player_death::GetPenetrated)
        SET_GETTER(t, NoReplay, player_death::GetIsNoReplay)
        SET_GETTER(t, IsNoScope, player_death::GetIsNoScope)
        SET_GETTER(t, IsThroughSmoke, player_death::GetIsThroughSmoke)
        SET_GETTER(t, IsAttackerWasBlind, player_death::GetIsAttackerWasBlind)
        SET_GETTER(t, GetDistance, player_death::GetDistance)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));
        obj->SetInternalField(2, v8::External::New(g_Engine->isolate, this->attacker_player));
        obj->SetInternalField(3, v8::External::New(g_Engine->isolate, this->assister_player));

        return obj;
    };
};


class player_footstep : public RMEvent {
public:
    player_footstep(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "player_footstep";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *userid_pawn;//strict_ehandle

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class player_hintmessage : public RMEvent {
public:
    player_hintmessage(IGameEvent *event) {
        this->hintmessage = event->GetString("hintmessage");
    };
    const char *event_name = "player_hintmessage";
    const char *hintmessage;

    STRING_GETTER(HintMessage, player_hintmessage, hintmessage)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetMessage, player_hintmessage::GetHintMessage)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class break_breakable : public RMEvent {
public:
    break_breakable(IGameEvent *event) {
        this->entindex = event->GetInt("entindex");
        this->userid = event->GetPlayerController("userid");
        this->material = event->GetInt("material");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "break_breakable";
    int entindex;
    CEntityInstance *userid; //player_pawn
    int material; // BREAK_GLASS, BREAK_WOOD etc TODO: check what acutually happened
    CEntityInstance *userid_pawn; //strict_ehandle

    INT_GETTER(EntityIndex, break_breakable, entindex)

    INT_GETTER(Material, break_breakable, material)

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        SET_GETTER(t, GetEntityIndex, break_breakable::GetEntityIndex)
        SET_GETTER(t, GetMaterial, break_breakable::GetMaterial)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class break_prop : public RMEvent {
public:
    break_prop(IGameEvent *event) {
        this->entindex = event->GetInt("entindex");
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "break_prop";
    int entindex;
    CEntityInstance *userid;// player_pawn
    CEntityInstance *userid_pawn; //strict_ehandle

    INT_GETTER(EntityIndex, break_prop, entindex)

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        SET_GETTER(t, GetEntityIndex, break_prop::GetEntityIndex)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class entity_killed : public RMEvent {
public:
    entity_killed(IGameEvent *event) {
        this->entindex_killed = event->GetInt("entindex_killed");
        this->entindex_attacker = event->GetInt("entindex_attacker");
        this->entindex_inflictor = event->GetInt("entindex_inflictor");
        this->damagebits = event->GetInt("damagebits");
    };
    const char *event_name = "entity_killed";
    int entindex_killed;
    int entindex_attacker;
    int entindex_inflictor;
    int damagebits;

    INT_GETTER(KilledEntityIndex, entity_killed, entindex_killed)

    INT_GETTER(AttackerEntityIndex, entity_killed, entindex_attacker)

    INT_GETTER(InflictorEntityIndex, entity_killed, entindex_inflictor)

    INT_GETTER(DamageBits, entity_killed, damagebits)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetKilledEntityIndex, entity_killed::GetKilledEntityIndex)
        SET_GETTER(t, GetAttackerEntityIndex, entity_killed::GetAttackerEntityIndex)
        SET_GETTER(t, GetInflictorEntityIndex, entity_killed::GetInflictorEntityIndex)
        SET_GETTER(t, GetDamageBits, entity_killed::GetDamageBits)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class door_close : public RMEvent {
public:
    door_close(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->checkpoint = event->GetBool("checkpoint");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "door_close";
    CEntityInstance *userid; //player_pawn
    bool checkpoint;
    CEntityInstance *userid_pawn;//strict_ehandle

    BOOL_GETTER(IsCheckPoint, door_close, checkpoint)

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        SET_GETTER(t, IsCheckPoint, door_close::GetIsCheckPoint)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class vote_started : public RMEvent {
public:
    vote_started(IGameEvent *event) {
        this->issue = event->GetString("issue");
        this->param1 = event->GetString("param1");
        this->team = event->GetInt("team");
        this->initiator = event->GetInt("initiator");
    };
    const char *event_name = "vote_started";
    const char *issue;
    const char *param1;
    int team;
    int initiator;

    STRING_GETTER(Issue, vote_started, issue)
    STRING_GETTER(Param1, vote_started, param1)
    INT_GETTER(Team, vote_started, team)
    INT_GETTER(Initiator, vote_started, initiator)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetIssue, vote_started::GetIssue)
        SET_GETTER(t, GetParam1, vote_started::GetParam1)
        SET_GETTER(t, GetTeam, vote_started::GetTeam)
        SET_GETTER(t, GetInitiator, vote_started::GetInitiator)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class vote_failed : public RMEvent {
public:
    vote_failed(IGameEvent *event) {
        this->team = event->GetInt("team");
    };
    const char *event_name = "vote_failed";
    int team;

    INT_GETTER(Team, vote_failed, team)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetTeam, vote_failed::GetTeam)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class vote_passed : public RMEvent {
public:
    vote_passed(IGameEvent *event) {
        this->details = event->GetString("details");
        this->param1 = event->GetString("param1");
        this->team = event->GetInt("team");
    };
    const char *event_name = "vote_passed";
    const char *details;
    const char *param1;
    int team;

    STRING_GETTER(Details, vote_passed, details)
    STRING_GETTER(Param1, vote_passed, param1)
    INT_GETTER(Team, vote_started, team)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetDetails, vote_passed::GetDetails)
        SET_GETTER(t, GetParam1, vote_passed::GetParam1)
        SET_GETTER(t, GetTeam, vote_passed::GetTeam)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class vote_changed : public RMEvent {
public:
    vote_changed(IGameEvent *event) {
        this->vote_option1 = event->GetInt("vote_option1");
        this->vote_option2 = event->GetInt("vote_option2");
        this->vote_option3 = event->GetInt("vote_option3");
        this->vote_option4 = event->GetInt("vote_option4");
        this->vote_option5 = event->GetInt("vote_option5");
        this->potentialVotes = event->GetInt("potentialVotes");
    };
    const char *event_name = "vote_changed";
    int vote_option1;
    int vote_option2;
    int vote_option3;
    int vote_option4;
    int vote_option5;
    int potentialVotes;

    INT_GETTER(Option1, vote_changed, vote_option1)
    INT_GETTER(Option2, vote_changed, vote_option2)
    INT_GETTER(Option3, vote_changed, vote_option3)
    INT_GETTER(Option4, vote_changed, vote_option4)
    INT_GETTER(Option5, vote_changed, vote_option5)
    INT_GETTER(PotentialVotes, vote_changed, potentialVotes)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetOption1, vote_changed::GetOption1)
        SET_GETTER(t, GetOption2, vote_changed::GetOption2)
        SET_GETTER(t, GetOption3, vote_changed::GetOption3)
        SET_GETTER(t, GetOption4, vote_changed::GetOption4)
        SET_GETTER(t, GetOption5, vote_changed::GetOption5)
        SET_GETTER(t, GetPotentialVotes, vote_changed::GetPotentialVotes)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class vote_cast_yes : public RMEvent {
public:
    vote_cast_yes(IGameEvent *event) {
        this->team = event->GetInt("team");
        this->entityid = event->GetInt("entityid");
    };
    const char *event_name = "vote_cast_yes";
    int team;
    int entityid;

    INT_GETTER(Team, vote_cast_yes, team)
    INT_GETTER(EntityID, vote_cast_yes, entityid)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetTeam, vote_cast_yes::GetTeam)
        SET_GETTER(t, GetEntityID, vote_cast_yes::GetEntityID)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class vote_cast_no : public RMEvent {
public:
    vote_cast_no(IGameEvent *event) {
        this->team = event->GetInt("team");
        this->entityid = event->GetInt("entityid");
    };
    const char *event_name = "vote_cast_no";
    int team;
    int entityid;

    INT_GETTER(Team, vote_cast_no, team)
    INT_GETTER(EntityID, vote_cast_no, entityid)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetTeam, vote_cast_no::GetTeam)
        SET_GETTER(t, GetEntityID, vote_cast_no::GetEntityID)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class achievement_event : public RMEvent {
public:
    achievement_event(IGameEvent *event) {
        this->achievement_name = event->GetString("achievement_name");
        this->cur_val = event->GetInt("cur_val");
        this->max_val = event->GetInt("max_val");
    };
    const char *event_name = "achievement_event";
    const char *achievement_name;
    int cur_val;
    int max_val;

    STRING_GETTER(AchievementName, achievement_event, achievement_name)
    INT_GETTER(CurrentValue, achievement_event, cur_val)
    INT_GETTER(MaxValue, achievement_event, max_val)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetAchievementName, achievement_event::GetAchievementName)
        SET_GETTER(t, GetCurrentValue, achievement_event::GetCurrentValue)
        SET_GETTER(t, GetMaxValue, achievement_event::GetMaxValue)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class achievement_earned : public RMEvent {
public:
    achievement_earned(IGameEvent *event) {
        this->player = event->GetPlayerController("player");
        this->achievement = event->GetInt("achievement");

        CBasePlayerController *controller = (CBasePlayerController *) this->player;
        this->_player = new Player(controller);
    };
    const char *event_name = "achievement_earned";
    CEntityInstance *player; //playercontroller
    int achievement;
    Player* _player;

    INT_GETTER(Achievement, achievement_earned, achievement)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetAchievement, achievement_earned::GetAchievement)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->_player));

        return obj;
    };
};

class spec_target_updated : public RMEvent {
public:
    spec_target_updated(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->target = event->GetInt("target");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "spec_target_updated";
    CEntityInstance *userid; //playercontroller
    int target;
    CEntityInstance *userid_pawn; //strict_ehandle

    Player* player;

    INT_GETTER(Target, spec_target_updated, target)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetTarget, spec_target_updated::GetTarget)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class spec_mode_updated : public RMEvent {
public:
    spec_mode_updated(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "spec_mode_updated";
    CEntityInstance *userid; //playercontroller

    Player* player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class entity_visible : public RMEvent {
public:
    entity_visible(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->subject = event->GetInt("subject");
        this->classname = event->GetString("classname");
        this->entityname = event->GetString("entityname");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "entity_visible";
    CEntityInstance *userid;//playercontroller
    int subject;
    const char *classname;
    const char *entityname;

    Player* player;

    INT_GETTER(Subject, entity_visible, subject)
    STRING_GETTER(ClassName, entity_visible, classname)
    STRING_GETTER(EntityName, entity_visible, entityname)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetSubject, entity_visible::GetSubject)
        SET_GETTER(t, GetClassName, entity_visible::GetClassName)
        SET_GETTER(t, GetEntityName, entity_visible::GetEntityName)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class items_gifted : public RMEvent {
public:
    items_gifted(IGameEvent *event) {
        this->player = event->GetPlayerController("player");
        this->itemdef = event->GetInt("itemdef");
        this->numgifts = event->GetInt("numgifts");
        this->giftidx = event->GetInt("giftidx");
        this->accountid = event->GetInt("accountid");

        CBasePlayerController *controller = (CBasePlayerController *) this->player;
        this->_player = new Player(controller);
    };
    const char *event_name = "items_gifted";
    CEntityInstance *player;//playercontroller
    int itemdef;
    int numgifts;
    int giftidx;
    int accountid;

    Player* _player;

    INT_GETTER(ItemDef, items_gifted, itemdef)
    INT_GETTER(NumGifts, items_gifted, numgifts)
    INT_GETTER(GiftIDx, items_gifted, giftidx)
    INT_GETTER(AccountID, items_gifted, accountid)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetItemDef, items_gifted::GetItemDef)
        SET_GETTER(t, GetNumGifts, items_gifted::GetNumGifts)
        SET_GETTER(t, GetGiftIDx, items_gifted::GetGiftIDx)
        SET_GETTER(t, GetAccountID, items_gifted::GetAccountID)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->_player));

        return obj;
    };
};

class player_score : public RMEvent {
public:
    player_score(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->kills = event->GetInt("kills");
        this->deaths = event->GetInt("deaths");
        this->score = event->GetInt("score");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "player_score";
    CEntityInstance *userid; //playercontroller
    int kills;
    int deaths;
    int score;

    Player* player;

    PREVENT_SUPPORT;

    INT_GETTER(Kills, player_score, kills)
    INT_GETTER(Deaths, player_score, deaths)
    INT_GETTER(Score, player_score, score)

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetKills, player_score::GetKills)
        SET_GETTER(t, GetDeaths, player_score::GetDeaths)
        SET_GETTER(t, GetScore, player_score::GetScore)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class player_shoot : public RMEvent {
public:
    player_shoot(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->weapon = event->GetInt("weapon");
        this->mode = event->GetInt("mode");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "player_shoot";
    CEntityInstance *userid; //playercontroller
    int weapon;
    int mode;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    PREVENT_SUPPORT;

    INT_GETTER(Weapon, player_shoot, weapon)
    INT_GETTER(Mode, player_shoot, mode)

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetWeapon, player_shoot::GetWeapon)
        SET_GETTER(t, GetMode, player_shoot::GetMode)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class game_init : public RMEvent {
public:
    game_init(IGameEvent *event) {
    };
    const char *event_name = "game_init";

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);
        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class game_start : public RMEvent {
public:
    game_start(IGameEvent *event) {
        this->roundslimit = event->GetInt("roundslimit");
        this->timelimit = event->GetInt("timelimit");
        this->fraglimit = event->GetInt("fraglimit");
        this->objective = event->GetString("objective");
    };
    const char *event_name = "game_start";
    int roundslimit;
    int timelimit;
    int fraglimit;
    const char *objective;

    PREVENT_SUPPORT;

    INT_GETTER(RoundsLimit, game_start, roundslimit)
    INT_GETTER(TimeLimit, game_start, timelimit)
    INT_GETTER(FragLimit, game_start, fraglimit)
    STRING_GETTER(Objective, game_start, objective)

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetRoundsLimit, game_start::GetRoundsLimit)
        SET_GETTER(t, GetTimeLimit, game_start::GetTimeLimit)
        SET_GETTER(t, GetFragLimit, game_start::GetFragLimit)
        SET_GETTER(t, GetObjective, game_start::GetObjective)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class game_end : public RMEvent {
public:
    game_end(IGameEvent *event) {
        this->winner = event->GetInt("winner");
    };
    const char *event_name = "game_end";
    int winner;

    PREVENT_SUPPORT;

    INT_GETTER(Winner, game_end, winner)

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetWinner, game_end::GetWinner)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class round_announce_match_point : public RMEvent {
public:
    round_announce_match_point(IGameEvent *event) {
    };
    const char *event_name = "round_announce_match_point";

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);
        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class round_announce_final : public RMEvent {
public:
    round_announce_final(IGameEvent *event) {
    };
    const char *event_name = "round_announce_final";

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class round_announce_last_round_half : public RMEvent {
public:
    round_announce_last_round_half(IGameEvent *event) {
    };
    const char *event_name = "round_announce_last_round_half";

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);
        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class round_announce_match_start : public RMEvent {
public:
    round_announce_match_start(IGameEvent *event) {
    };
    const char *event_name = "round_announce_match_start";

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);
        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class round_announce_warmup : public RMEvent {
public:
    round_announce_warmup(IGameEvent *event) {
    };
    const char *event_name = "round_announce_warmup";

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);
        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class round_end_upload_stats : public RMEvent {
public:
    round_end_upload_stats(IGameEvent *event) {
    };
    const char *event_name = "round_end_upload_stats";

    PREVENT_SUPPORT;
    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);
        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class round_officially_ended : public RMEvent {
public:
    round_officially_ended(IGameEvent *event) {
    };
    const char *event_name = "round_officially_ended";

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);
        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class round_time_warning : public RMEvent {
public:
    round_time_warning(IGameEvent *event) {
    };
    const char *event_name = "round_time_warning";

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);
        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class begin_new_match : public RMEvent {
public:
    begin_new_match(IGameEvent *event) {
    };
    const char *event_name = "begin_new_match";

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);
        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class other_death : public RMEvent {
public:
    other_death(IGameEvent *event) {
        this->otherid = event->GetInt("otherid");
        this->othertype = event->GetString("othertype");
        this->attacker = event->GetInt("attacker");
        this->weapon = event->GetString("weapon");
        this->weapon_itemid = event->GetString("weapon_itemid");
        this->weapon_fauxitemid = event->GetString("weapon_fauxitemid");
        this->weapon_originalowner_xuid = event->GetString("weapon_originalowner_xuid");
        this->headshot = event->GetBool("headshot");
        this->penetrated = event->GetInt("penetrated");
        this->noscope = event->GetBool("noscope");
        this->thrusmoke = event->GetBool("thrusmoke");
        this->attackerblind = event->GetBool("attackerblind");
    };
    const char *event_name = "other_death";
    int otherid;
    const char *othertype;
    int attacker;
    const char *weapon;
    const char *weapon_itemid;
    const char *weapon_fauxitemid;
    const char *weapon_originalowner_xuid;
    bool headshot;
    int penetrated;
    bool noscope;
    bool thrusmoke;
    bool attackerblind;

    PREVENT_SUPPORT;

    INT_GETTER(OtherID, other_death, otherid)
    STRING_GETTER(OtherType, other_death, othertype)
    INT_GETTER(Attacker, other_death, attacker)
    STRING_GETTER(Weapon, other_death, weapon)
    STRING_GETTER(WeaponItemID, other_death, weapon_itemid)
    STRING_GETTER(WeaponFauxItemID, other_death, weapon_fauxitemid)
    STRING_GETTER(WeaponOriginalOwnerXuid, other_death, weapon_originalowner_xuid)
    BOOL_GETTER(IsHeadshot, other_death, headshot)
    INT_GETTER(IsPenetrated, other_death, penetrated)
    BOOL_GETTER(IsNoScope, other_death, noscope)
    BOOL_GETTER(IsThroughSmoke, other_death, thrusmoke)
    BOOL_GETTER(IsAttackerWasBlind, other_death, attackerblind)

    // todo: attacker is player?
    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetOtherID, other_death::GetOtherID)
        SET_GETTER(t, GetOtherType, other_death::GetOtherType)
        SET_GETTER(t, GetAttacker, other_death::GetAttacker)
        SET_GETTER(t, GetWeapon, other_death::GetWeapon)
        SET_GETTER(t, GetWeaponItemID, other_death::GetWeaponItemID)
        SET_GETTER(t, GetWeaponFauxItemID, other_death::GetWeaponFauxItemID)
        SET_GETTER(t, GetWeaponOriginalOwnerXuid, other_death::GetWeaponOriginalOwnerXuid)
        SET_GETTER(t, IsHeadshot, other_death::GetIsHeadshot)
        SET_GETTER(t, IsPenetrated, other_death::GetIsPenetrated)
        SET_GETTER(t, IsNoScope, other_death::GetIsNoScope)
        SET_GETTER(t, IsThroughSmoke, other_death::GetIsThroughSmoke)
        SET_GETTER(t, IsAttackerWasBlind, other_death::GetIsAttackerWasBlind)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class item_purchase : public RMEvent {
public:
    item_purchase(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->team = event->GetInt("team");
        this->loadout = event->GetInt("loadout");
        this->weapon = event->GetString("weapon");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "item_purchase";
    CEntityInstance *userid; //playercontroller
    int team;
    int loadout;
    const char *weapon;

    Player* player;

    INT_GETTER(Team, item_purchase, team)
    INT_GETTER(Loadout, item_purchase, loadout)
    STRING_GETTER(Weapon, item_purchase, weapon)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetTeam, item_purchase::GetTeam)
        SET_GETTER(t, GetLoadout, item_purchase::GetLoadout)
        SET_GETTER(t, GetWeapon, item_purchase::GetWeapon)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class bomb_beginplant : public RMEvent {
public:
    bomb_beginplant(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->site = event->GetInt("site");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "bomb_beginplant";
    CEntityInstance *userid;//playercontroller
    int site;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    INT_GETTER(Site, bomb_beginplant, site)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetSite, bomb_beginplant::GetSite)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class bomb_abortplant : public RMEvent {
public:
    bomb_abortplant(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->site = event->GetInt("site");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "bomb_abortplant";
    CEntityInstance *userid;//playercontroller
    int site;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    INT_GETTER(Site, bomb_abortplant, site)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetSite, bomb_abortplant::GetSite)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class bomb_planted : public RMEvent {
public:
    bomb_planted(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->site = event->GetInt("site");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "bomb_planted";
    CEntityInstance *userid;//playercontroller
    int site;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    INT_GETTER(Site, bomb_planted, site)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetSite, bomb_planted::GetSite)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class bomb_defused : public RMEvent {
public:
    bomb_defused(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->site = event->GetInt("site");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "bomb_defused";
    CEntityInstance *userid;//playercontroller
    int site;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    INT_GETTER(Site, bomb_defused, site)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetSite, bomb_defused::GetSite)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class bomb_exploded : public RMEvent {
public:
    bomb_exploded(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->site = event->GetInt("site");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "bomb_exploded";
    CEntityInstance *userid;//playercontroller
    int site;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    INT_GETTER(Site, bomb_exploded, site)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetSite, bomb_exploded::GetSite)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class bomb_dropped : public RMEvent {
public:
    bomb_dropped(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entindex = event->GetInt("entindex");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "bomb_dropped";
    CEntityInstance *userid;//playercontroller
    int entindex;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    INT_GETTER(EntityIndex, bomb_dropped, entindex)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetEntityIndex, bomb_dropped::GetEntityIndex)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class bomb_pickup : public RMEvent {
public:
    bomb_pickup(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "bomb_pickup";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class defuser_dropped : public RMEvent {
public:
    defuser_dropped(IGameEvent *event) {
        this->entityid = event->GetInt("entityid");
    };
    const char *event_name = "defuser_dropped";
    int entityid;

    INT_GETTER(EntityID, defuser_dropped, entityid)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetEntityID, defuser_dropped::GetEntityID)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class defuser_pickup : public RMEvent {
public:
    defuser_pickup(IGameEvent *event) {
        this->entityid = event->GetInt("entityid");
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "defuser_pickup";
    int entityid;
    CEntityInstance *userid;//playercontroller
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    INT_GETTER(EntityID, defuser_pickup, entityid)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetEntityID, defuser_pickup::GetEntityID)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class bomb_begindefuse : public RMEvent {
public:
    bomb_begindefuse(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->haskit = event->GetBool("haskit");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "bomb_begindefuse";
    CEntityInstance *userid; //playercontroller
    bool haskit;
    CEntityInstance *userid_pawn; //strict_ehandle

    Player* player;

    BOOL_GETTER(HasKit, bomb_begindefuse, haskit)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, WithKit, bomb_begindefuse::GetHasKit)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class bomb_abortdefuse : public RMEvent {
public:
    bomb_abortdefuse(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "bomb_abortdefuse";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class hostage_follows : public RMEvent {
public:
    hostage_follows(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hostage = event->GetInt("hostage");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "hostage_follows";
    CEntityInstance *userid; //playercontroller
    int hostage;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    INT_GETTER(Hostage, hostage_follows, hostage)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetHostage, hostage_follows::GetHostage)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class hostage_hurt : public RMEvent {
public:
    hostage_hurt(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hostage = event->GetInt("hostage");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "hostage_hurt";
    CEntityInstance *userid;//playercontroller
    int hostage;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    INT_GETTER(Hostage, hostage_hurt, hostage)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetHostage, hostage_hurt::GetHostage)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class hostage_killed : public RMEvent {
public:
    hostage_killed(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hostage = event->GetInt("hostage");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "hostage_killed";
    CEntityInstance *userid;//playercontroller
    int hostage;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    INT_GETTER(Hostage, hostage_killed, hostage)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetHostage, hostage_killed::GetHostage)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class hostage_rescued : public RMEvent {
public:
    hostage_rescued(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hostage = event->GetInt("hostage");
        this->site = event->GetInt("site");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "hostage_rescued";
    CEntityInstance *userid;//playercontroller
    int hostage;
    int site;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    INT_GETTER(Hostage, hostage_rescued, hostage)
    INT_GETTER(Site, hostage_rescued, site)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetHostage, hostage_rescued::GetHostage)
        SET_GETTER(t, GetSite, hostage_rescued::GetSite)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class hostage_stops_following : public RMEvent {
public:
    hostage_stops_following(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hostage = event->GetInt("hostage");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "hostage_stops_following";
    CEntityInstance *userid;//playercontroller
    int hostage;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    INT_GETTER(Hostage, hostage_stops_following, hostage)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetHostage, hostage_stops_following::GetHostage)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class hostage_rescued_all : public RMEvent {
public:
    hostage_rescued_all(IGameEvent *event) {
    };
    const char *event_name = "hostage_rescued_all";

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);
        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class hostage_call_for_help : public RMEvent {
public:
    hostage_call_for_help(IGameEvent *event) {
        this->hostage = event->GetInt("hostage");
    };
    const char *event_name = "hostage_call_for_help";
    int hostage;

    INT_GETTER(Hostage, hostage_call_for_help, hostage)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetHostage, hostage_call_for_help::GetHostage)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        return obj;
    };
};

class player_radio : public RMEvent {
public:
    player_radio(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->slot = event->GetInt("slot");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "player_radio";

    CEntityInstance *userid; //playercontroller
    int slot;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    INT_GETTER(Slot, player_radio, slot)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetSlot, player_radio::GetSlot)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class bomb_beep : public RMEvent {
public:
    bomb_beep(IGameEvent *event) {
        this->entindex = event->GetInt("entindex");
    };
    const char *event_name = "bomb_beep";
    int entindex;

    INT_GETTER(EntityIndex, bomb_beep, entindex)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetEntityIndex, bomb_beep::GetEntityIndex)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        return obj;
    };
};

class weapon_fire : public RMEvent {
public:
    weapon_fire(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->weapon = event->GetString("weapon");
        this->silenced = event->GetBool("silenced");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "weapon_fire";
    CEntityInstance *userid;//playercontroller
    const char *weapon;
    bool silenced;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    STRING_GETTER(Weapon, weapon_fire, weapon)
    BOOL_GETTER(IsSilenced, weapon_fire, silenced)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetWeapon, weapon_fire::GetWeapon)
        SET_GETTER(t, IsSilenced, weapon_fire::GetIsSilenced)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class weapon_fire_on_empty : public RMEvent {
public:
    weapon_fire_on_empty(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->weapon = event->GetString("weapon");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "weapon_fire_on_empty";
    CEntityInstance *userid; //playercontroller
    const char *weapon;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    STRING_GETTER(Weapon, weapon_fire_on_empty, weapon)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetWeapon, weapon_fire_on_empty::GetWeapon)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class grenade_thrown : public RMEvent {
public:
    grenade_thrown(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->weapon = event->GetString("weapon");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "grenade_thrown";
    CEntityInstance *userid;//playercontroller
    const char *weapon;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    STRING_GETTER(Weapon, grenade_thrown, weapon)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetWeapon, grenade_thrown::GetWeapon)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class weapon_outofammo : public RMEvent {
public:
    weapon_outofammo(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "weapon_outofammo";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *userid_pawn;//strict_ehandle;

    Player* player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class weapon_reload : public RMEvent {
public:
    weapon_reload(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "weapon_reload";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *userid_pawn;//strict_ehandle;

    Player* player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class weapon_zoom : public RMEvent {
public:
    weapon_zoom(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "weapon_zoom";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *userid_pawn;//strict_ehandle;

    Player* player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class silencer_detach : public RMEvent {
public:
    silencer_detach(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "silencer_detach";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *userid_pawn;//strict_ehandle;

    Player* player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class inspect_weapon : public RMEvent {
public:
    inspect_weapon(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "inspect_weapon";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *userid_pawn;//strict_ehandle;

    Player* player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class player_spawned : public RMEvent {
public:
    player_spawned(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->inrestart = event->GetBool("inrestart");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "player_spawned";
    CEntityInstance *userid; //playercontroller
    bool inrestart;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    INT_GETTER(InRestart, player_spawned, inrestart)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, InRestart, player_spawned::GetInRestart)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class item_pickup : public RMEvent {
public:
    item_pickup(IGameEvent *event) {
        this->event_name = event->GetName();
        this->userid = event->GetPlayerController("userid");
        this->item = event->GetString("item");
        this->silent = event->GetBool("silent");
        this->defindex = event->GetInt("defindex");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };

    const char *event_name = "item_pickup";
    CEntityInstance *userid;//playercontroller
    Player *player;
    const char *item;
    bool silent;
    int defindex;

    STRING_GETTER(Item, item_pickup, item)
    BOOL_GETTER(IsSilent, item_pickup, silent)
    INT_GETTER(DefIndex, item_pickup, defindex)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetItem, item_pickup::GetItem)
        SET_GETTER(t, IsSilent, item_pickup::GetIsSilent)
        SET_GETTER(t, GetDefIndex, item_pickup::GetDefIndex)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class item_pickup_slerp : public RMEvent {
public:
    item_pickup_slerp(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->index = event->GetInt("index");
        this->behavior = event->GetInt("behavior");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "item_pickup_slerp";
    CEntityInstance *userid;//playercontroller
    int index;
    int behavior;

    Player *player;

    INT_GETTER(Index, item_pickup_slerp, index)
    INT_GETTER(Behavior, item_pickup_slerp, behavior)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetIndex, item_pickup_slerp::GetIndex)
        SET_GETTER(t, GetBehavior, item_pickup_slerp::GetBehavior)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class item_pickup_failed : public RMEvent {
public:
    item_pickup_failed(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->item = event->GetString("item");
        this->reason = event->GetInt("reason");
        this->limit = event->GetInt("limit");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "item_pickup_failed";
    CEntityInstance *userid;//playercontroller
    const char *item;
    int reason;
    int limit;

    Player *player;

    STRING_GETTER(Item, item_pickup_failed, item)
    INT_GETTER(Reason, item_pickup_failed, reason)
    INT_GETTER(Limit, item_pickup_failed, limit)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetItem, item_pickup_failed::GetItem)
        SET_GETTER(t, GetReason, item_pickup_failed::GetReason)
        SET_GETTER(t, GetLimit, item_pickup_failed::GetLimit)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class item_remove : public RMEvent {
public:
    item_remove(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->item = event->GetString("item");
        this->defindex = event->GetInt("defindex");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "item_remove";
    CEntityInstance *userid;//playercontroller
    const char *item;
    int defindex;

    Player *player;

    STRING_GETTER(Item, item_remove, item)
    INT_GETTER(DefIndex, item_remove, defindex)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetDefIndex, item_remove::GetDefIndex)
        SET_GETTER(t, GetItem, item_remove::GetItem)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class ammo_pickup : public RMEvent {
public:
    ammo_pickup(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->item = event->GetString("item");
        this->index = event->GetInt("index");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "ammo_pickup";
    CEntityInstance *userid; //playercontroller
    const char *item;
    int index;

    Player *player;

    STRING_GETTER(Item, ammo_pickup, item)
    INT_GETTER(Index, ammo_pickup, index)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetIndex, ammo_pickup::GetIndex)
        SET_GETTER(t, GetItem, ammo_pickup::GetItem)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class item_equip : public RMEvent {
public:
    item_equip(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->item = event->GetString("item");
        this->defindex = event->GetInt("defindex");
        this->canzoom = event->GetBool("canzoom");
        this->hassilencer = event->GetBool("hassilencer");
        this->issilenced = event->GetBool("issilenced");
        this->hastracers = event->GetBool("hastracers");
        this->weptype = event->GetInt("weptype");
        this->ispainted = event->GetBool("ispainted");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "item_equip";
    CEntityInstance *userid;//playercontroller
    const char *item;
    int defindex;
    bool canzoom;
    bool hassilencer;
    bool issilenced;
    bool hastracers;
    int weptype;
    bool ispainted;

    Player *player;

    STRING_GETTER(Item, item_equip, item)
    INT_GETTER(DefIndex, item_equip, defindex)
    BOOL_GETTER(CanZoom, item_equip, canzoom)
    BOOL_GETTER(HasSilincer, item_equip, hassilencer)
    BOOL_GETTER(IsSilinced, item_equip, issilenced)
    BOOL_GETTER(HasTracers, item_equip, hastracers)
    INT_GETTER(WepType, item_equip, weptype)
    BOOL_GETTER(IsPainted, item_equip, ispainted)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetItem, item_equip::GetItem)
        SET_GETTER(t, CanZoom, item_equip::GetCanZoom)
        SET_GETTER(t, HasSilincer, item_equip::GetHasSilincer)
        SET_GETTER(t, IsSilinced, item_equip::GetIsSilinced)
        SET_GETTER(t, HasTracers, item_equip::GetHasTracers)
        SET_GETTER(t, GetWepType, item_equip::GetWepType)
        SET_GETTER(t, IsPainted, item_equip::GetIsPainted)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};


class enter_buyzone : public RMEvent {
public:
    enter_buyzone(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->canbuy = event->GetBool("canbuy");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "enter_buyzone";
    CEntityInstance *userid;//playercontroller
    bool canbuy;

    Player *player;

    BOOL_GETTER(CanBuy, enter_buyzone, canbuy)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, CanBuy, enter_buyzone::GetCanBuy)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class exit_buyzone : public RMEvent {
public:
    exit_buyzone(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->canbuy = event->GetBool("canbuy");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "exit_buyzone";
    CEntityInstance *userid; //playercontroller
    bool canbuy;

    Player *player;

    BOOL_GETTER(CanBuy, exit_buyzone, canbuy)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, CanBuy, exit_buyzone::GetCanBuy)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};


class buytime_ended : public RMEvent {
public:
    buytime_ended(IGameEvent *event) {

    };
    const char *event_name = "buytime_ended";

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};


class enter_bombzone : public RMEvent {
public:
    enter_bombzone(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hasbomb = event->GetBool("hasbomb");
        this->isplanted = event->GetBool("isplanted");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "enter_bombzone";
    CEntityInstance *userid;//playercontroller
    bool hasbomb;
    bool isplanted;

    Player *player;

    BOOL_GETTER(HasBomb, enter_bombzone, hasbomb)
    BOOL_GETTER(IsPlanted, enter_bombzone, isplanted)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, HasBomb, enter_bombzone::GetHasBomb)
        SET_GETTER(t, BombIsPlanted, enter_bombzone::GetIsPlanted)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class exit_bombzone : public RMEvent {
public:
    exit_bombzone(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hasbomb = event->GetBool("hasbomb");
        this->isplanted = event->GetBool("isplanted");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "exit_bombzone";
    CEntityInstance *userid;//playercontroller
    bool hasbomb;
    bool isplanted;

    Player *player;

    BOOL_GETTER(HasBomb, exit_bombzone, hasbomb)
    BOOL_GETTER(IsPlanted, exit_bombzone, isplanted)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, HasBomb, exit_bombzone::GetHasBomb)
        SET_GETTER(t, BombIsPlanted, exit_bombzone::GetIsPlanted)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class enter_rescue_zone : public RMEvent {
public:
    enter_rescue_zone(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "enter_rescue_zone";
    CEntityInstance *userid;//playercontroller

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class exit_rescue_zone : public RMEvent {
public:
    exit_rescue_zone(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "exit_rescue_zone";
    CEntityInstance *userid;//playercontroller

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class silencer_off : public RMEvent {
public:
    silencer_off(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "silencer_off";
    CEntityInstance *userid;//playercontroller

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class silencer_on : public RMEvent {
public:
    silencer_on(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "silencer_on";
    CEntityInstance *userid;//playercontroller

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class buymenu_open : public RMEvent {
public:
    buymenu_open(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "buymenu_open";
    CEntityInstance *userid;//playercontroller

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class buymenu_close : public RMEvent {
public:
    buymenu_close(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "buymenu_close";
    CEntityInstance *userid;//playercontroller

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class round_prestart : public RMEvent {
public:
    round_prestart(IGameEvent *event) {

    };

    const char *event_name = "round_prestart";

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class round_poststart : public RMEvent {
public:
    round_poststart(IGameEvent *event) {

    };

    const char *event_name = "round_poststart";

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class grenade_bounce : public RMEvent {
public:
    grenade_bounce(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "grenade_bounce";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *userid_pawn;//strict_ehandle

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class hegrenade_detonate : public RMEvent {
public:
    hegrenade_detonate(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entityid = event->GetInt("entityid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "hegrenade_detonate";
    CEntityInstance *userid;//playercontroller
    int entityid;
    float x;
    float y;
    float z;
    CEntityInstance *userid_pawn;//strict_ehandle

    INT_GETTER(EntityID, hegrenade_detonate, entityid)
    NUMBER_GETTER(X, hegrenade_detonate, x)
    NUMBER_GETTER(Y, hegrenade_detonate, y)
    NUMBER_GETTER(Z, hegrenade_detonate, z)

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetEntityID, hegrenade_detonate::GetEntityID)
        SET_GETTER(t, GetX, hegrenade_detonate::GetX)
        SET_GETTER(t, GetY, hegrenade_detonate::GetY)
        SET_GETTER(t, GetZ, hegrenade_detonate::GetZ)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class flashbang_detonate : public RMEvent {
public:
    flashbang_detonate(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entityid = event->GetInt("entityid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    }

    const char *event_name = "flashbang_detonate";
    CEntityInstance *userid;//playercontroller
    int entityid;
    float x;
    float y;
    float z;
    CEntityInstance *userid_pawn;//strict_ehandle

    INT_GETTER(EntityID, flashbang_detonate, entityid)
    NUMBER_GETTER(X, flashbang_detonate, x)
    NUMBER_GETTER(Y, flashbang_detonate, y)
    NUMBER_GETTER(Z, flashbang_detonate, z)

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetEntityID, flashbang_detonate::GetEntityID)
        SET_GETTER(t, GetX, flashbang_detonate::GetX)
        SET_GETTER(t, GetY, flashbang_detonate::GetY)
        SET_GETTER(t, GetZ, flashbang_detonate::GetZ)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class smokegrenade_detonate : public RMEvent {
public:
    smokegrenade_detonate(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entityid = event->GetInt("entityid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "smokegrenade_detonate";
    CEntityInstance *userid;//playercontroller
    int entityid;
    float x;
    float y;
    float z;
    CEntityInstance *userid_pawn;//strict_ehandle

    INT_GETTER(EntityID, smokegrenade_detonate, entityid)
    NUMBER_GETTER(X, smokegrenade_detonate, x)
    NUMBER_GETTER(Y, smokegrenade_detonate, y)
    NUMBER_GETTER(Z, smokegrenade_detonate, z)

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetEntityID, smokegrenade_detonate::GetEntityID)
        SET_GETTER(t, GetX, smokegrenade_detonate::GetX)
        SET_GETTER(t, GetY, smokegrenade_detonate::GetY)
        SET_GETTER(t, GetZ, smokegrenade_detonate::GetZ)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class smokegrenade_expired : public RMEvent {
public:
    smokegrenade_expired(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entityid = event->GetInt("entityid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "smokegrenade_expired";
    CEntityInstance *userid;//playercontroller
    int entityid;
    float x;
    float y;
    float z;
    CEntityInstance *userid_pawn;//strict_ehandle

    INT_GETTER(EntityID, smokegrenade_expired, entityid)
    NUMBER_GETTER(X, smokegrenade_expired, x)
    NUMBER_GETTER(Y, smokegrenade_expired, y)
    NUMBER_GETTER(Z, smokegrenade_expired, z)

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetEntityID, smokegrenade_expired::GetEntityID)
        SET_GETTER(t, GetX, smokegrenade_expired::GetX)
        SET_GETTER(t, GetY, smokegrenade_expired::GetY)
        SET_GETTER(t, GetZ, smokegrenade_expired::GetZ)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class molotov_detonate : public RMEvent {
public:
    molotov_detonate(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "molotov_detonate";
    CEntityInstance *userid;//playercontroller
    float x;
    float y;
    float z;
    CEntityInstance *userid_pawn;//strict_ehandle

    NUMBER_GETTER(X, molotov_detonate, x)
    NUMBER_GETTER(Y, molotov_detonate, y)
    NUMBER_GETTER(Z, molotov_detonate, z)

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetX, molotov_detonate::GetX)
        SET_GETTER(t, GetY, molotov_detonate::GetY)
        SET_GETTER(t, GetZ, molotov_detonate::GetZ)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class decoy_detonate : public RMEvent {
public:
    decoy_detonate(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entityid = event->GetInt("entityid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "decoy_detonate";
    CEntityInstance *userid;//playercontroller
    short entityid;
    float x;
    float y;
    float z;
    CEntityInstance *userid_pawn;//strict_ehandle

    INT_GETTER(EntityID, decoy_detonate, entityid)
    NUMBER_GETTER(X, decoy_detonate, x)
    NUMBER_GETTER(Y, decoy_detonate, y)
    NUMBER_GETTER(Z, decoy_detonate, z)

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetEntityID, decoy_detonate::GetEntityID)
        SET_GETTER(t, GetX, decoy_detonate::GetX)
        SET_GETTER(t, GetY, decoy_detonate::GetY)
        SET_GETTER(t, GetZ, decoy_detonate::GetZ)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class decoy_started : public RMEvent {
public:
    decoy_started(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entityid = event->GetInt("entityid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "decoy_started";
    CEntityInstance *userid;//player_pawn
    short entityid;
    float x;
    float y;
    float z;
    CEntityInstance *userid_pawn;//strict_ehandle

    INT_GETTER(EntityID, decoy_started, entityid)
    NUMBER_GETTER(X, decoy_started, x)
    NUMBER_GETTER(Y, decoy_started, y)
    NUMBER_GETTER(Z, decoy_started, z)

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetEntityID, decoy_started::GetEntityID)
        SET_GETTER(t, GetX, decoy_started::GetX)
        SET_GETTER(t, GetY, decoy_started::GetY)
        SET_GETTER(t, GetZ, decoy_started::GetZ)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class tagrenade_detonate : public RMEvent {
public:
    tagrenade_detonate(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entityid = event->GetInt("entityid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "tagrenade_detonate";
    CEntityInstance *userid;//playercontroller
    int entityid;
    float x;
    float y;
    float z;

    INT_GETTER(EntityID, tagrenade_detonate, entityid)
    NUMBER_GETTER(X, tagrenade_detonate, x)
    NUMBER_GETTER(Y, tagrenade_detonate, y)
    NUMBER_GETTER(Z, tagrenade_detonate, z)

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetEntityID, tagrenade_detonate::GetEntityID)
        SET_GETTER(t, GetX, tagrenade_detonate::GetX)
        SET_GETTER(t, GetY, tagrenade_detonate::GetY)
        SET_GETTER(t, GetZ, tagrenade_detonate::GetZ)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class decoy_firing : public RMEvent {
public:
    decoy_firing(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entityid = event->GetInt("entityid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "decoy_firing";
    CEntityInstance *userid;//playercontroller
    int entityid;
    float x;
    float y;
    float z;
    CEntityInstance *userid_pawn;//strict_ehandle

    INT_GETTER(EntityID, decoy_firing, entityid)
    NUMBER_GETTER(X, decoy_firing, x)
    NUMBER_GETTER(Y, decoy_firing, y)
    NUMBER_GETTER(Z, decoy_firing, z)

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetEntityID, decoy_firing::GetEntityID)
        SET_GETTER(t, GetX, decoy_firing::GetX)
        SET_GETTER(t, GetY, decoy_firing::GetY)
        SET_GETTER(t, GetZ, decoy_firing::GetZ)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class bullet_impact : public RMEvent {
public:
    bullet_impact(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->x = event->GetFloat("x");
        this->y = event->GetFloat("y");
        this->z = event->GetFloat("z");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "bullet_impact";
    CEntityInstance *userid;//playercontroller
    float x;
    float y;
    float z;
    CEntityInstance *userid_pawn;//strict_ehandle

    NUMBER_GETTER(X, bullet_impact, x)
    NUMBER_GETTER(Y, bullet_impact, y)
    NUMBER_GETTER(Z, bullet_impact, z)

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetX, bullet_impact::GetX)
        SET_GETTER(t, GetY, bullet_impact::GetY)
        SET_GETTER(t, GetZ, bullet_impact::GetZ)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class player_jump : public RMEvent {
public:
    player_jump(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "player_jump";
    CEntityInstance *userid;//playercontroller

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class player_blind : public RMEvent {
public:
    player_blind(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->attacker = event->GetPlayerController("attacker");
        this->entityid = event->GetInt("entityid");
        this->blind_duration = event->GetFloat("blind_duration");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);

        CBasePlayerController *at_controller = (CBasePlayerController *) this->attacker;
        this->_attacker = new Player(at_controller);
    };
    const char *event_name = "player_blind";
    CEntityInstance *userid; //playercontroller
    CEntityInstance *attacker; //playercontroller
    int entityid;
    float blind_duration;

    INT_GETTER(EntityID, player_blind, entityid)
    NUMBER_GETTER(Duration, player_blind, blind_duration)

    Player *player;
    Player *_attacker;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(3);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetPlayer, Player::GetAttackerObject)

        SET_GETTER(t, GetEntityID, player_blind::GetEntityID)
        SET_GETTER(t, GetDuration, player_blind::GetDuration)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));
        obj->SetInternalField(2, v8::External::New(g_Engine->isolate, this->_attacker));

        return obj;
    };
};

class player_falldamage : public RMEvent {
public:
    player_falldamage(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->damage = event->GetFloat("damage");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "player_falldamage";
    CEntityInstance *userid;//playercontroller
    float damage;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player *player;

    NUMBER_GETTER(Damage, player_falldamage, damage)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetDamage, player_falldamage::GetDamage)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class door_moving : public RMEvent {
public:
    door_moving(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entindex = event->GetInt("entindex");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "door_moving";
    CEntityInstance *userid;//playercontroller
    int entindex;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player *player;

    INT_GETTER(EntIndex, door_moving, entindex)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetEntIndex, door_moving::GetEntIndex)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class switch_team : public RMEvent {
public:
    switch_team(IGameEvent *event) {
        this->numPlayers = event->GetInt("numPlayers");
        this->numSpectators = event->GetInt("numSpectators");
        this->avg_rank = event->GetInt("avg_rank");
        this->numTSlotsFree = event->GetInt("numTSlotsFree");
        this->numCTSlotsFree = event->GetInt("numCTSlotsFree");
    };
    const char *event_name = "switch_team";
    int numPlayers;
    int numSpectators;
    int avg_rank;
    int numTSlotsFree;
    int numCTSlotsFree;

    INT_GETTER(NumPlayers, switch_team, numPlayers)
    INT_GETTER(NumSpectators, switch_team, numSpectators)
    INT_GETTER(AvgRank, switch_team, avg_rank)
    INT_GETTER(NumTSlotsFree, switch_team, numTSlotsFree)
    INT_GETTER(NumCTSlotsFree, switch_team, numCTSlotsFree)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetNumPlayers, switch_team::GetNumPlayers)
        SET_GETTER(t, GetNumSpectators, switch_team::GetNumSpectators)
        SET_GETTER(t, GetAvgRank, switch_team::GetAvgRank)
        SET_GETTER(t, GetNumTSlotsFree, switch_team::GetNumTSlotsFree)
        SET_GETTER(t, GetNumCTSlotsFree, switch_team::GetNumCTSlotsFree)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class player_given_c4 : public RMEvent {
public:
    player_given_c4(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "player_given_c4";
    CEntityInstance *userid;//playercontroller

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class bot_takeover : public RMEvent {
public:
    bot_takeover(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->botid = event->GetPlayerController("botid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "bot_takeover";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *botid;//playercontroller
    CEntityInstance *userid_pawn;//strict_ehandle

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class jointeam_failed : public RMEvent {
public:
    jointeam_failed(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->reason = event->GetInt("reason");

        CBasePlayerController *controller = (CBasePlayerController *) this->userid;
        this->player = new Player(controller);
    };
    const char *event_name = "jointeam_failed";
    CEntityInstance *userid;//playercontroller
    int reason; // 0 - full team

    INT_GETTER(Reason, jointeam_failed, reason)

    Player *player;

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(2);

        SET_GETTER(t, GetPlayer, Player::GetPlayerObject)
        SET_GETTER(t, GetReason, jointeam_failed::GetReason)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));
        obj->SetInternalField(1, v8::External::New(g_Engine->isolate, this->player));

        return obj;
    };
};

class nextlevel_changed : public RMEvent {
public:
    nextlevel_changed(IGameEvent *event) {
        this->nextlevel = event->GetString("nextlevel");
        this->mapgroup = event->GetString("mapgroup");
        this->skirmishmode = event->GetString("skirmishmode");
    };
    const char *event_name = "nextlevel_changed";
    const char *nextlevel;
    const char *mapgroup;
    const char *skirmishmode;

    STRING_GETTER(NextLevel, nextlevel_changed, nextlevel)
    STRING_GETTER(MapGroup, nextlevel_changed, mapgroup)
    STRING_GETTER(SkirmIsHMode, nextlevel_changed, skirmishmode)

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        SET_GETTER(t, GetNextLevel, nextlevel_changed::GetNextLevel)
        SET_GETTER(t, GetMapGroup, nextlevel_changed::GetMapGroup)
        SET_GETTER(t, GetSkirmIsHMode, nextlevel_changed::GetSkirmIsHMode)

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class team_intro_start : public RMEvent {
public:
    team_intro_start(IGameEvent *event) {};
    const char *event_name = "team_intro_start";

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

class team_intro_end : public RMEvent {
public:
    team_intro_end(IGameEvent *event) {
    };
    const char *event_name = "team_intro_end";

    PREVENT_SUPPORT;

    v8::Local<v8::Object> SerializeV8(v8::Local<v8::Context> ctx) override {
        v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(g_Engine->isolate);
        t->SetInternalFieldCount(1);

        v8::Local<v8::Object> obj = t->NewInstance(ctx).ToLocalChecked();
        obj->SetInternalField(0, v8::External::New(g_Engine->isolate, this));

        return obj;
    };
};

#endif //RESOURCEMOD_LEGACYEVENTS_H
