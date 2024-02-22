//
// Created by Twelvee on 08.10.2023.
//

#include "ResourceMod.h"
#include <string>
#include "iserver.h"
#include "../engine/Engine.h"
#include "hooks/EventManager.h"
#include "../logger/logger.h"
#include "cs2/cschemasystem.h"
#include <engine/igameeventsystem.h>
#include "cs2/cgameresourceserviceserver.h"
#include <sourcehook/sourcehook.h>
#include "cs2/Memory.h"
#include "GameDLL.h"
#include "GameSystem.h"
#include <icvar.h>
#ifdef WIN_32
#include <direct.h>
#endif
#include <tier1/utlstring.h>
#include <tier1/utlvector.h>

ResourceMod resourcemod;
ResourceMod *g_ResourceMod;

PLUGIN_EXPOSE(ResourceMod, resourcemod);

#define MAX_PLAYERS 64

IVEngineServer2 *g_SourceEngine;
IGameEventSystem *g_SourceEvents;
EventManager *g_EventManager;
CSchemaSystem *g_SchemaSystem;
CEntitySystem *g_pEntitySystem;

CGameResourceService *g_GameResourceService;
Memory *g_Memory;

class CGameEventListener;

CUtlVector<CGameEventListener *> g_vecEventListeners;

class GameSessionConfiguration_t {
};

SH_DECL_HOOK3_void(INetworkServerService, StartupServer, SH_NOATTRIB, 0, const GameSessionConfiguration_t&,
                   ISource2WorldSession*, const char*);

SH_DECL_HOOK3_void(IServerGameDLL, GameFrame, SH_NOATTRIB, 0, bool, bool, bool);

bool ResourceMod::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late) {
    g_ResourceMod = this;
    PLUGIN_SAVEVARS();

    GET_V_IFACE_ANY(GetEngineFactory, g_SourceEngine, IVEngineServer2, INTERFACEVERSION_VENGINESERVER);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pCVar, ICvar, CVAR_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetServerFactory, g_pSource2Server, ISource2Server, INTERFACEVERSION_SERVERGAMEDLL);
    GET_V_IFACE_CURRENT(GetServerFactory, g_pSource2ServerConfig, ISource2ServerConfig, INTERFACEVERSION_SERVERCONFIG);
    GET_V_IFACE_ANY(GetServerFactory, g_pSource2GameClients, IServerGameClients, SOURCE2GAMECLIENTS_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetEngineFactory, g_SourceEvents, IGameEventSystem, GAMEEVENTSYSTEM_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_SchemaSystem, CSchemaSystem, SCHEMASYSTEM_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetEngineFactory, g_pNetworkServerService, INetworkServerService,
                    NETWORKSERVERSERVICE_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_GameResourceService, CGameResourceService,
                        GAMERESOURCESERVICESERVER_INTERFACE_VERSION);

    g_SMAPI->AddListener(this, this);

    SH_ADD_HOOK_MEMFUNC(INetworkServerService, StartupServer, g_pNetworkServerService, this,
                        &ResourceMod::Hook_StartupServer, true);
    SH_ADD_HOOK(IServerGameDLL, GameFrame, g_pSource2Server, SH_MEMBER(this, &ResourceMod::Hook_GameFrame), true);

    Engine *rmod = new Engine();
    rmod->Init();

    g_Memory = new Memory();
    g_Memory->LoadOffsets(rmod->gameDataPath);
    g_Memory->LoadSignatures(rmod->gameDataPath);
    g_Memory->MakeSignaturesCallable();

    g_gameEventManager = (IGameEventManager2 *) (
            CALL_VIRTUAL(uintptr_t, g_Memory->offsets["GameEventManager"], g_pSource2Server) - 8);

    if (!InitGameSystems(rmod->precacheList)) {
        logger::log("Failed to init game systems");
        return false;
    }

    g_EventManager = new EventManager();
    g_EventManager->StartHooks();

    ConVar_Register(FCVAR_RELEASE | FCVAR_GAMEDLL);

    return true;
}

void ResourceMod::Hook_StartupServer(const GameSessionConfiguration_t &config, ISource2WorldSession *, const char *) {
    g_pEntitySystem = g_GameResourceService->GetGameEntitySystem();
}

CGameEntitySystem *GameEntitySystem() {
    return *reinterpret_cast<CGameEntitySystem **>((uintptr_t) (g_GameResourceService) +
                                                   g_Memory->offsets["GameEntitySystem"]);
}

void ResourceMod::OnLevelInit(char const *pMapName, char const *pMapEntities, char const *pOldLevel,
                              char const *pLandmarkName, bool loadGame, bool background) {
    auto e = new map_loaded(pMapName, pOldLevel);
    e->Emit();
    delete e;
}

bool ResourceMod::Unload(char *error, size_t maxlen) {
    // remove hooks
    SH_REMOVE_HOOK_MEMFUNC(INetworkServerService, StartupServer, g_pNetworkServerService, this,
                           &ResourceMod::Hook_StartupServer, true);
    SH_REMOVE_HOOK(IServerGameDLL, GameFrame, g_pSource2Server, SH_MEMBER(this, &ResourceMod::Hook_GameFrame), true);
    g_EventManager->StopHooks();
    g_Engine->Shutdown();
    ConVar_Unregister();
    return true;
}

void ResourceMod::Hook_GameFrame(bool simulating, bool bFirstTick, bool bLastTick) {
    this->RMFrame();
}

void ResourceMod::RMFrame() {
    for (uint16_t i = 0; i < MAX_PLAYERS; i++) {
        CCSPlayerController *player = CCSPlayerController::FromSlot(i);
        if (!player)
            continue;
        if (!player->IsConnected())
            continue;

        CBasePlayerPawn *pawn = player->GetPawn();
        if (!pawn)
            continue;

        CPlayer_MovementServices *movementServices = pawn->m_pMovementServices();
        if (!movementServices)
            continue;

        if (!g_Engine) {
            continue;
        }
        if (g_Engine->gameMessages.count(i) > 0) {
            player->PrintGameMessage();
        }
    }

    while (!m_nextFrame.empty()) {
        m_nextFrame.front()();
        m_nextFrame.pop_front();
    }
}

void ResourceMod::NextFrame(std::function<void()> fn) {
    m_nextFrame.push_back(fn);
}

const char *ResourceMod::GetLicense() {
    return "GPL-3.0";
}

const char *ResourceMod::GetVersion() {
    return "1.0.33-beta";
}

const char *ResourceMod::GetDate() {
    return __DATE__;
}

const char *ResourceMod::GetLogTag() {
    return "ResourceMod";
}

const char *ResourceMod::GetAuthor() {
    return "Twelvee";
}

const char *ResourceMod::GetDescription() {
    return "Ecosystem for CS2 plugins.";
}

const char *ResourceMod::GetName() {
    return "ResourceMod";
}

const char *ResourceMod::GetURL() {
    return "https://rmod.dev/";
}