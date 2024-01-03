#include "Engine.h"
#include "../logger/logger.h"
#include <chrono>
#include <mutex>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

extern Engine *g_Engine;

void Engine::SendNodeMessage(std::string event, std::string data) {
    json v;
    v["event"] = event;
    v["message"] = data;
    std::string json = v.dump();
    g_Engine->rtEvents.push(json.c_str());
}

void Engine::HandleSocketMessage() {
    for (;;) {
        if (g_Engine->rtCallbacks.empty()) {
            continue;
        }
        std::string msg = g_Engine->rtCallbacks.pop();
        json v = json::parse(msg);
        if (!v.empty()) {
            continue;
        }
        g_Engine->FireEvent(v["event"], v["data"].dump());
    }
}

bool LoadResourceModDependencies() {
    std::string path = "../../csgo/addons/resourcemod/bin/libzmq-mt-4_3_4.dll"; //todo: do smth
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
    std::string path = "../../csgo/addons/resourcemod/bin/external_runtime.dll"; //todo: do smth
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