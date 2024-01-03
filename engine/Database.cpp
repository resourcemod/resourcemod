//
// Created by Twelvee on 28.12.2023.
//

#include "Engine.h"
#include "../logger/logger.h"
#include "EventLoop.h"

extern EventLoop* g_EventLoop;

bool Engine::RunDatabaseConnector() {
    std::string path = "../../csgo/addons/resourcemod/bin/databases.dll"; //todo: do smth
    const auto hd = LoadLibraryA(path.c_str());
    if (!hd) {
        printf("\n\n%lu\n\n", GetLastError());
        return false;
    }

    logger::log("Done! Executing exposed method...");
    const auto run = GetProcAddress(hd, "Run");
    if (!run) {
        printf("\n\nERRTHR:%lu\n\n", GetLastError());
        return false;
    }

    using Fn = void *(*)(Engine*, EventLoop*);
    const auto RunDatabasesModule = reinterpret_cast<Fn>(run);
    RunDatabasesModule(this, g_EventLoop);

    return true;
}