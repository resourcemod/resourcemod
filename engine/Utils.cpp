//
// Created by Twelvee on 17.09.2023.
//
#include "Engine.h"
#include "../logger/logger.h"
#include "Plugin.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

extern Engine *g_Engine;

void Engine::LoadMeta() {
    std::ifstream f(this->metaPath);
    json meta = json::parse(f);

    std::string n = meta["name"];
    logger::log(logger::format("Launching %s", n.c_str()));

    if (meta["features"]["database"]) {
        this->isDatabaseRequired = true;
        this->databaseType = meta["database"]["type"];
        this->databaseHost = meta["database"]["connection"]["host"];
        this->databasePort = meta["database"]["connection"]["port"];
        this->databaseUser = meta["database"]["connection"]["user"];
        this->databasePass = meta["database"]["connection"]["password"];
        this->databaseName = meta["database"]["connection"]["database"];
        if (!this->RunDatabaseConnector()) {
            logger::log("Cannot start the database module, but it is required.");
            logger::fatal();
            return;
        }
    }

    if (meta["features"]["external_runtime"]) {
        std::string externalRuntimePullAddr =
                meta["external_runtime"]["endpoints"]["pull"].is_null() ?
                "127.0.0.1:30001" :
                meta["external_runtime"]["endpoints"]["pull"]["host"] + ":" +
                meta["external_runtime"]["endpoints"]["pull"]["port"];

        std::string externalRuntimePushAddr =
                meta["external_runtime"]["endpoints"]["push"].is_null() ?
                "127.0.0.1:30002" :
                meta["external_runtime"]["endpoints"]["push"]["host"] + ":" +
                meta["external_runtime"]["endpoints"]["push"]["port"];

        g_Engine->pullAddr = externalRuntimePullAddr;
        g_Engine->pushAddr = externalRuntimePushAddr;
        if (!Engine::RunExternalRuntime(meta["external_runtime"]["console"])) {
            logger::log("Cannot start the external runtime module, but it is required.");
            logger::fatal();
            return;
        }
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
    std::ifstream f(this->pluginsPath);
    json list = json::parse(f);

    logger::log("Loading plugins...");
    for (auto element = list.begin(); element != list.end(); ++element){
        logger::log(element.key());
        std::string folder = this->pluginsFolder.c_str();
        folder.append("/").append(element.key());
        std::string meta = folder.c_str();
        meta.append("/").append("plugin.json");

        std::ifstream fi(meta);
        json info = json::parse(fi);
        std::string authors;
        if (info["authors"].is_array()) {
            for(auto author: info["authors"]) {
                authors.append(", ").append(author);
            }
        } else {
            authors = "unknown";
        }
        Plugin *p = new Plugin(info["name"], info["version"], folder, info["website"], authors);
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