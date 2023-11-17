//
// Created by Twelvee on 06.09.2023.
//

#include "EventManager.h"
#include "../../logger/logger.h"
#include "../GameDLL.h"
#include "../../protoc_generated/usermessages.pb.h"
#include "../../engine/Engine.h"
#include "Structs.h"
#include "../Virtual.h"
#include "../cs2/Memory.h"
#include <tier1/KeyValues.h>
#include <igameevents.h>

extern PluginId g_PLID;
extern SourceHook::ISourceHook *g_SHPtr;

extern IVEngineServer2 *g_SourceEngine;
extern IGameEventSystem *g_SourceEvents;
extern ISource2Server *g_pSource2Server;
extern Memory *g_Memory;
IGameEventManager2 *g_gameEventManager;

SH_DECL_HOOK8_void(IGameEventSystem, PostEventAbstract, SH_NOATTRIB, 0, CSplitScreenSlot, bool, int, const uint64*,
                   INetworkSerializable*, const void*, unsigned long, NetChannelBufType_t)

SH_DECL_HOOK6_void(ISource2GameClients, OnClientConnected, SH_NOATTRIB, 1, CPlayerSlot, const char *, uint64,
                   const char *, const char *, bool);

SH_DECL_HOOK2(IGameEventManager2, FireEvent, SH_NOATTRIB, 0, bool, IGameEvent *, bool);

void EventManager::StartHooks() {
    // make event calls with O1
    this->FillLegacyEventsMap();
    g_gameEventManager = (IGameEventManager2 *) (CALL_VIRTUAL(uintptr_t, g_Memory->offsets["GameEventManager"], g_pSource2Server) - 8);
    SH_ADD_HOOK(IGameEventManager2, FireEvent, g_gameEventManager,
            SH_MEMBER(this, &EventManager::OnEventFired), false);
    SH_ADD_HOOK(IGameEventSystem, PostEventAbstract, g_SourceEvents,
                SH_MEMBER(this, &EventManager::OnPostEventAbstract), false);
    SH_ADD_HOOK(ISource2GameClients, OnClientConnected, g_pSource2GameClients,
                SH_MEMBER(this, &EventManager::OnClientConnected_hk), false);
}

void EventManager::StopHooks() {
    SH_REMOVE_HOOK(IGameEventManager2, FireEvent, g_gameEventManager,
                SH_MEMBER(this, &EventManager::OnEventFired), false);
    SH_REMOVE_HOOK(IGameEventSystem, PostEventAbstract, g_SourceEvents,
                   SH_MEMBER(this, &EventManager::OnPostEventAbstract), false);
    SH_REMOVE_HOOK(ISource2GameClients, OnClientConnected, g_pSource2GameClients,
                   SH_MEMBER(this, &EventManager::OnClientConnected_hk), false);
}

extern Engine* g_Engine;

bool EventManager::OnEventFired(IGameEvent *event, bool bDontBroadcast = false) {
    if (!event)
    {
        RETURN_META_VALUE(MRES_IGNORED, false);
    }
    int prevent = 0;
    if (this->events.count(event->GetName()) > 0) {
        if (g_Engine->FireGameEvent(event)) {
            prevent++;
        }
    }
    if (prevent > 0) {
        RETURN_META_VALUE(MRES_SUPERCEDE, false);
    }
    return false;
}

void EventManager::OnPostEventAbstract(CSplitScreenSlot nSlot, bool bLocalOnly, int nClientCount, const uint64 *clients,
                                       INetworkSerializable *pEvent, const void *pData, unsigned long nSize,
                                       NetChannelBufType_t bufType) {
    NetMessageInfo_t *info = pEvent->GetNetMessageInfo();

    //Engine::FireEvent();
}

void EventManager::OnClientConnected_hk(CPlayerSlot slot, const char *pszName, uint64 xuid, const char *pszNetworkID,
                                        const char *pszAddress, bool bFakePlayer) {
    g_Engine->FireEvent("client_connected", R"({"message":"hello world"})");
    RETURN_META(MRES_IGNORED);
}