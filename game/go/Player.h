//
// Created by Twelvee on 13.11.2023.
//

#ifndef RESOURCEMOD_PLAYER_H
#define RESOURCEMOD_PLAYER_H

#include "../cs2/cbaseplayercontroller.h"
#include "../cs2/csplayerpawn.h"
#include "../cs2/ccsplayercontroller.h"
#include "../ResourceMod.h"
#include <engine/igameeventsystem.h>
#include <igameevents.h>
#include <metacall/metacall.h>
#include <vector>

class Player {
public:
    Player(CCSPlayerController *c) {
        this->controller = c;
    };

    CCSPlayerController *controller;

    static void *GetHP(size_t argc, void *args[], void *data);

    static void *SetHP(size_t argc, void *args[], void *data);

    static void *GetMoney(size_t argc, void *args[], void *data);

    static void *SetMoney(size_t argc, void *args[], void *data);

    static void *GetDamage(size_t argc, void *args[], void *data);

    static void *GetKills(size_t argc, void *args[], void *data);

    static void *GetAssists(size_t argc, void *args[], void *data);

    static void *GetDeaths(size_t argc, void *args[], void *data);

    static void *SetDamage(size_t argc, void *args[], void *data);

    static void *SetKills(size_t argc, void *args[], void *data);

    static void *SetAssists(size_t argc, void *args[], void *data);

    static void *SetDeaths(size_t argc, void *args[], void *data);

    static void *GetArmor(size_t argc, void *args[], void *data);

    static void *SetArmor(size_t argc, void *args[], void *data);

    static void *GetName(size_t argc, void *args[], void *data);

    static void *GetSteamID(size_t argc, void *args[], void *data);

    static void *GetIsAlive(size_t argc, void *args[], void *data);

    static void *Slap(size_t argc, void *args[], void *data);

    static void *Slay(size_t argc, void *args[], void *data);

    static void *Respawn(size_t argc, void *args[], void *data);

    static void *PrintGameMessage(size_t argc, void *args[], void *data);

    static void *Play(size_t argc, void *args[], void *data);

    static void *Kick(size_t argc, void *args[], void *data);

    static void *GetTeam(size_t argc, void *args[], void *data);

    static void *ChangeTeam(size_t argc, void *args[], void *data);

    static void *GetIsConnected(size_t argc, void *args[], void *data);

    static void *GetIsConnecting(size_t argc, void *args[], void *data);

    static void *GetIsDisconnected(size_t argc, void *args[], void *data);

    static void ClientPrintAll(int hud_dest, const char *msg, ...);

    static void ClientPrint(CCSPlayerController *player, int dest, const char *text, ...);

    static void *PrintAll(size_t argc, void *args[], void *data);

    static void *Print(size_t argc, void *args[], void *data);

    static void *GetIsDisconnecting(size_t argc, void *args[], void *data);

    static void *GetIsReserved(size_t argc, void *args[], void *data);

    static void *GetIsReconnecting(size_t argc, void *args[], void *data);
};

#endif //RESOURCEMOD_PLAYER_H
