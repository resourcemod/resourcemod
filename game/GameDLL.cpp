//
// Created by Twelvee on 06.09.2023.
//

#include "GameDLL.h"
#include "../logger/logger.h"
#include "cs2/cschemasystem.h"

template<typename T>
T* GetInGameInterface(const char* name, const char * library) {
    const auto handle = GetModuleHandle(library);
    if (!handle) {
        logger::log("No handle found.");
        return nullptr;
    }

    const auto functionAddress = GetProcAddress(handle, "CreateInterface");
    if (!functionAddress) {
        logger::log("CreateInterface not found.");
        return nullptr;
    }

    using Fn = T*(*)(const char*, int*);
    const auto CreateInterface = reinterpret_cast<Fn>(functionAddress);
    if (CreateInterface == nullptr) {
        return nullptr;
    }
    return CreateInterface(name, nullptr);
}

/*IVEngineServer2 * GameDLL::getIVEngineServer2Interface() {
    return GetInGameInterface<IVEngineServer2>("Source2EngineToServer001", "engine2.dll");
}*/

CSchemaSystem *GameDLL::getSchemaSystem() {
    return GetInGameInterface<CSchemaSystem>("SchemaSystem_001", "server.dll");
}

IVEngineServer2 *GameDLL::getSource2EngineToServer() {
    return GetInGameInterface<IVEngineServer2>("Source2EngineToServer001", "engine2.dll");
}

ISource2GameClients *GameDLL::getSource2GameClients() {
    return GetInGameInterface<ISource2GameClients>("Source2GameClients001", "server.dll");
}