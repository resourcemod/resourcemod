//
// Created by Twelvee on 06.09.2023.
//

#ifndef RESOURCEMOD_EVENTMANAGER_H
#define RESOURCEMOD_EVENTMANAGER_H

#include <tier1/functors.h>
#include <stdint.h>
#include <sourcehook/sourcehook.h>
#include <ISmmPlugin.h>
#include <igameevents.h>
#include <unordered_map>
#include "LegacyEvents.h"
#include "engine/igameeventsystem.h"
#include "../../protobuf/generated/network_connection.pb.h"
#include <map>

typedef std::map<std::string, RMEvent *(*)(IGameEvent *)> events_map_t;

class EventManager {
public:
    void StartHooks();

    void StopHooks();

    events_map_t events;

    void FillLegacyEventsMap();

    // smm hooks
    void InitSMMHooks(IGameEventSystem *, IVEngineServer2 *, ISource2GameClients *, ICvar *, ISource2ServerConfig *);

    void OnServerCommand(const char *command);

    void OnPostEventAbstract(CSplitScreenSlot nSlot, bool bLocalOnly, int nClientCount, const uint64 *clients,
                             INetworkSerializable *pEvent, const void *pData, unsigned long nSize,
                             NetChannelBufType_t bufType);

    void OnClientConnected_hk(CPlayerSlot slot, const char *pszName, uint64 xuid, const char *pszNetworkID,
                              const char *pszAddress, bool bFakePlayer);

    void OnClientDisconnect_hk(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char *pszName, uint64 xuid,
                               const char *pszNetworkID);

    bool OnClientConnect_hk(CPlayerSlot slot, const char *pszName, uint64 xuid, const char *pszNetworkID, bool unk1,
                            CBufferString *pRejectReason);

    void OnClientPutInServer_hk(CPlayerSlot slot, char const *pszName, int type, uint64 xuid);

    void OnClientCommand_hk(CPlayerSlot slot, const CCommand &args);

    void StartupServer_hk(const GameSessionConfiguration_t &config, ISource2WorldSession *, const char *);

    void DispatchConCommand_hk(ConCommandHandle cmdHandle, const CCommandContext& ctx, const CCommand& args);

    bool OnEventFired(IGameEvent *event, bool bDontBroadcast);
};


#endif //RESOURCEMOD_EVENTMANAGER_H
