//
// Created by Twelvee on 16.11.2023.
//

#ifndef RESOURCEMOD_MEMORY_H
#define RESOURCEMOD_MEMORY_H

#include <string>
#include <map>
#include <rana/rana.hpp>
#include "../../logger/logger.h"

class Memory {
public:
    std::map<std::string, int> offsets;

    void LoadOffsets(std::string path) {
        rana::value json = rana::from_file(path.c_str());
        for (const auto &element: json["offsets"].iter_object()) {
            this->offsets[element.first.c_str()] = atoi(element.second.find("windows")->second.as_string().c_str());
        }
    }

    void LoadSignatures(std::string path) {
    }
};

#endif RESOURCEMOD_MEMORY_H
