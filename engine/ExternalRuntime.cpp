#include "Engine.h"
#include "../logger/logger.h"
#include <chrono>
#include <rana/rana.hpp>
#include <mutex>

extern Engine *g_Engine;

void Engine::SendNodeMessage(std::string event, std::string data) {
    rana::value v = rana::from_string("{}");
    v["event"] = event;
    v["message"] = data;
    std::string json = rana::to_string(v);
    g_Engine->rtEvents.push(json.c_str());
}

void Engine::HandleSocketMessage() {
    for (;;) {
        if (g_Engine->rtCallbacks.empty()) {
            continue;
        }
        std::string msg = g_Engine->rtCallbacks.pop();
        rana::value v = rana::from_string(msg);
        if (!v.is_valid()) {
            continue;
        }
        g_Engine->FireEvent(v["event"].as_string(), rana::to_string(v["data"]));
    }
}

bool LoadResourceModDependencies() {
    std::string path = "../../csgo/addons/resourcemod/bin/libzmq-mt-4_3_4.dll";
    const auto hd = LoadLibraryA(path.c_str());
    if (!hd) {
        printf("\n\n%lu\n\n", GetLastError());
        return false;
    }
    return true;
}

bool Engine::RunExternalRuntime(bool console) {
    if (!LoadResourceModDependencies()) {
        logger::log("Failed to inject external runtime libraries");
        return false;
    }
    std::string path = "../../csgo/addons/resourcemod/bin/external_runtime.dll";
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

    using Fn = void *(*)(const char *, bool, Engine*);
    const auto RunExternalRT = reinterpret_cast<Fn>(run);
    RunExternalRT(g_Engine->resourcemodFolder.c_str(), console, g_Engine);

    std::thread cbHandler(Engine::HandleSocketMessage);
    cbHandler.detach();
    return true;
}