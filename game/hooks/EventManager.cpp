//
// Created by Twelvee on 06.09.2023.
//

#include "EventManager.h"
#include "../../logger/logger.h"
#include "../GameDLL.h"
#include "Structs.h"
#include <tier1/KeyValues.h>
#include <tier1/utlstring.h>
#include <igameevents.h>
#include <iserver.h>
#include "eiface.h"
#include "../GameSystem.h"
#include "../../protobuf/generated/cs_gameevents.pb.h"
#include "../../protobuf/generated/te.pb.h"

extern PluginId g_PLID;
extern SourceHook::ISourceHook *g_SHPtr;

extern IVEngineServer2 *g_SourceEngine;
extern IGameEventSystem *g_SourceEvents;
extern ISource2Server *g_pSource2Server;
extern Memory *g_Memory;
IGameEventManager2 *g_gameEventManager;

class GameSessionConfiguration_t {
};

SH_DECL_HOOK5_void(IServerGameClients, ClientDisconnect, SH_NOATTRIB, 0, CPlayerSlot, ENetworkDisconnectionReason,
                   const char *, uint64, const char *);

SH_DECL_HOOK4_void(IServerGameClients, ClientPutInServer, SH_NOATTRIB, 0, CPlayerSlot, char const *, int, uint64);

SH_DECL_HOOK6_void(IServerGameClients, OnClientConnected, SH_NOATTRIB, 0, CPlayerSlot, const char*, uint64,
                   const char *, const char *, bool);

SH_DECL_HOOK6(IServerGameClients, ClientConnect, SH_NOATTRIB, 0, bool, CPlayerSlot, const char*, uint64, const char *,
              bool, CBufferString *);

SH_DECL_HOOK8_void(IGameEventSystem, PostEventAbstract, SH_NOATTRIB, 0, CSplitScreenSlot, bool, int, const uint64*,
                   INetworkSerializable*, const void*, unsigned long, NetChannelBufType_t)

SH_DECL_HOOK2_void(IServerGameClients, ClientCommand, SH_NOATTRIB, 0, CPlayerSlot, const CCommand &);

SH_DECL_HOOK3_void(ICvar, DispatchConCommand, SH_NOATTRIB, 0, ConCommandHandle, const CCommandContext&,
                   const CCommand&);

SH_DECL_HOOK2(IGameEventManager2, FireEvent, SH_NOATTRIB, 0, bool, IGameEvent *, bool);

void EventManager::StartHooks() {
    // make event calls with o1 overhead
    this->FillLegacyEventsMap();

    SH_ADD_HOOK(IGameEventManager2, FireEvent, g_gameEventManager,
                SH_MEMBER(this, &EventManager::OnEventFired), false);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientDisconnect, g_pSource2GameClients, this,
                        &EventManager::OnClientDisconnect_hk, true);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientPutInServer, g_pSource2GameClients, this,
                        &EventManager::OnClientPutInServer_hk, true);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, OnClientConnected, g_pSource2GameClients, this,
                        &EventManager::OnClientConnected_hk, false);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientConnect, g_pSource2GameClients, this,
                        &EventManager::OnClientConnect_hk, false);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientCommand, g_pSource2GameClients, this,
                        &EventManager::OnClientCommand_hk, false);
    SH_ADD_HOOK_MEMFUNC(IGameEventSystem, PostEventAbstract, g_SourceEvents, this, &EventManager::OnPostEventAbstract,
                        false);
    SH_ADD_HOOK_MEMFUNC(ICvar, DispatchConCommand, g_pCVar, this, &EventManager::DispatchConCommand_hk, false);
}

void EventManager::StopHooks() {
    SH_REMOVE_HOOK(IGameEventManager2, FireEvent, g_gameEventManager,
                   SH_MEMBER(this, &EventManager::OnEventFired), true);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientDisconnect, g_pSource2GameClients, this,
                           &EventManager::OnClientDisconnect_hk, true);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientPutInServer, g_pSource2GameClients, this,
                           &EventManager::OnClientPutInServer_hk, true);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, OnClientConnected, g_pSource2GameClients, this,
                           &EventManager::OnClientConnected_hk, false);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientConnect, g_pSource2GameClients, this,
                           &EventManager::OnClientConnect_hk, false);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientCommand, g_pSource2GameClients, this,
                           &EventManager::OnClientCommand_hk, false);
    SH_REMOVE_HOOK_MEMFUNC(IGameEventSystem, PostEventAbstract, g_SourceEvents, this,
                           &EventManager::OnPostEventAbstract,
                           false);
    SH_REMOVE_HOOK_MEMFUNC(ICvar, DispatchConCommand, g_pCVar, this, &EventManager::DispatchConCommand_hk, false);
}

extern Engine *g_Engine;

bool EventManager::OnEventFired(IGameEvent *event, bool bDontBroadcast = false) {
    if (!event) {
        RETURN_META_VALUE(MRES_IGNORED, false);
    }

    if (this->events.count(event->GetName()) > 0) {
        auto e = this->events[event->GetName()](event);
        bool prevent = e->Emit();
        if (prevent) {
            delete e;
            RETURN_META_VALUE_NEWPARAMS(MRES_IGNORED, true, &IGameEventManager2::FireEvent, (event, false));
        }
        delete e;
    }

    return false;
}

void EventManager::OnPostEventAbstract(CSplitScreenSlot nSlot, bool bLocalOnly, int nClientCount, const uint64 *clients,
                                       INetworkSerializable *pEvent, const void *pData, unsigned long nSize,
                                       NetChannelBufType_t bufType) {
    static void (IGameEventSystem::*PostEventAbstract)(CSplitScreenSlot, bool, int, const uint64 *,
                                                       INetworkSerializable *, const void *, unsigned long, NetChannelBufType_t) = &IGameEventSystem::PostEventAbstract;

    NetMessageInfo_t *info = pEvent->GetNetMessageInfo();
    if (info->m_MessageId == GE_FireBulletsId) {
        // ge block
    }
}

void EventManager::OnClientDisconnect_hk(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char *pszName,
                                         uint64 xuid, const char *pszNetworkID) {
    auto e = new client_disconnected(slot.Get());
    e->Emit();
    delete e;
}

void EventManager::OnClientConnected_hk(CPlayerSlot slot, const char *pszName, uint64 xuid, const char *pszNetworkID,
                                        const char *pszAddress, bool bFakePlayer) {
    auto e = new client_connected(pszName, xuid, pszNetworkID, pszAddress, bFakePlayer);
    logger::log(logger::format("%s slot: %d", pszName, slot));
    if (e->Emit()) {
        RETURN_META(MRES_SUPERCEDE);
    }
    delete e;
}

bool EventManager::OnClientConnect_hk(CPlayerSlot slot, const char *pszName, uint64 xuid, const char *pszNetworkID,
                                      bool unk1, CBufferString *pRejectReason) {
    auto e = new client_connect(pszName, xuid, pszNetworkID);
    if (e->Emit()) {
        RETURN_META_VALUE(MRES_SUPERCEDE, false);
    }
    delete e;
}

void EventManager::OnClientCommand_hk(CPlayerSlot slot, const CCommand &args) {

}

void EventManager::DispatchConCommand_hk(ConCommandHandle cmdHandle, const CCommandContext &ctx, const CCommand &args) {
    if (!g_pEntitySystem)
        return;

    auto iCommandPlayerSlot = ctx.GetPlayerSlot();

    bool bSay = !V_strcmp(args.Arg(0), "say");
    bool bTeamSay = !V_strcmp(args.Arg(0), "say_team");
    if (iCommandPlayerSlot != -1 && (bSay || bTeamSay)) {
        auto pController = CCSPlayerController::FromSlot(iCommandPlayerSlot);
        if (pController) {
            IGameEvent *pEvent = g_gameEventManager->CreateEvent("player_chat");

            if (pEvent) {
                pEvent->SetBool("teamonly", bTeamSay);
                pEvent->SetInt("userid", pController->GetPlayerSlot());
                pEvent->SetString("text", args[1]);

                g_gameEventManager->FireEvent(pEvent, true);
            }

            auto e = new player_chat(pController->GetPlayerSlot(), bTeamSay, args[1]);
            if (e->Emit()) {
                RETURN_META(MRES_SUPERCEDE);
            }
        }
    }
}

void EventManager::OnClientPutInServer_hk(CPlayerSlot slot, char const *pszName, int type, uint64 xuid) {
    auto e = new client_put_in_server(slot.Get());
    if (e->Emit()) {
        RETURN_META(MRES_SUPERCEDE);
    }
    delete e;
}

void EventManager::StartupServer_hk(const GameSessionConfiguration_t &config, ISource2WorldSession *, const char *) {
    logger::log("Server startup hook");
}