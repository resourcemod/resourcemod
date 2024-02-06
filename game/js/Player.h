//
// Created by Twelvee on 13.11.2023.
//

#ifndef RESOURCEMOD_PLAYER_H
#define RESOURCEMOD_PLAYER_H

#include "../cs2/cbaseplayercontroller.h"
#include "../cs2/ccsplayercontroller.h"
#include "../hooks/LegacyEvents.h"
#include "../ResourceMod.h"
#include <engine/igameeventsystem.h>
#include <igameevents.h>
#include <metacall/metacall.h>

class Player;

extern ResourceMod *g_ResourceMod;
extern IGameEventManager2 *g_gameEventManager;

class Player {
public:
    Player(CCSPlayerController *c) {
        this->controller = c;
    };

    CCSPlayerController *controller;

    static void *GetHP(size_t argc, void *args[], void *data) {
        CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
        return metacall_value_create_int(c->GetPawn()->GetHP());
    }

    static void *SetHP(size_t argc, void *args[], void *data) {
        CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
        c->GetPawn()->SetHP(metacall_value_to_int(args[1]));
        return metacall_value_create_bool(true);
    }

    static void *GetName(size_t argc, void *args[], void *data) {
        CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
        std::string name = c->GetPlayerName();
        return metacall_value_create_string(name.c_str(), name.length());
    }

    static void *GetSteamID(size_t argc, void *args[], void *data) {
        CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
        uint64 value = c->m_steamID;
        return metacall_value_create_long(value);
    }

    static void *GetIsAlive(size_t argc, void *args[], void *data) {
        CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
        return metacall_value_create_bool(c->IsAlive());
    }

    static void *Slap(size_t argc, void *args[], void *data) {
        CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
        c->GetPawn()->Slap(metacall_value_to_int(args[1]));
        return metacall_value_create_bool(true);
    }

    static void *Slay(size_t argc, void *args[], void *data) {
        CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
        c->GetPawn()->CommitSuicide(true, true);
        return metacall_value_create_bool(true);
    }

    static void *Respawn(size_t argc, void *args[], void *data) {
        CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
        return metacall_value_create_bool(c->Respawn());
    }

    static void *GetTeam(size_t argc, void *args[], void *data) {
        CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
        return metacall_value_create_int(c->m_iTeamNum.Get());
    }

    static void *ChangeTeam(size_t argc, void *args[], void *data) {
        CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));

        int team = metacall_value_to_int(args[1]);
        if (team < 0 || team > 3) {
            void *throwArgs[] = {
                    metacall_value_create_string("Invalid team parameter. 0-3 range expected.",
                                                 strlen("Invalid team parameter. 0-3 range expected.")),
            };
            metacallv("_throw_exception", throwArgs);
            return metacall_value_create_bool(false);
        }
        if (argc > 2) {
            bool kill = metacall_value_to_bool(args[2]);
            if (kill) {
                c->ChangeTeam(team);
            } else {
                c->SwitchTeam(team);
            }
        } else {
            c->SwitchTeam(team);
        }

        return metacall_value_create_bool(true);
    }

    static void *GetIsConnected(size_t argc, void *args[], void *data) {
        CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
        return metacall_value_create_bool(c->IsConnected());
    }

    static void *GetIsConnecting(size_t argc, void *args[], void *data) {
        CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
        return metacall_value_create_bool(c->IsConnecting());
    }

    static void *GetIsDisconnected(size_t argc, void *args[], void *data) {
        CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
        return metacall_value_create_bool(c->IsDisconnected());
    }

    static void ClientPrintAll(int hud_dest, const char *msg, ...) {
        va_list args;
                va_start(args, msg);
        char buf[256];
        V_vsnprintf(buf, sizeof(buf), msg, args);
                va_end(args);

        SignatureCall::UTIL_ClientPrintAll(hud_dest, buf, nullptr, nullptr, nullptr, nullptr);
    }

    static void ClientPrint(CCSPlayerController *player, int dest, const char *text, ...) {
        va_list args;
                va_start(args, text);

        char buf[256];
        V_vsnprintf(buf, sizeof(buf), text, args);
                va_end(args);
        g_ResourceMod->NextFrame([player, buf, dest]() {
            if (player->m_hPawn() && player->m_steamID() > 0)
                SignatureCall::UTIL_ClientPrint(player, dest, buf, nullptr, nullptr, nullptr, nullptr);
        });
    }

    static void *PrintAll(size_t argc, void *args[], void *data) {
        int dest = metacall_value_to_int(args[0]);
        std::string msg = metacall_value_to_string(args[1]);

        Player::ClientPrintAll(dest, msg.c_str());

        return metacall_value_create_bool(true);
    }

    static void *Print(size_t argc, void *args[], void *data) {
        int slot = metacall_value_to_int(args[0]);
        int dest = metacall_value_to_int(args[1]);
        std::string msg = metacall_value_to_string(args[2]);

        CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
        Player::ClientPrint(c, dest, msg.c_str());

        return metacall_value_create_bool(true);
    }

    static void *GetIsDisconnecting(size_t argc, void *args[], void *data) {
        CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
        return metacall_value_create_bool(c->IsDisconnecting());
    }

    static void *GetIsReserved(size_t argc, void *args[], void *data) {
        CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
        return metacall_value_create_bool(c->IsReserved());
    }

    static void *GetIsReconnecting(size_t argc, void *args[], void *data) {
        CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
        return metacall_value_create_bool(c->IsReconnecting());
    }
};

#endif //RESOURCEMOD_PLAYER_H
