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

#ifndef IS_INTERNAL_RUNTIME
#else

#include "igameevents.h"

#endif

#define EXPORT __declspec(dllexport)

#define CS_TEAM_NONE        0
#define CS_TEAM_SPECTATOR   1
#define CS_TEAM_T           2
#define CS_TEAM_CT          3

#define HUD_PRINTNOTIFY        1
#define HUD_PRINTCONSOLE    2
#define HUD_PRINTTALK        3
#define HUD_PRINTCENTER        4

EXPORT class Engine {
public:
    const std::string RESOURCEMOD_META = "resourcemod.config.js";

    std::string rootPath = "";
    std::string resourcemodFolder = "";
    std::string pluginsFolder = "";
    std::string gameDataPath = "";
    std::string metaPath = "";
    bool isRunning = false;

    void Init();

    void InitMetacall();
    void LoadMeta();

    // Utils
    static std::string GetFileContent(std::string);

    static bool HasSuffix(std::string string, std::string suffix);

    void Shutdown();
};


#endif //RESOURCEMOD_ENGINE_H
