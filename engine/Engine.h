//
// Created by Twelvee on 13.09.2023.
//

#ifndef RESOURCEMOD_ENGINE_H
#define RESOURCEMOD_ENGINE_H

#include <string>
#include <windows.h>
#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include "TSQueue.h"
#include <map>

#include "../game/cs2/cbasemodelentity.h"
#ifndef IS_INTERNAL_RUNTIME
#else

#include "igameevents.h"

#endif

#define CS_TEAM_NONE        0
#define CS_TEAM_SPECTATOR   1
#define CS_TEAM_T           2
#define CS_TEAM_CT          3

#define HUD_PRINTNOTIFY        1
#define HUD_PRINTCONSOLE    2
#define HUD_PRINTTALK        3
#define HUD_PRINTCENTER        4
#define HUD_GAME_MESSAGE 5

class Engine {
public:
    const std::string RESOURCEMOD_META = "resourcemod.config.js";

    std::string rootPath = "";
    std::string resourcemodFolder = "";
    std::string pluginsFolder = "";
    std::string gameDataPath = "";

    std::vector<std::string> precacheList;

    void Init();

    static uint64_t Now();

    std::map<int, std::pair<uint64_t, std::string>> gameMessages;
    std::map<int, CBaseModelEntity *> entities;

    void InitMetacall();

    // Utils
    static std::string GetFileContent(std::string);

    static bool HasSuffix(std::string string, std::string suffix);

    void Shutdown();
};


#endif //RESOURCEMOD_ENGINE_H
