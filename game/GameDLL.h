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
    static CSchemaSystem * getSchemaSystem();
    static IVEngineServer2 * getSource2EngineToServer();
    static ISource2GameClients* getSource2GameClients();

    CGameEntitySystem *GetGameEntitySystem();

    static void BootResourceMod();
};

#endif //RESOURCEMOD_GAMEDLL_H
