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
#include <direct.h>

ResourceMod g_ResourceMod;

PLUGIN_EXPOSE(ResourceMod, g_ResourceMod);

IVEngineServer2* g_SourceEngine;
IGameEventSystem* g_SourceEvents;
EventManager *g_EventManager;
CSchemaSystem* g_SchemaSystem;
CEntitySystem* g_pEntitySystem;
CGameResourceService* g_GameResourceService;
Memory *g_Memory;

class GameSessionConfiguration_t { };

SH_DECL_HOOK3_void(INetworkServerService, StartupServer, SH_NOATTRIB, 0, const GameSessionConfiguration_t&, ISource2WorldSession*, const char*);

bool ResourceMod::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late) {
    PLUGIN_SAVEVARS();
    if (late) {
        V_strncpy(error, "Late load is not supported", maxlen);
        ConColorMsg(Color(250, 0, 0, 255), "[%s] %s\n", GetLogTag(), error);
        return false;
    }

    GET_V_IFACE_ANY(GetEngineFactory, g_SourceEngine, IVEngineServer2, INTERFACEVERSION_VENGINESERVER);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pCVar, ICvar, CVAR_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetServerFactory, g_pSource2Server, ISource2Server, INTERFACEVERSION_SERVERGAMEDLL);
    GET_V_IFACE_CURRENT(GetServerFactory, g_pSource2ServerConfig, ISource2ServerConfig, INTERFACEVERSION_SERVERCONFIG);
    GET_V_IFACE_CURRENT(GetServerFactory, g_pSource2GameClients, ISource2GameClients, INTERFACEVERSION_SERVERCONFIG);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_SourceEvents, IGameEventSystem, GAMEEVENTSYSTEM_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_SchemaSystem, CSchemaSystem, SCHEMASYSTEM_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetEngineFactory, g_pNetworkServerService, INetworkServerService, NETWORKSERVERSERVICE_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_GameResourceService, CGameResourceService, GAMERESOURCESERVICESERVER_INTERFACE_VERSION);

    SH_ADD_HOOK_MEMFUNC(INetworkServerService, StartupServer, g_pNetworkServerService, this, &ResourceMod::Hook_StartupServer, true);

    CSchemaSystemTypeScope *scope = g_SchemaSystem->FindTypeScopeForModule("server.dll");
    logger::log(logger::format("%p", scope));
    auto a = scope->FindDeclaredClass("CBasePlayerController");

    do
    {
        SchemaClassFieldData_t *pFields = a->GetFields();
        short fieldsSize = a->GetFieldsSize();
        for (int i = 0; i < fieldsSize; ++i)
        {
            SchemaClassFieldData_t &field = pFields[i];
            logger::log(logger::format("%s: %d", field.m_name, field.m_offset));
            if (V_strcmp(field.m_name, "__m_pChainEntity") == 0)
            {
                return field.m_offset;
            }
        }
    } while ((a = a->GetParent()) != nullptr);

    Engine *rmod = new Engine();
    rmod->Init();
    g_Memory = new Memory();
    g_Memory->LoadOffsets(rmod->gameDataPath);

    g_EventManager = new EventManager();
    g_EventManager->StartHooks();

    ConVar_Register(FCVAR_RELEASE | FCVAR_GAMEDLL);

    return true;
}

void ResourceMod::Hook_StartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession*, const char*) {
    g_pEntitySystem = g_GameResourceService->GetGameEntitySystem();
}

CGameEntitySystem *GameEntitySystem()
{
    return *reinterpret_cast<CGameEntitySystem **>((uintptr_t)(g_GameResourceService) + g_Memory->offsets["GameEntitySystem"]);
}

bool ResourceMod::Unload(char *error, size_t maxlen) {
    // remove hooks
    SH_REMOVE_HOOK_MEMFUNC(INetworkServerService, StartupServer, g_pNetworkServerService, this, &ResourceMod::Hook_StartupServer, true);
    g_EventManager->StopHooks();
    g_Engine->Shutdown();
    ConVar_Unregister();
    return true;
}

const char *ResourceMod::GetLicense() {
    return "GPL-3.0";
}

const char *ResourceMod::GetVersion() {
    return "1.0.0";
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
    return "https://resourcemod.net/";
}