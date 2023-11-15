//
// Created by Twelvee on 13.09.2023.
//

#include "Engine.h"
#include "../logger/logger.h"
#include "../game/hooks/EventManager.h"
#include "v8/Module.h"
#include <direct.h>

Engine *g_Engine;

void Engine::Init() {
    // Get current exe path
    char cwd[MAX_PATH];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        this->rootPath = cwd;
    }
    this->resourcemodFolder = "../../csgo/addons/resourcemod";
    this->pluginsFolder = this->resourcemodFolder.c_str();
    this->pluginsFolder.append("/plugins");

    this->metaPath = this->resourcemodFolder.c_str();
    this->metaPath.append("/").append(RESOURCEMOD_META);

    g_Engine = this;
    this->InitV8();
}

void Engine::InitV8() {
    // todo: i18n support (more here https://v8.dev/docs/embed)
    //v8::V8::InitializeICUDefaultLocation(mainFile);
    //v8::V8::InitializeExternalStartupData(mainFile);
    this->platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(this->platform.get());
    v8::V8::Initialize();

    this->create_params.array_buffer_allocator =
            v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    this->isolate = v8::Isolate::New(this->create_params);
    this->isolate->SetHostImportModuleDynamicallyCallback(Module::CallDynamic);
    this->isolate->SetHostInitializeImportMetaObjectCallback(Module::CallMeta);
    this->LoadMeta();
}

extern EventManager *g_EventManager;

void Engine::FireEvent(std::string name, std::string json) {
    for (auto item: g_Engine->plugins) {
        item->FireEvent(name, json);
    }
}

void Engine::FireGameEvent(IGameEvent *event) {
    for (auto item: g_Engine->plugins) {
        item->FireGameEvent(event);
    }
}

void Engine::Shutdown() {
    // todo: properly shutdown v8
}
