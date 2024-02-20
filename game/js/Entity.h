//
// Created by Twelvee on 13.11.2023.
//

#ifndef RESOURCEMOD_ENTITY_H
#define RESOURCEMOD_ENTITY_H

#include "../cs2/cbaseplayercontroller.h"
#include "../cs2/csplayerpawn.h"
#include "../cs2/ccsplayercontroller.h"
#include "../ResourceMod.h"
#include <engine/igameeventsystem.h>
#include <igameevents.h>
#include <metacall/metacall.h>
#include <vector>

class Entity {
public:
    static void *Create(size_t argc, void *args[], void *data);

    static void *Spawn(size_t argc, void *args[], void *data);

    static void *Remove(size_t argc, void *args[], void *data);

    static void *SetModel(size_t argc, void *args[], void *data);

    static void *GetCollision(size_t argc, void *args[], void *data);

    static void *SetCollision(size_t argc, void *args[], void *data);

    static void *SetColor(size_t argc, void *args[], void *data);

    static void *GetColor(size_t argc, void *args[], void *data);

    static void *GetAngle(size_t argc, void *args[], void *data);

    static void *SetAngle(size_t argc, void *args[], void *data);

    static void *GetCoords(size_t argc, void *args[], void *data);

    static void *SetCoords(size_t argc, void *args[], void *data);
};

#endif //RESOURCEMOD_ENTITY_H
