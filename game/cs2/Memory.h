//
// Created by Twelvee on 16.11.2023.
//

#ifndef RESOURCEMOD_MEMORY_H
#define RESOURCEMOD_MEMORY_H

#include <string>
#include <map>
#include "../../logger/logger.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Memory {
public:
    std::map<std::string, int> offsets;

    void LoadOffsets(std::string path) {
        std::ifstream f(path);
        json list = json::parse(f);
        for (auto element = list["offsets"].begin(); element != list["offsets"].end(); ++element) {
            std::string v = element.value().find("windows").value();
            this->offsets[element.key()] = atoi(v.c_str());
        }
    }

    void LoadSignatures(std::string path) {
    }
};

#endif RESOURCEMOD_MEMORY_H
