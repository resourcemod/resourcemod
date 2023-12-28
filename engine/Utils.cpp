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

    if (meta["features"]["external_runtime"].is_true()) {
        std::string externalRuntimePullAddr =
                meta["external_runtime"]["endpoints"]["pull"].is_null() ?
                "127.0.0.1:30001" :
                meta["external_runtime"]["endpoints"]["pull"]["host"].as_string() + ":" +
                meta["external_runtime"]["endpoints"]["pull"]["port"].as_string();

        std::string externalRuntimePushAddr =
                meta["external_runtime"]["endpoints"]["push"].is_null() ?
                "127.0.0.1:30002" :
                meta["external_runtime"]["endpoints"]["push"]["host"].as_string() + ":" +
                meta["external_runtime"]["endpoints"]["push"]["port"].as_string();

        g_Engine->pullAddr = externalRuntimePullAddr;
        g_Engine->pushAddr = externalRuntimePushAddr;
        Engine::RunExternalRuntime(meta["external_runtime"]["console"].is_true());
        logger::log("Launching external runtime");
        for (;;) {
            if (g_Engine->isExternalRuntimeRunning) {
                break;
            }
        }
    }

    this->isRunning = true;
}

void Engine::LoadPlugins() {
    rana::value meta = rana::from_file(this->pluginsPath.c_str()); // plugins.json
    logger::log("Loading plugins...");
    for (const auto &element: meta.iter_object()) {
        logger::log(element.first);
        std::string folder = this->pluginsFolder.c_str();
        folder.append("/").append(element.first);
        std::string meta = folder.c_str();
        meta.append("/").append("plugin.json");
        rana::value info = rana::from_file(meta);
        std::string authors;
        if (info["authors"].is_array()) {
            for(const auto &author: info["authors"].iter_array()) {
                authors.append(", ").append(author.as_string());
            }
        } else {
            authors = "unknown";
        }
        Plugin *p = new Plugin(info["name"].as_string(), info["version"].as_string(), folder, info["website"].as_string(), authors);
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

v8::Isolate* Engine::GetIsolate() {
    return this->isolate;
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