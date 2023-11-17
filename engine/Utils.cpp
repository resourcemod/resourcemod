//
// Created by Twelvee on 17.09.2023.
//
#include "Engine.h"
#include "../logger/logger.h"
#include "Plugin.h"
#include <fstream>
#include <rana/rana.hpp>

extern Engine *g_Engine;

void Engine::LoadMeta() {
    rana::value meta = rana::from_file(this->metaPath.c_str());

    std::string n = meta["name"].as_string();
    logger::log(logger::format("Launching %s", n.c_str()));

    if(meta["features"]["external_runtime"].is_true()) {
        std::string externalRuntimePullAddr =
                meta["external_runtime"]["endpoints"]["pull"].is_null() ?
                "127.0.0.1:30001" :
                meta["external_runtime"]["endpoints"]["pull"]["host"].as_string()+":"+meta["external_runtime"]["endpoints"]["pull"]["port"].as_string();

        std::string externalRuntimePushAddr =
                meta["external_runtime"]["endpoints"]["push"].is_null() ?
                "127.0.0.1:30002" :
                meta["external_runtime"]["endpoints"]["push"]["host"].as_string()+":"+meta["external_runtime"]["endpoints"]["push"]["port"].as_string();

        g_Engine->pullAddr = externalRuntimePullAddr;
        g_Engine->pushAddr = externalRuntimePushAddr;
        Engine::RunExternalRuntime(meta["external_runtime"]["console"].is_true());
        logger::log("Launching external runtime");
        for(;;) {
            logger::log(logger::format("is ext rt running: %d", g_Engine->isExternalRuntimeRunning));
            if (g_Engine->isExternalRuntimeRunning) {
                break;
            }
        }
    }

    if (!this->isRunning) {
        this->isRunning = true;
    }
    logger::log("Done! Loading ResourceMod plugins in memory...");

    for (const auto &element: meta["plugins"].iter_object()) {
        Plugin *p = new Plugin(element.first, element.second.as_string());
        this->plugins.push_back(p);
        p->LoadPluginFS();
    }
}
std::string Engine::GetFileContent(std::string filePath) {
    std::ifstream input_file(filePath);
    if (!input_file.is_open()) {
        logger::log(logger::format("Failed to load file: %s", filePath.c_str()));
    }
    return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

std::string Engine::LoadPluginFile(std::string pluginName, std::string pluginsFolder) {
    return Engine::GetFileContent(
            pluginsFolder.append("\\").append(pluginName.c_str()).append("\\").append("plugin.js"));
}

bool Engine::HasSuffix(std::string str, std::string suffix) {
    size_t len = str.length();
    size_t slen = suffix.length();
    return (len >= slen && !memcmp(str.c_str() + len - slen, suffix.c_str(), slen));
}