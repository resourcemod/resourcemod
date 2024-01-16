//
// Created by Twelvee on 13.09.2023.
//

#include "Engine.h"
#include "../logger/logger.h"
#include "../game/hooks/EventManager.h"
#include "v8/Module.h"
#include <direct.h>
#include "EventLoop.h"

Engine *g_Engine;
EventLoop *g_EventLoop;
void Engine::Init() {
    // Get current exe path
    char cwd[MAX_PATH];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        this->rootPath = cwd;
    }
    this->resourcemodFolder = "../../csgo/addons/resourcemod";

    this->pluginsFolder = this->resourcemodFolder.c_str();
    this->pluginsFolder.append("/plugins");

    //this->extensionsFolder = this->resourcemodFolder.c_str();
    //this->extensionsFolder.append("/extensions");

    this->metaPath = this->resourcemodFolder.c_str();
    this->metaPath.append("/").append(RESOURCEMOD_META);

    this->pluginsPath = this->resourcemodFolder.c_str();
    this->pluginsPath.append("/").append(RESOURCEMOD_PLUGINS);

    //this->extensionsPath = this->resourcemodFolder.c_str();
    //this->extensionsPath.append("/").append(RESOURCEMOD_EXTENSIONS);

    this->gameDataPath = this->resourcemodFolder.c_str();
    this->gameDataPath.append("/").append("gamedata").append("/").append("rmod.cs2.json");

    g_Engine = this;
    g_EventLoop = new EventLoop();
    g_EventLoop->Run();
    this->InitV8();
}

void Engine::InitV8() {
    logger::log(logger::format("V8 Version: %s", v8::V8::GetVersion()));
    // todo: i18n support (more here https://v8.dev/docs/embed)
    //v8::V8::InitializeICUDefaultLocation(mainFile);
    //v8::V8::InitializeExternalStartupData(mainFile);
    this->platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(this->platform.get());
    v8::V8::Initialize();

    this->create_params.array_buffer_allocator =
            v8::ArrayBuffer::Allocator::NewDefaultAllocator();

    this->isolate = v8::Isolate::New(this->create_params);
    this->isolate->SetHostImportModuleDynamicallyCallback(V8Module::CallDynamic);
    this->isolate->SetHostInitializeImportMetaObjectCallback(V8Module::CallMeta);
    this->isolate->Enter();
    this->LoadMeta();
    this->LoadPlugins();
}

extern EventManager *g_EventManager;

void Engine::FireEvent(std::string name, std::string json) {
    for (auto item: g_Engine->plugins) {
        item->FireEvent(name, json);
    }
}

bool Engine::FireGameEvent(IGameEvent *event) {
    int prevent = 0;
    for (auto item: g_Engine->plugins) {
        if (item->FireGameEvent(event)) {
            prevent++;
        }
    }
    if (prevent > 0) {
        return true;
    }
    return false;
}

void Engine::Shutdown() {
    // todo: properly shutdown v8
}
