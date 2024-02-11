//
// Created by Twelvee on 23.01.2024.
//

#include "_resourcemod_loader.h"
#include "entitysystem.h"
#include <sourcehook/sourcehook.h>
#include <ISmmPlugin.h>
#include <string>
#include <filesystem>

_resourcemod_loader loader;

PLUGIN_EXPOSE(_resourcemod_loader, loader);

bool _resourcemod_loader::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late) {
    PLUGIN_SAVEVARS();
    g_SMAPI->AddListener(this, this);
    this->LoadDependencies();
    ConVar_Register(FCVAR_RELEASE | FCVAR_GAMEDLL);

    return true;
}

void _resourcemod_loader::LoadDependencies() {
    std::string path = "../../csgo/addons/resourcemod/node_modules/resourcemod/bin/metacall/metacall.dll"; //todo: linux
    const auto metacall = LoadLibraryA(path.c_str());
    if (!metacall) {
        printf("\n\n%lu\n\n", GetLastError());
        return;
    }
}

CGameEntitySystem *GameEntitySystem()
{
    return nullptr;
}

bool _resourcemod_loader::Unload(char *error, size_t maxlen) {
    ConVar_Unregister();
    return true;
}

const char *_resourcemod_loader::GetLicense() {
    return "GPL-3.0";
}

const char *_resourcemod_loader::GetVersion() {
    return "1.0.0";
}

const char *_resourcemod_loader::GetDate() {
    return __DATE__;
}

const char *_resourcemod_loader::GetLogTag() {
    return "ResourceMod Loader";
}

const char *_resourcemod_loader::GetAuthor() {
    return "Twelvee";
}

const char *_resourcemod_loader::GetDescription() {
    return "Load metacall.";
}

const char *_resourcemod_loader::GetName() {
    return "ResourceMod Loader";
}

const char *_resourcemod_loader::GetURL() {
    return "https://resourcemod.net/";
}