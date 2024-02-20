//
// Created by Twelvee on 10.10.2023.
//

#ifndef RESOURCEMOD_LEGACYEVENTS_H
#define RESOURCEMOD_LEGACYEVENTS_H

#include "../cs2/cbaseentity.h"
#include "../../logger/logger.h"
#include "../cs2/CCSPlayerController.h"
#include "../../engine/Engine.h"
#include "metacall/metacall.h"
#include "../js/Player.h"

extern Engine *g_Engine;

class RMEvent {
public:
    const char *event_name;

    virtual bool Emit() = 0;

};

class player_activate : public RMEvent {
public:
    player_activate(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "player_activate";
    CEntityInstance *userid; //playercontroller

    Player *player;

    bool Emit() override {
        void *playerArgs[] = {
                metacall_value_create_string(this->player->controller->GetPlayerName(),
                                             strlen(this->player->controller->GetPlayerName())),
                metacall_value_create_long(this->player->controller->m_steamID),
                metacall_value_create_int(this->player->controller->GetPlayerSlot())
        };

        void *playerObj = metacallv("_Player", playerArgs);

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                playerObj
        };

        void *objptr = metacallv("_PlayerActivateEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        metacall_value_destroy(playerArgs[0]);
        metacall_value_destroy(playerArgs[1]);
        metacall_value_destroy(playerArgs[2]);
        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class player_spawn : public RMEvent {
public:
    player_spawn(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");
        if (this->userid_pawn == nullptr && this->userid == nullptr) {
            this->shouldEmit = false;
            return;
        }
        this->shouldEmit = true;
        this->player = new Player((CCSPlayerController *) this->userid);
        if (!this->player->controller && this->userid_pawn != nullptr) {
            this->player->controller = CCSPlayerController::FromPawn((CCSPlayerPawn *) this->userid_pawn);
        }
    };
    std::string event_name = "player_spawn";
    CEntityInstance *userid; //playercontroller
    CEntityInstance *userid_pawn; //strict_ehandle
    bool shouldEmit = false;

    Player *player;

    bool Emit() override {
        if (!this->shouldEmit) {
            return false;
        }
        void *playerArgs[] = {
                metacall_value_create_string(this->player->controller->GetPlayerName(),
                                             strlen(this->player->controller->GetPlayerName())),
                metacall_value_create_long(this->player->controller->m_steamID),
                metacall_value_create_int(this->player->controller->GetPlayerSlot())
        };

        void *playerObj = metacallv("_Player", playerArgs);

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                playerObj
        };

        void *objptr = metacallv("_PlayerSpawnEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        metacall_value_destroy(playerArgs[0]);
        metacall_value_destroy(playerArgs[1]);
        metacall_value_destroy(playerArgs[2]);
        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
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

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "player_team";
    CEntityInstance *userid;//playercontroller
    int team;
    int oldteam;
    bool disconnect;
    bool silent;
    bool isbot;
    CEntityInstance *userid_pawn; //strict_ehandle

    Player *player;

    bool Emit() override {
        void *playerArgs[] = {
                metacall_value_create_string(this->player->controller->GetPlayerName(),
                                             strlen(this->player->controller->GetPlayerName())),
                metacall_value_create_long(this->player->controller->m_steamID),
                metacall_value_create_int(this->player->controller->GetPlayerSlot())
        };

        void *playerObj = metacallv("_Player", playerArgs);

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->team),
                metacall_value_create_int(this->oldteam),
                metacall_value_create_bool(this->disconnect),
                metacall_value_create_bool(this->silent),
                playerObj
        };

        void *objptr = metacallv("_PlayerChangeTeamEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        metacall_value_destroy(playerArgs[0]);
        metacall_value_destroy(playerArgs[1]);
        metacall_value_destroy(playerArgs[2]);
        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
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
        this->player = new Player((CCSPlayerController *) this->userid);
        this->attacker_player = new Player((CCSPlayerController *) this->attacker);
    };
    std::string event_name = "player_hurt";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *attacker; //playercontroller
    int health;
    int armor;
    std::string weapon;
    int dmg_health;
    int dmg_armor;
    int hitgroup;
    CEntityInstance *userid_pawn; //strict_ehandle
    CEntityInstance *attacker_pawn; //strict_ehandle

    Player *player;
    Player *attacker_player;

    bool Emit() override {
        void *playerArgs[] = {
                metacall_value_create_string(this->player->controller->GetPlayerName(),
                                             strlen(this->player->controller->GetPlayerName())),
                metacall_value_create_long(this->player->controller->m_steamID),
                metacall_value_create_int(this->player->controller->GetPlayerSlot())
        };

        void *playerObj = metacallv("_Player", playerArgs);
        void *attackerObj;
        if (this->attacker_player->controller != nullptr) {
            void *attackerArgs[] = {
                    metacall_value_create_string(this->attacker_player->controller->GetPlayerName(),
                                                 strlen(this->attacker_player->controller->GetPlayerName())),
                    metacall_value_create_long(this->attacker_player->controller->m_steamID),
                    metacall_value_create_int(this->attacker_player->controller->GetPlayerSlot())
            };

            attackerObj = metacallv("_Player", attackerArgs);

            metacall_value_destroy(attackerArgs[0]);
            metacall_value_destroy(attackerArgs[1]);
            metacall_value_destroy(attackerArgs[2]);
        } else {
            attackerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->health),
                metacall_value_create_int(this->armor),
                metacall_value_create_string(this->weapon.c_str(), this->weapon.length()),
                metacall_value_create_int(this->dmg_health),
                metacall_value_create_int(this->dmg_armor),
                metacall_value_create_int(this->hitgroup),
                playerObj,
                attackerObj
        };

        void *objptr = metacallv("_PlayerHurtEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        metacall_value_destroy(playerArgs[0]);
        metacall_value_destroy(playerArgs[1]);
        metacall_value_destroy(playerArgs[2]);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);
        return V_strcmp(result.c_str(), "event_prevent_default") == 0;
    };
};

// PRE HOOK
class player_chat : public RMEvent {
public:
    player_chat(int slot, bool teamOnly, std::string text) {
        this->teamonly = teamOnly;
        this->userid = slot;
        this->text = text;

        this->player = new Player(CCSPlayerController::FromSlot(this->userid));
    };
    std::string event_name = "player_chat";

    bool teamonly;
    int userid;
    std::string text;
    Player *player;

    bool Emit() override {
        void *playerArgs[] = {
                metacall_value_create_string(this->player->controller->GetPlayerName(),
                                             strlen(this->player->controller->GetPlayerName())),
                metacall_value_create_long(this->player->controller->m_steamID),
                metacall_value_create_int(this->player->controller->GetPlayerSlot())
        };

        void *playerObj = metacallv("_Player", playerArgs);

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_string(this->text.c_str(), this->text.length()),
                metacall_value_create_bool(this->teamonly),
                playerObj
        };

        void *objptr = metacallv("_PlayerChatEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        metacall_value_destroy(playerArgs[0]);
        metacall_value_destroy(playerArgs[1]);
        metacall_value_destroy(playerArgs[2]);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class game_message : public RMEvent {
public:
    game_message(IGameEvent *event) {
        this->target = event->GetInt("target");
        this->text = event->GetString("text");
        if (this->target == 0) {
            this->targetStr = "console";
        } else {
            this->targetStr = "hud";
        }
    };
    std::string event_name = "game_message";
    int target; //0- console, 1 - hud
    std::string targetStr;
    std::string text;

    bool Emit() override {
        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_string(this->text.c_str(), this->text.length()),
                metacall_value_create_string(this->targetStr.c_str(), this->targetStr.length()),
        };

        void *objptr = metacallv("_GameMessageEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class map_loaded {
public:
    map_loaded(std::string mapName, std::string oldMapName) {
        this->mapName = mapName;
        this->oldMapName = oldMapName;
    };
    std::string event_name = "map_loaded";
    std::string mapName;
    std::string oldMapName;

    bool Emit() {
        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_string(this->mapName.c_str(), this->mapName.length()),
                metacall_value_create_string(this->oldMapName.c_str(), this->oldMapName.length()),
        };

        void *objptr = metacallv("_MapLoadedEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
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
    std::string event_name = "round_end";
    int winner;
    int reason;
    std::string message;
    int legacy;
    int player_count;
    int nomusic;

    bool Emit() override {
        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->winner),
                metacall_value_create_int(this->reason),
                metacall_value_create_string(this->message.c_str(), this->message.length()),
                metacall_value_create_int(this->legacy),
                metacall_value_create_int(this->player_count),
                metacall_value_create_int(this->nomusic),
        };

        void *objptr = metacallv("_RoundEndEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class round_start : public RMEvent {
public:
    round_start(IGameEvent *event) {
        this->timelimit = event->GetInt("timelimit");
        this->fraglimit = event->GetInt("fraglimit");
        this->objective = event->GetString("objective");
    };
    std::string event_name = "round_start";
    int timelimit;
    int fraglimit;
    std::string objective;

    bool Emit() override {
        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->timelimit),
                metacall_value_create_int(this->fraglimit),
                metacall_value_create_string(this->objective.c_str(), this->objective.length()),
        };

        void *objptr = metacallv("_RoundStartEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class round_freeze_end : public RMEvent {
public:
    round_freeze_end(IGameEvent *event) {
    };
    std::string event_name = "round_freeze_end";

    bool Emit() override {
        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length())
        };

        void *objptr = metacallv("_FreezeTimeEndedEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
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

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);

        CCSPlayerController *at_controller = (CCSPlayerController *) this->attacker;
        this->attacker_player = new Player(at_controller);

        CCSPlayerController *as_controller = (CCSPlayerController *) this->assister;
        this->assister_player = new Player(as_controller);
    };
    std::string event_name = "player_death";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *attacker;//playercontroller
    CEntityInstance *assister;//playercontroller
    bool assistedflash;
    std::string weapon;
    std::string weapon_itemid;
    std::string weapon_fauxitemid;
    std::string weapon_originalowner_xuid;
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

    Player *player;
    Player *attacker_player;
    Player *assister_player;

    bool Emit() override {
        void *playerArgs[] = {
                metacall_value_create_string(this->player->controller->GetPlayerName(),
                                             strlen(this->player->controller->GetPlayerName())),
                metacall_value_create_long(this->player->controller->m_steamID),
                metacall_value_create_int(this->player->controller->GetPlayerSlot())
        };

        void *playerObj = metacallv("_Player", playerArgs);

        void *attackerObj;
        if (this->attacker_player->controller != nullptr) {
            void *attackerArgs[] = {
                    metacall_value_create_string(this->attacker_player->controller->GetPlayerName(),
                                                 strlen(this->attacker_player->controller->GetPlayerName())),
                    metacall_value_create_long(this->attacker_player->controller->m_steamID),
                    metacall_value_create_int(this->attacker_player->controller->GetPlayerSlot())
            };

            attackerObj = metacallv("_Player", attackerArgs);

            metacall_value_destroy(attackerArgs[0]);
            metacall_value_destroy(attackerArgs[1]);
            metacall_value_destroy(attackerArgs[2]);
        } else {
            attackerObj = metacall_value_create_null();
        }

        void *assisterObj;
        if (this->assister_player->controller != nullptr) {
            void *assisterArgs[] = {
                    metacall_value_create_string(this->assister_player->controller->GetPlayerName(),
                                                 strlen(this->assister_player->controller->GetPlayerName())),
                    metacall_value_create_long(this->assister_player->controller->m_steamID),
                    metacall_value_create_int(this->assister_player->controller->GetPlayerSlot())
            };

            assisterObj = metacallv("_Player", assisterArgs);

            metacall_value_destroy(assisterArgs[0]);
            metacall_value_destroy(assisterArgs[1]);
            metacall_value_destroy(assisterArgs[2]);
        } else {
            assisterObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_bool(this->assistedflash),
                metacall_value_create_string(this->weapon.c_str(), this->weapon.length()),
                metacall_value_create_string(this->weapon_itemid.c_str(), this->weapon_itemid.length()),
                metacall_value_create_string(this->weapon_fauxitemid.c_str(), this->weapon_fauxitemid.length()),
                metacall_value_create_string(this->weapon_originalowner_xuid.c_str(),
                                             this->weapon_originalowner_xuid.length()),
                metacall_value_create_bool(this->headshot),
                metacall_value_create_int(this->dominated),
                metacall_value_create_int(this->revenge),
                metacall_value_create_int(this->wipe),
                metacall_value_create_int(this->penetrated),
                metacall_value_create_bool(this->noreplay),
                metacall_value_create_bool(this->noscope),
                metacall_value_create_bool(this->thrusmoke),
                metacall_value_create_bool(this->attackerblind),
                metacall_value_create_float(this->distance),
                playerObj,
                attackerObj,
                assisterObj
        };

        void *objptr = metacallv("_PlayerDeathEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        metacall_value_destroy(playerArgs[0]);
        metacall_value_destroy(playerArgs[1]);
        metacall_value_destroy(playerArgs[2]);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class player_footstep : public RMEvent {
public:
    player_footstep(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "player_footstep";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *userid_pawn;//strict_ehandle

    Player *player;

    bool Emit() override {
        void *playerArgs[] = {
                metacall_value_create_string(this->player->controller->GetPlayerName(),
                                             strlen(this->player->controller->GetPlayerName())),
                metacall_value_create_long(this->player->controller->m_steamID),
                metacall_value_create_int(this->player->controller->GetPlayerSlot())
        };

        void *playerObj = metacallv("_Player", playerArgs);

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                playerObj
        };

        void *objptr = metacallv("_PlayerFootstepEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        metacall_value_destroy(playerArgs[0]);
        metacall_value_destroy(playerArgs[1]);
        metacall_value_destroy(playerArgs[2]);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class break_prop : public RMEvent {
public:
    break_prop(IGameEvent *event) {
        this->entindex = event->GetInt("entindex");
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "break_prop";
    int entindex;
    CEntityInstance *userid;// player_pawn
    CEntityInstance *userid_pawn; //strict_ehandle

    Player *player;

    bool Emit() override {
        void *playerObj;
        if (this->player->controller != nullptr) {
            void *playerArgs[] = {
                    metacall_value_create_string(this->player->controller->GetPlayerName(),
                                                 strlen(this->player->controller->GetPlayerName())),
                    metacall_value_create_long(this->player->controller->m_steamID),
                    metacall_value_create_int(this->player->controller->GetPlayerSlot())
            };

            playerObj = metacallv("_Player", playerArgs);

            metacall_value_destroy(playerArgs[0]);
            metacall_value_destroy(playerArgs[1]);
            metacall_value_destroy(playerArgs[2]);
        } else {
            playerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->entindex),
                playerObj
        };

        void *objptr = metacallv("_BreakPropEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class item_purchase : public RMEvent {
public:
    item_purchase(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->team = event->GetInt("team");
        this->loadout = event->GetInt("loadout");
        this->weapon = event->GetString("weapon");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "item_purchase";
    CEntityInstance *userid; //playercontroller
    int team;
    int loadout;
    std::string weapon;

    Player *player;

    bool Emit() override {
        void *playerArgs[] = {
                metacall_value_create_string(this->player->controller->GetPlayerName(),
                                             strlen(this->player->controller->GetPlayerName())),
                metacall_value_create_long(this->player->controller->m_steamID),
                metacall_value_create_int(this->player->controller->GetPlayerSlot())
        };

        void *playerObj = metacallv("_Player", playerArgs);

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->team),
                metacall_value_create_int(this->loadout),
                metacall_value_create_string(this->weapon.c_str(), this->weapon.length()),
                playerObj
        };

        void *objptr = metacallv("_ItemPurchaseEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        metacall_value_destroy(playerArgs[0]);
        metacall_value_destroy(playerArgs[1]);
        metacall_value_destroy(playerArgs[2]);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class bomb_beginplant : public RMEvent {
public:
    bomb_beginplant(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->site = event->GetInt("site");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "bomb_beginplant";
    CEntityInstance *userid;//playercontroller
    int site;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player *player;

    bool Emit() override {
        void *playerObj;
        if (this->player->controller != nullptr) {
            void *playerArgs[] = {
                    metacall_value_create_string(this->player->controller->GetPlayerName(),
                                                 strlen(this->player->controller->GetPlayerName())),
                    metacall_value_create_long(this->player->controller->m_steamID),
                    metacall_value_create_int(this->player->controller->GetPlayerSlot())
            };

            playerObj = metacallv("_Player", playerArgs);
            metacall_value_destroy(playerArgs[0]);
            metacall_value_destroy(playerArgs[1]);
            metacall_value_destroy(playerArgs[2]);
        } else {
            playerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->site),
                playerObj
        };

        void *objptr = metacallv("_BombBeginPlantEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class bomb_planted : public RMEvent {
public:
    bomb_planted(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->site = event->GetInt("site");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "bomb_planted";
    CEntityInstance *userid;//playercontroller
    int site;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player *player;

    bool Emit() override {
        void *playerObj;
        if (this->player->controller != nullptr) {
            void *playerArgs[] = {
                    metacall_value_create_string(this->player->controller->GetPlayerName(),
                                                 strlen(this->player->controller->GetPlayerName())),
                    metacall_value_create_long(this->player->controller->m_steamID),
                    metacall_value_create_int(this->player->controller->GetPlayerSlot())
            };

            playerObj = metacallv("_Player", playerArgs);
            metacall_value_destroy(playerArgs[0]);
            metacall_value_destroy(playerArgs[1]);
            metacall_value_destroy(playerArgs[2]);
        } else {
            playerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->site),
                playerObj
        };

        void *objptr = metacallv("_BombPlantedEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class bomb_defused : public RMEvent {
public:
    bomb_defused(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->site = event->GetInt("site");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "bomb_defused";
    CEntityInstance *userid;//playercontroller
    int site;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player *player;

    bool Emit() override {
        void *playerObj;
        if (this->player->controller != nullptr) {
            void *playerArgs[] = {
                    metacall_value_create_string(this->player->controller->GetPlayerName(),
                                                 strlen(this->player->controller->GetPlayerName())),
                    metacall_value_create_long(this->player->controller->m_steamID),
                    metacall_value_create_int(this->player->controller->GetPlayerSlot())
            };

            playerObj = metacallv("_Player", playerArgs);
            metacall_value_destroy(playerArgs[0]);
            metacall_value_destroy(playerArgs[1]);
            metacall_value_destroy(playerArgs[2]);
        } else {
            playerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->site),
                playerObj
        };

        void *objptr = metacallv("_BombDefusedEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class bomb_exploded : public RMEvent {
public:
    bomb_exploded(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->site = event->GetInt("site");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "bomb_exploded";
    CEntityInstance *userid;//playercontroller
    int site;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player *player;

    bool Emit() override {
        void *playerObj;
        if (this->player->controller != nullptr) {
            void *playerArgs[] = {
                    metacall_value_create_string(this->player->controller->GetPlayerName(),
                                                 strlen(this->player->controller->GetPlayerName())),
                    metacall_value_create_long(this->player->controller->m_steamID),
                    metacall_value_create_int(this->player->controller->GetPlayerSlot())
            };

            playerObj = metacallv("_Player", playerArgs);
            metacall_value_destroy(playerArgs[0]);
            metacall_value_destroy(playerArgs[1]);
            metacall_value_destroy(playerArgs[2]);
        } else {
            playerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->site),
                playerObj
        };

        void *objptr = metacallv("_BombExplodedEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class bomb_dropped : public RMEvent {
public:
    bomb_dropped(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->entindex = event->GetInt("entindex");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "bomb_dropped";
    CEntityInstance *userid;//playercontroller
    int entindex;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player *player;

    bool Emit() override {
        void *playerObj;
        if (this->player->controller != nullptr) {
            void *playerArgs[] = {
                    metacall_value_create_string(this->player->controller->GetPlayerName(),
                                                 strlen(this->player->controller->GetPlayerName())),
                    metacall_value_create_long(this->player->controller->m_steamID),
                    metacall_value_create_int(this->player->controller->GetPlayerSlot())
            };

            playerObj = metacallv("_Player", playerArgs);
            metacall_value_destroy(playerArgs[0]);
            metacall_value_destroy(playerArgs[1]);
            metacall_value_destroy(playerArgs[2]);
        } else {
            playerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->entindex),
                playerObj
        };

        void *objptr = metacallv("_BombDroppedEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class bomb_pickup : public RMEvent {
public:
    bomb_pickup(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "bomb_pickup";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *userid_pawn;//strict_ehandle

    Player *player;

    bool Emit() override {
        void *playerObj;

        if (this->player->controller != nullptr) {
            void *playerArgs[] = {
                    metacall_value_create_string(this->player->controller->GetPlayerName(),
                                                 strlen(this->player->controller->GetPlayerName())),
                    metacall_value_create_long(this->player->controller->m_steamID),
                    metacall_value_create_int(this->player->controller->GetPlayerSlot())
            };

            playerObj = metacallv("_Player", playerArgs);

            metacall_value_destroy(playerArgs[0]);
            metacall_value_destroy(playerArgs[1]);
            metacall_value_destroy(playerArgs[2]);
        } else {
            playerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                playerObj
        };

        void *objptr = metacallv("_BombPickedUpEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class defuser_dropped : public RMEvent {
public:
    defuser_dropped(IGameEvent *event) {
        this->entityid = event->GetInt("entityid");
    };
    std::string event_name = "defuser_dropped";
    int entityid;

    bool Emit() override {
        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->entityid),
        };

        void *objptr = metacallv("_DefuserDroppedEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class defuser_pickup : public RMEvent {
public:
    defuser_pickup(IGameEvent *event) {
        this->entityid = event->GetInt("entityid");
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "defuser_pickup";
    int entityid;
    CEntityInstance *userid;//playercontroller
    CEntityInstance *userid_pawn;//strict_ehandle

    Player *player;

    bool Emit() override {
        void *playerObj;

        if (this->player->controller != nullptr) {
            void *playerArgs[] = {
                    metacall_value_create_string(this->player->controller->GetPlayerName(),
                                                 strlen(this->player->controller->GetPlayerName())),
                    metacall_value_create_long(this->player->controller->m_steamID),
                    metacall_value_create_int(this->player->controller->GetPlayerSlot())
            };

            playerObj = metacallv("_Player", playerArgs);

            metacall_value_destroy(playerArgs[0]);
            metacall_value_destroy(playerArgs[1]);
            metacall_value_destroy(playerArgs[2]);
        } else {
            playerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->entityid),
                playerObj
        };

        void *objptr = metacallv("_DefuserPickupEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class bomb_begindefuse : public RMEvent {
public:
    bomb_begindefuse(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->haskit = event->GetBool("haskit");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "bomb_begindefuse";
    CEntityInstance *userid; //playercontroller
    bool haskit;
    CEntityInstance *userid_pawn; //strict_ehandle

    Player *player;

    bool Emit() override {
        void *playerObj;

        if (this->player->controller != nullptr) {
            void *playerArgs[] = {
                    metacall_value_create_string(this->player->controller->GetPlayerName(),
                                                 strlen(this->player->controller->GetPlayerName())),
                    metacall_value_create_long(this->player->controller->m_steamID),
                    metacall_value_create_int(this->player->controller->GetPlayerSlot())
            };

            playerObj = metacallv("_Player", playerArgs);

            metacall_value_destroy(playerArgs[0]);
            metacall_value_destroy(playerArgs[1]);
            metacall_value_destroy(playerArgs[2]);
        } else {
            playerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_bool(this->haskit),
                playerObj
        };

        void *objptr = metacallv("_BeginDefuseEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class bomb_abortdefuse : public RMEvent {
public:
    bomb_abortdefuse(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "bomb_abortdefuse";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *userid_pawn;//strict_ehandle

    Player *player;

    bool Emit() override {
        void *playerObj;

        if (this->player->controller != nullptr) {
            void *playerArgs[] = {
                    metacall_value_create_string(this->player->controller->GetPlayerName(),
                                                 strlen(this->player->controller->GetPlayerName())),
                    metacall_value_create_long(this->player->controller->m_steamID),
                    metacall_value_create_int(this->player->controller->GetPlayerSlot())
            };

            playerObj = metacallv("_Player", playerArgs);

            metacall_value_destroy(playerArgs[0]);
            metacall_value_destroy(playerArgs[1]);
            metacall_value_destroy(playerArgs[2]);
        } else {
            playerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                playerObj
        };

        void *objptr = metacallv("_AbortDefuseEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};


class hostage_follows : public RMEvent {
public:
    hostage_follows(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hostage = event->GetInt("hostage");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "hostage_follows";
    CEntityInstance *userid; //playercontroller
    int hostage;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    bool Emit() override {
        void *playerObj;

        if (this->player->controller != nullptr) {
            void *playerArgs[] = {
                    metacall_value_create_string(this->player->controller->GetPlayerName(),
                                                 strlen(this->player->controller->GetPlayerName())),
                    metacall_value_create_long(this->player->controller->m_steamID),
                    metacall_value_create_int(this->player->controller->GetPlayerSlot())
            };

            playerObj = metacallv("_Player", playerArgs);

            metacall_value_destroy(playerArgs[0]);
            metacall_value_destroy(playerArgs[1]);
            metacall_value_destroy(playerArgs[2]);
        } else {
            playerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->hostage),
                playerObj
        };

        void *objptr = metacallv("_HostageBeginsFollowingEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class hostage_hurt : public RMEvent {
public:
    hostage_hurt(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hostage = event->GetInt("hostage");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "hostage_hurt";
    CEntityInstance *userid;//playercontroller
    int hostage;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    bool Emit() override {
        void *playerObj;

        if (this->player->controller != nullptr) {
            void *playerArgs[] = {
                    metacall_value_create_string(this->player->controller->GetPlayerName(),
                                                 strlen(this->player->controller->GetPlayerName())),
                    metacall_value_create_long(this->player->controller->m_steamID),
                    metacall_value_create_int(this->player->controller->GetPlayerSlot())
            };

            playerObj = metacallv("_Player", playerArgs);

            metacall_value_destroy(playerArgs[0]);
            metacall_value_destroy(playerArgs[1]);
            metacall_value_destroy(playerArgs[2]);
        } else {
            playerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->hostage),
                playerObj
        };

        void *objptr = metacallv("_HostageHurtEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class hostage_rescued : public RMEvent {
public:
    hostage_rescued(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hostage = event->GetInt("hostage");
        this->site = event->GetInt("site");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "hostage_rescued";
    CEntityInstance *userid;//playercontroller
    int hostage;
    int site;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;
    bool Emit() override {
        void *playerObj;

        if (this->player->controller != nullptr) {
            void *playerArgs[] = {
                    metacall_value_create_string(this->player->controller->GetPlayerName(),
                                                 strlen(this->player->controller->GetPlayerName())),
                    metacall_value_create_long(this->player->controller->m_steamID),
                    metacall_value_create_int(this->player->controller->GetPlayerSlot())
            };

            playerObj = metacallv("_Player", playerArgs);

            metacall_value_destroy(playerArgs[0]);
            metacall_value_destroy(playerArgs[1]);
            metacall_value_destroy(playerArgs[2]);
        } else {
            playerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->hostage),
                metacall_value_create_int(this->site),
                playerObj
        };

        void *objptr = metacallv("_HostageRescuedEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class hostage_stops_following : public RMEvent {
public:
    hostage_stops_following(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hostage = event->GetInt("hostage");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "hostage_stops_following";
    CEntityInstance *userid;//playercontroller
    int hostage;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    bool Emit() override {
        void *playerObj;

        if (this->player->controller != nullptr) {
            void *playerArgs[] = {
                    metacall_value_create_string(this->player->controller->GetPlayerName(),
                                                 strlen(this->player->controller->GetPlayerName())),
                    metacall_value_create_long(this->player->controller->m_steamID),
                    metacall_value_create_int(this->player->controller->GetPlayerSlot())
            };

            playerObj = metacallv("_Player", playerArgs);

            metacall_value_destroy(playerArgs[0]);
            metacall_value_destroy(playerArgs[1]);
            metacall_value_destroy(playerArgs[2]);
        } else {
            playerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->hostage),
                playerObj
        };

        void *objptr = metacallv("_HostageStopsFollowingEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};


class weapon_fire : public RMEvent {
public:
    weapon_fire(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->weapon = event->GetString("weapon");
        this->silenced = event->GetBool("silenced");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "weapon_fire";
    CEntityInstance *userid;//playercontroller
    std::string weapon;
    bool silenced;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player* player;

    bool Emit() override {
        void *playerObj;

        if (this->player->controller != nullptr) {
            void *playerArgs[] = {
                    metacall_value_create_string(this->player->controller->GetPlayerName(),
                                                 strlen(this->player->controller->GetPlayerName())),
                    metacall_value_create_long(this->player->controller->m_steamID),
                    metacall_value_create_int(this->player->controller->GetPlayerSlot())
            };

            playerObj = metacallv("_Player", playerArgs);

            metacall_value_destroy(playerArgs[0]);
            metacall_value_destroy(playerArgs[1]);
            metacall_value_destroy(playerArgs[2]);
        } else {
            playerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_string(this->weapon.c_str(), this->weapon.length()),
                metacall_value_create_bool(this->silenced),
                playerObj
        };

        void *objptr = metacallv("_WeaponFireEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class weapon_reload : public RMEvent {
public:
    weapon_reload(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "weapon_reload";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *userid_pawn;//strict_ehandle;

    Player* player;

    bool Emit() override {
        void *playerObj;

        if (this->player->controller != nullptr) {
            void *playerArgs[] = {
                    metacall_value_create_string(this->player->controller->GetPlayerName(),
                                                 strlen(this->player->controller->GetPlayerName())),
                    metacall_value_create_long(this->player->controller->m_steamID),
                    metacall_value_create_int(this->player->controller->GetPlayerSlot())
            };

            playerObj = metacallv("_Player", playerArgs);

            metacall_value_destroy(playerArgs[0]);
            metacall_value_destroy(playerArgs[1]);
            metacall_value_destroy(playerArgs[2]);
        } else {
            playerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                playerObj
        };

        void *objptr = metacallv("_WeaponReloadEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class weapon_zoom : public RMEvent {
public:
    weapon_zoom(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "weapon_zoom";
    CEntityInstance *userid;//playercontroller
    CEntityInstance *userid_pawn;//strict_ehandle;

    Player* player;

    bool Emit() override {
        void *playerObj;

        if (this->player->controller != nullptr) {
            void *playerArgs[] = {
                    metacall_value_create_string(this->player->controller->GetPlayerName(),
                                                 strlen(this->player->controller->GetPlayerName())),
                    metacall_value_create_long(this->player->controller->m_steamID),
                    metacall_value_create_int(this->player->controller->GetPlayerSlot())
            };

            playerObj = metacallv("_Player", playerArgs);

            metacall_value_destroy(playerArgs[0]);
            metacall_value_destroy(playerArgs[1]);
            metacall_value_destroy(playerArgs[2]);
        } else {
            playerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                playerObj
        };

        void *objptr = metacallv("_WeaponZoomEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class client_disconnected {
public:
    client_disconnected(int slot) {
        this->event_name = "client_disconnected";
        this->slot = slot;
        this->controller = CCSPlayerController::FromSlot(slot);
        this->player = new Player(controller);
    };
    std::string event_name = "client_disconnected";
    int slot;
    CCSPlayerController *controller;
    Player *player;

    bool Emit() {
        void *playerArgs[] = {
                metacall_value_create_string(this->player->controller->GetPlayerName(),
                                             strlen(this->player->controller->GetPlayerName())),
                metacall_value_create_long(this->player->controller->m_steamID),
                metacall_value_create_int(this->slot)
        };

        void *playerObj = metacallv("_Player", playerArgs);

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                playerObj,
        };

        void *objptr = metacallv("_ClientDisconnectedEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        metacall_value_destroy(playerArgs[0]);
        metacall_value_destroy(playerArgs[1]);
        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class client_put_in_server {
public:
    client_put_in_server(int slot) {
        this->event_name = "client_put_in_server";
        this->slot = slot;
        this->controller = CCSPlayerController::FromSlot(slot);
        this->player = new Player(controller);
    };
    std::string event_name = "client_put_in_server";
    int slot;
    CCSPlayerController *controller;
    Player *player;

    bool Emit() {
        void *playerArgs[] = {
                metacall_value_create_string(this->player->controller->GetPlayerName(),
                                             strlen(this->player->controller->GetPlayerName())),
                metacall_value_create_long(this->player->controller->m_steamID),
                metacall_value_create_int(this->slot)
        };

        void *playerObj = metacallv("_Player", playerArgs);

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                playerObj,
        };

        void *objptr = metacallv("_ClientPutInServerEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        metacall_value_destroy(playerArgs[0]);
        metacall_value_destroy(playerArgs[1]);
        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class client_connected {
public:
    client_connected(const char *pszName, uint64 xuid, const char *pszNetworkID,
                     const char *pszAddress, bool bFakePlayer) {
        this->event_name = "client_connected";
        this->pszName = pszName;
        this->xuid = xuid;
        this->pszNetworkID = pszNetworkID;
        this->pszAddress = pszAddress;
        size_t colonPos = this->pszAddress.find(':');
        if (colonPos != std::string::npos) {
            this->ip = this->pszAddress.substr(0, colonPos);
        }
        this->bFakePlayer = bFakePlayer;
    };
    std::string event_name = "client_connected";

    std::string pszName;
    uint64 xuid;
    std::string pszNetworkID;
    std::string pszAddress;
    std::string ip;
    bool bFakePlayer;

    bool Emit() {
        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_long(this->xuid),
                metacall_value_create_string(this->ip.c_str(), this->ip.length()),
                metacall_value_create_bool(this->bFakePlayer),
        };

        void *objptr = metacallv("_ClientConnectedEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class client_connect {
public:
    client_connect(const char *pszName, uint64 xuid, const char *pszNetworkID) {
        this->event_name = "client_connect";
        this->pszName = pszName;
        this->xuid = xuid;
        this->pszNetworkID = pszNetworkID;
    };
    std::string event_name = "client_connect";

    std::string pszName;
    uint64 xuid;
    std::string pszNetworkID;

    bool Emit() {
        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_long(this->xuid),
        };

        void *objptr = metacallv("_ClientConnectEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
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

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };

    std::string event_name = "item_pickup";
    CEntityInstance *userid;//playercontroller
    Player *player;
    std::string item;
    bool silent;
    int defindex;

    bool Emit() override {
        void *playerArgs[] = {
                metacall_value_create_string(this->player->controller->GetPlayerName(),
                                             strlen(this->player->controller->GetPlayerName())),
                metacall_value_create_long(this->player->controller->m_steamID),
                metacall_value_create_int(this->player->controller->GetPlayerSlot())
        };

        void *playerObj = metacallv("_Player", playerArgs);

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_string(this->item.c_str(), this->item.length()),
                metacall_value_create_bool(this->silent),
                metacall_value_create_int(this->defindex),
                playerObj,
        };

        void *objptr = metacallv("_ItemPickupEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        metacall_value_destroy(playerArgs[0]);
        metacall_value_destroy(playerArgs[1]);
        metacall_value_destroy(playerArgs[2]);
        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class enter_buyzone : public RMEvent {
public:
    enter_buyzone(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->canbuy = event->GetBool("canbuy");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "enter_buyzone";
    CEntityInstance *userid;//playercontroller
    bool canbuy;

    Player *player;

    bool Emit() override {
        void *playerArgs[] = {
                metacall_value_create_string(this->player->controller->GetPlayerName(),
                                             strlen(this->player->controller->GetPlayerName())),
                metacall_value_create_long(this->player->controller->m_steamID),
                metacall_value_create_int(this->player->controller->GetPlayerSlot())
        };

        void *playerObj = metacallv("_Player", playerArgs);

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_bool(this->canbuy),
                playerObj,
        };

        void *objptr = metacallv("_EnterBuyZoneEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        metacall_value_destroy(playerArgs[0]);
        metacall_value_destroy(playerArgs[1]);
        metacall_value_destroy(playerArgs[2]);
        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class exit_buyzone : public RMEvent {
public:
    exit_buyzone(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->canbuy = event->GetBool("canbuy");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "exit_buyzone";
    CEntityInstance *userid; //playercontroller
    bool canbuy;

    Player *player;
    bool Emit() override {
        void *playerArgs[] = {
                metacall_value_create_string(this->player->controller->GetPlayerName(),
                                             strlen(this->player->controller->GetPlayerName())),
                metacall_value_create_long(this->player->controller->m_steamID),
                metacall_value_create_int(this->player->controller->GetPlayerSlot())
        };

        void *playerObj = metacallv("_Player", playerArgs);

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_bool(this->canbuy),
                playerObj,
        };

        void *objptr = metacallv("_ExitBuyZoneEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        metacall_value_destroy(playerArgs[0]);
        metacall_value_destroy(playerArgs[1]);
        metacall_value_destroy(playerArgs[2]);
        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};


class buytime_ended : public RMEvent {
public:
    buytime_ended(IGameEvent *event) {

    };
    std::string event_name = "buytime_ended";

    bool Emit() override {
        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length())
        };

        void *objptr = metacallv("_BuyTimeEndedEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};


class enter_bombzone : public RMEvent {
public:
    enter_bombzone(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hasbomb = event->GetBool("hasbomb");
        this->isplanted = event->GetBool("isplanted");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "enter_bombzone";
    CEntityInstance *userid;//playercontroller
    bool hasbomb;
    bool isplanted;

    Player *player;

    bool Emit() override {
        void *playerArgs[] = {
                metacall_value_create_string(this->player->controller->GetPlayerName(),
                                             strlen(this->player->controller->GetPlayerName())),
                metacall_value_create_long(this->player->controller->m_steamID),
                metacall_value_create_int(this->player->controller->GetPlayerSlot())
        };

        void *playerObj = metacallv("_Player", playerArgs);

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_bool(this->hasbomb),
                metacall_value_create_bool(this->isplanted),
                playerObj,
        };

        void *objptr = metacallv("_EnterBombZoneEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        metacall_value_destroy(playerArgs[0]);
        metacall_value_destroy(playerArgs[1]);
        metacall_value_destroy(playerArgs[2]);
        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class exit_bombzone : public RMEvent {
public:
    exit_bombzone(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->hasbomb = event->GetBool("hasbomb");
        this->isplanted = event->GetBool("isplanted");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "exit_bombzone";
    CEntityInstance *userid;//playercontroller
    bool hasbomb;
    bool isplanted;

    Player* player;

    bool Emit() override {
        void *playerArgs[] = {
                metacall_value_create_string(this->player->controller->GetPlayerName(),
                                             strlen(this->player->controller->GetPlayerName())),
                metacall_value_create_long(this->player->controller->m_steamID),
                metacall_value_create_int(this->player->controller->GetPlayerSlot())
        };

        void *playerObj = metacallv("_Player", playerArgs);

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_bool(this->hasbomb),
                metacall_value_create_bool(this->isplanted),
                playerObj,
        };

        void *objptr = metacallv("_ExitBombZoneEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        metacall_value_destroy(playerArgs[0]);
        metacall_value_destroy(playerArgs[1]);
        metacall_value_destroy(playerArgs[2]);
        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

/*
class enter_rescue_zone : public RMEvent {
public:
    enter_rescue_zone(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
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

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
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

class buymenu_open : public RMEvent {
public:
    buymenu_open(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
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

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
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
*/

class player_blind : public RMEvent {
public:
    player_blind(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->attacker = event->GetPlayerController("attacker");
        this->entityid = event->GetInt("entityid");
        this->blind_duration = event->GetFloat("blind_duration");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);

        CCSPlayerController *at_controller = (CCSPlayerController *) this->attacker;
        this->_attacker = new Player(at_controller);
    };
    std::string event_name = "player_blind";
    CEntityInstance *userid; //playercontroller
    CEntityInstance *attacker; //playercontroller
    int entityid;
    float blind_duration;

    Player *player;
    Player *_attacker;

    bool Emit() override {
        void *playerArgs[] = {
                metacall_value_create_string(this->player->controller->GetPlayerName(),
                                             strlen(this->player->controller->GetPlayerName())),
                metacall_value_create_long(this->player->controller->m_steamID),
                metacall_value_create_int(this->player->controller->GetPlayerSlot())
        };

        void *playerObj = metacallv("_Player", playerArgs);
        void *attackerObj;
        if (this->_attacker->controller != nullptr) {
            void *attackerArgs[] = {
                    metacall_value_create_string(this->_attacker->controller->GetPlayerName(),
                                                 strlen(this->_attacker->controller->GetPlayerName())),
                    metacall_value_create_long(this->_attacker->controller->m_steamID),
                    metacall_value_create_int(this->_attacker->controller->GetPlayerSlot())
            };

            attackerObj = metacallv("_Player", attackerArgs);

            metacall_value_destroy(attackerArgs[0]);
            metacall_value_destroy(attackerArgs[1]);
            metacall_value_destroy(attackerArgs[2]);
        } else {
            attackerObj = metacall_value_create_null();
        }

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->entityid),
                metacall_value_create_float(this->blind_duration),
                playerObj,
                attackerObj
        };

        void *objptr = metacallv("_PlayerBlindEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        metacall_value_destroy(playerArgs[0]);
        metacall_value_destroy(playerArgs[1]);
        metacall_value_destroy(playerArgs[2]);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class player_falldamage : public RMEvent {
public:
    player_falldamage(IGameEvent *event) {
        this->userid = event->GetPlayerController("userid");
        this->damage = event->GetFloat("damage");
        this->userid_pawn = event->GetPlayerPawn("userid_pawn");

        CCSPlayerController *controller = (CCSPlayerController *) this->userid;
        this->player = new Player(controller);
    };
    std::string event_name = "player_falldamage";
    CEntityInstance *userid;//playercontroller
    float damage;
    CEntityInstance *userid_pawn;//strict_ehandle

    Player *player;

    bool Emit() override {
        void *playerArgs[] = {
                metacall_value_create_string(this->player->controller->GetPlayerName(),
                                             strlen(this->player->controller->GetPlayerName())),
                metacall_value_create_long(this->player->controller->m_steamID),
                metacall_value_create_int(this->player->controller->GetPlayerSlot())
        };

        void *playerObj = metacallv("_Player", playerArgs);

        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_int(this->damage),
                playerObj
        };

        void *objptr = metacallv("_PlayerFallDamageEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);
        metacall_value_destroy(playerArgs[0]);
        metacall_value_destroy(playerArgs[1]);
        metacall_value_destroy(playerArgs[2]);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

class nextlevel_changed : public RMEvent {
public:
    nextlevel_changed(IGameEvent *event) {
        this->nextlevel = event->GetString("nextlevel");
        this->mapgroup = event->GetString("mapgroup");
        this->skirmishmode = event->GetString("skirmishmode");
    };
    std::string event_name = "nextlevel_changed";
    std::string nextlevel;
    std::string mapgroup;
    std::string skirmishmode;

    bool Emit() override {
        void *args[] = {
                metacall_value_create_string(this->event_name.c_str(), this->event_name.length()),
                metacall_value_create_string(this->nextlevel.c_str(), this->nextlevel.length()),
                metacall_value_create_string(this->mapgroup.c_str(), this->mapgroup.length()),
                metacall_value_create_string(this->skirmishmode.c_str(), this->skirmishmode.length()),
        };

        void *objptr = metacallv("_NextLevelChangedEvent", args);
        void *call[] = {
                objptr
        };
        void *ret = metacallv("_onEventCall", call);

        std::string result = metacall_value_to_string(ret);

        for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++) {
            metacall_value_destroy(args[i]);
        }
        metacall_value_destroy(objptr);
        metacall_value_destroy(ret);

        return result == "event_prevent_default";
    };
};

#endif //RESOURCEMOD_LEGACYEVENTS_H
