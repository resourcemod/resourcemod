//
// Created by Twelvee on 06.09.2023.
//

#ifndef RESOURCEMOD_GAMEDLL_H
#define RESOURCEMOD_GAMEDLL_H

#include <interfaces/interfaces.h>
#include <engine/igameeventsystem.h>
#include "cs2/cschemasystem.h"
#include "entitysystem.h"

#define IFACE_MAXNUM			999

class GameDLL {
public:
    static void BootResourceMod();

    static bool InitGameSystems();
};

#endif //RESOURCEMOD_GAMEDLL_H
