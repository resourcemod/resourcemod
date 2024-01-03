//
// Created by Twelvee on 28.12.2023.
//

#include "MySQL.h"
#include "../../engine/Engine.h"
#include "../../logger/logger.h"
#include <Windows.h>

#if defined _WIN32
#define EXPORT extern "C" __declspec(dllexport)
#elif defined __GNUC__
#define EXPORT extern "C" __attribute__ ((visibility("default")))
#endif

Engine *g_RMEngine;

EXPORT void *Run(const char *path, bool console, Engine *engine) {
    g_RMEngine = engine;



    return nullptr;
}