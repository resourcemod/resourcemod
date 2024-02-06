//
// Created by Twelvee on 17.09.2023.
//
#include "Engine.h"
#include "../logger/logger.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

extern Engine *g_Engine;

void Engine::LoadMeta() {
    std::ifstream f(this->metaPath);
    json meta = json::parse(f);

    std::string n = meta["name"];
    logger::log(logger::format("Launching %s", n.c_str()));

    this->isRunning = true;
}

std::string Engine::GetFileContent(std::string filePath) {
    std::ifstream input_file(filePath);
    if (!input_file.is_open()) {
        logger::log(logger::format("Failed to load file: %s", filePath.c_str()));
    }
    return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}


bool Engine::HasSuffix(std::string str, std::string suffix) {
    size_t len = str.length();
    size_t slen = suffix.length();
    return (len >= slen && !memcmp(str.c_str() + len - slen, suffix.c_str(), slen));
}