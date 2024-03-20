//
// Created by Twelvee on 20.02.2024.
//
#include "Player.h"
#include "../../protobuf/generated/network_connection.pb.h"
#include "../cs2/cbaseplayercontroller.h"
#include "../cs2/csplayerpawn.h"
#include "../cs2/ccsplayercontroller.h"
#include "../ResourceMod.h"
#include <engine/igameeventsystem.h>
#include <igameevents.h>
#include <vector>

extern ResourceMod *g_ResourceMod;
extern IVEngineServer2 *g_SourceEngine;
extern Engine *g_Engine;

int GetHP(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    return c->GetPawn()->GetHP();
}

bool SetHP(int slot, int hp) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    c->GetPawn()->SetHP(hp);
    return true;
}

int GetMoney(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    return c->m_pInGameMoneyServices->m_iAccount();
}

bool SetMoney(int slot, int money) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    c->m_pInGameMoneyServices->m_iAccount = money;
    return true;
}

int GetDamage(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    CSMatchStats_t *matchStats = &c->m_pActionTrackingServices->m_matchStats();
    return matchStats->m_iDamage.Get();
}

int GetKills(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    CSMatchStats_t *matchStats = &c->m_pActionTrackingServices->m_matchStats();
    return matchStats->m_iKills.Get();
}

int GetAssists(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    CSMatchStats_t *matchStats = &c->m_pActionTrackingServices->m_matchStats();
    return matchStats->m_iAssists.Get();
}

int GetDeaths(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    CSMatchStats_t *matchStats = &c->m_pActionTrackingServices->m_matchStats();
    return matchStats->m_iDeaths.Get();
}

bool SetDamage(int slot, int damage) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    c->m_pActionTrackingServices->m_matchStats().m_iDamage = damage;
    return true;
}

bool SetKills(int slot, int kills) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    c->m_pActionTrackingServices->m_matchStats().m_iKills = kills;
    return true;
}

bool SetAssists(int slot, int assists) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    c->m_pActionTrackingServices->m_matchStats().m_iAssists = assists;
    return true;
}

bool SetDeaths(int slot, int deaths) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    c->m_pActionTrackingServices->m_matchStats().m_iDeaths = deaths;
    return true;
}

int GetArmor(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    CCSPlayerPawnBase *pawn = (CCSPlayerPawnBase *) c->m_hPlayerPawn().Get();
    if (!pawn) {
        return 0;
    }
    return pawn->GetArmor();
}

bool SetArmor(int slot, int armor) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    CCSPlayerPawnBase *pawn = (CCSPlayerPawnBase *) c->m_hPlayerPawn().Get();
    if (!pawn) {
        return false;
    }
    pawn->SetArmor(armor);
    return true;
}

const char *GetName(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    return c->GetPlayerName();
}

long GetSteamID(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    return c->m_steamID;
}

bool GetIsAlive(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    return c->IsAlive();
}

bool Slap(int slot, int damage) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    c->GetPawn()->Slap(damage);
    return true;
}

bool Slay(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    c->GetPawn()->CommitSuicide(true, true);
    return true;
}

bool Respawn(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    return c->Respawn();
}

bool PrintGameMessage(int slot, char *message, int duration) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    if (strlen(message) == 0) {
        return false;
    }

    if (duration == 0) {
        return false;
    }

    std::pair<uint64_t, std::string> pair = std::make_pair(Engine::Now() + (duration * 1000), message);
    g_Engine->gameMessages[slot] = pair;

    return true;
}

bool Play(int slot, char *sound) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);

    std::string soundPath = "play ";
    soundPath.append(sound);
    if (c->GetPawn() != nullptr && c->m_steamID > 0)
        g_SourceEngine->ClientCommand(c->GetPlayerSlot(), soundPath.c_str());
    return true;
}

bool Kick(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);

    g_SourceEngine->DisconnectClient(c->GetPlayerSlot(), NETWORK_DISCONNECT_KICKED); //disconnect by server
    return true;
}

int GetTeam(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    return c->m_iTeamNum.Get();
}

int ChangeTeam(int slot, int team, bool kill) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    if (team < 0 || team > 3) {
        return false;
    }
    if (kill) {
        c->ChangeTeam(team);
    } else {
        c->SwitchTeam(team);
    }

    return true;
}

bool GetIsConnected(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    return c->IsConnected();
}

bool GetIsConnecting(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    return c->IsConnecting();
}

bool GetIsDisconnected(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    return c->IsDisconnected();
}

bool GetIsDisconnecting(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    return c->IsDisconnecting();
}

bool GetIsReserved(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    return c->IsReserved();
}

bool GetIsReconnecting(int slot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    return c->IsReconnecting();
}

void ClientPrintAll(int hud_dest, const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    char buf[256];
    V_vsnprintf(buf, sizeof(buf), msg, args);
    va_end(args);

    return SignatureCall::UTIL_ClientPrintAll(hud_dest, buf, nullptr, nullptr, nullptr, nullptr);
}

void ClientPrint(CCSPlayerController *player, int dest, const char *text, ...) {
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

bool PrintAll(int dest, const char* msg) {
    Player::ClientPrintAll(dest, msg);
    return true;
}

bool Print(int slot, int dest, const char* msg) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    Player::ClientPrint(c, dest, msg);
    return true;
}