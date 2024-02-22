//
// Created by Twelvee on 13.09.2023.
//

#include "Engine.h"
#include "../logger/logger.h"
#include "../game/hooks/EventManager.h"
#include "../game/ResourceMod.h"
#include <metacall/metacall.h>
#include <cstdlib>
#include "../../game/js/Weapon.h"
#ifdef WIN_32
#include <direct.h>
#include <windows.h>
#endif

Engine *g_Engine;

void Engine::Init() {
    // Get current exe path
    char cwd[MAX_PATH];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        this->rootPath = cwd;
    }

    this->resourcemodFolder = "../../csgo/addons/resourcemod";

    this->gameDataPath = this->resourcemodFolder.c_str();
    this->gameDataPath.append("/node_modules/resourcemod/gamedata/rmod.cs2.json");
    g_Engine = this;
    this->InitMetacall();
}

void *msg(size_t argc, void *args[], void *data) {
    for (int i = 0; i < argc; i++) {
        ConMsg(metacall_value_to_string(args[i]));
    }

    (void) argc;
    (void) data;
    return metacall_value_create_bool(true);
}

void *err(size_t argc, void *args[], void *data) {
    for (int i = 0; i < argc; i++) {
        ConColorMsg(Color(255,0,0,1), metacall_value_to_string(args[i]));
    }

    (void) argc;
    (void) data;
    return metacall_value_create_bool(true);
}

void Engine::InitMetacall() {
    #ifdef WIN_32
    _putenv_s("LOADER_LIBRARY_PATH",
              std::filesystem::absolute(
                      "../../csgo/addons/resourcemod/node_modules/resourcemod/bin/metacall").string().c_str());
#endif
    // Initialize MetaCall
    if (metacall_initialize() != 0) {
        logger::log("Cannot initialize metacall");
        return;
    }

    metacall_register("log", msg, nullptr, METACALL_BOOL, 1, METACALL_STRING);
    metacall_register("err", err, nullptr, METACALL_BOOL, 1, METACALL_STRING);

    metacall_register("_GameMessage", Player::PrintGameMessage, nullptr, METACALL_BOOL, 3, METACALL_INT, METACALL_STRING, METACALL_INT); // slot, message, duration
    metacall_register("_AllPrint", Player::PrintAll, nullptr, METACALL_BOOL, 2, METACALL_INT, METACALL_STRING);
    metacall_register("_PlayerPrint", Player::Print, nullptr, METACALL_BOOL, 3, METACALL_INT, METACALL_INT,
                      METACALL_STRING);

    metacall_register("_PlayerGetHP", Player::GetHP, nullptr, METACALL_INT, 1, METACALL_INT);
    metacall_register("_PlayerSetHP", Player::SetHP, nullptr, METACALL_BOOL, 2, METACALL_INT, METACALL_INT);
    metacall_register("_PlayerGetName", Player::GetName, nullptr, METACALL_STRING, 1, METACALL_INT);
    metacall_register("_PlayerGetSteamID", Player::GetSteamID, nullptr, METACALL_STRING, 1, METACALL_INT);
    metacall_register("_PlayerGetIsAlive", Player::GetIsAlive, nullptr, METACALL_BOOL, 1, METACALL_INT);
    metacall_register("_PlayerSlap", Player::Slap, nullptr, METACALL_BOOL, 2, METACALL_INT, METACALL_INT);
    metacall_register("_PlayerSlay", Player::Slay, nullptr, METACALL_BOOL, 1, METACALL_INT);
    metacall_register("_PlayerRespawn", Player::Respawn, nullptr, METACALL_BOOL, 1, METACALL_INT);
    metacall_register("_PlayerKick", Player::Kick, nullptr, METACALL_BOOL, 1, METACALL_INT);
    metacall_register("_PlayerSetModel", Player::SetModel, nullptr, METACALL_BOOL, 2, METACALL_INT, METACALL_STRING);
    metacall_register("_PlayerSetColor", Player::SetColor, nullptr, METACALL_BOOL, 2, METACALL_INT, METACALL_OBJECT);
    metacall_register("_PlayerPlaySound", Player::Play, nullptr, METACALL_BOOL, 2, METACALL_INT, METACALL_STRING);
    metacall_register("_PlayerGetTeam", Player::GetTeam, nullptr, METACALL_INT, 1, METACALL_INT);
    metacall_register("_PlayerChangeTeam", Player::ChangeTeam, nullptr, METACALL_BOOL, 3, METACALL_INT, METACALL_INT,
                      METACALL_BOOL);

    metacall_register("_PlayerGetIsConnected", Player::GetIsConnected, nullptr, METACALL_BOOL, 1, METACALL_INT);
    metacall_register("_PlayerGetIsConnecting", Player::GetIsConnecting, nullptr, METACALL_BOOL, 1, METACALL_INT);
    metacall_register("_PlayerGetIsDisconnected", Player::GetIsDisconnected, nullptr, METACALL_BOOL, 1, METACALL_INT);
    metacall_register("_PlayerGetIsDisconnecting", Player::GetIsDisconnecting, nullptr, METACALL_BOOL, 1, METACALL_INT);
    metacall_register("_PlayerGetIsReserved", Player::GetIsReserved, nullptr, METACALL_BOOL, 1, METACALL_INT);
    metacall_register("_PlayerGetIsReconnecting", Player::GetIsReconnecting, nullptr, METACALL_BOOL, 1, METACALL_INT);

    // weapons
    metacall_register("_WeaponGive", Weapon::Give, nullptr, METACALL_BOOL, 2, METACALL_INT, METACALL_STRING);
    metacall_register("_WeaponGet", Weapon::Get, nullptr, METACALL_STRING, 2, METACALL_INT, METACALL_INT);
    metacall_register("_WeaponDrop", Weapon::Drop, nullptr, METACALL_BOOL, 2, METACALL_INT, METACALL_BOOL);

    // armor
    metacall_register("_PlayerGetArmor", Player::GetArmor, nullptr, METACALL_INT, 1, METACALL_INT);
    metacall_register("_PlayerSetArmor", Player::SetArmor, nullptr, METACALL_BOOL, 2, METACALL_INT, METACALL_INT);

    // money
    metacall_register("_PlayerGetMoney", Player::GetMoney, nullptr, METACALL_INT, 1, METACALL_INT);
    metacall_register("_PlayerSetMoney", Player::SetMoney, nullptr, METACALL_BOOL, 2, METACALL_INT, METACALL_INT);

    // stats
    metacall_register("_PlayerGetDamage", Player::GetDamage, nullptr, METACALL_INT, 1, METACALL_INT);
    metacall_register("_PlayerGetKills", Player::GetKills, nullptr, METACALL_INT, 1, METACALL_INT);
    metacall_register("_PlayerGetAssists", Player::GetAssists, nullptr, METACALL_INT, 1, METACALL_INT);
    metacall_register("_PlayerGetDeaths", Player::GetDeaths, nullptr, METACALL_INT, 1, METACALL_INT);

    metacall_register("_PlayerSetDamage", Player::SetDamage, nullptr, METACALL_BOOL, 2, METACALL_INT, METACALL_INT);
    metacall_register("_PlayerSetKills", Player::SetKills, nullptr, METACALL_BOOL, 2, METACALL_INT, METACALL_INT);
    metacall_register("_PlayerSetAssists", Player::SetAssists, nullptr, METACALL_BOOL, 2, METACALL_INT, METACALL_INT);
    metacall_register("_PlayerSetDeaths", Player::SetDeaths, nullptr, METACALL_BOOL, 2, METACALL_INT, METACALL_INT);

    // Array of scripts to be loaded by MetaCall
    const char *js_scripts[] =
            {
                    "../../csgo/addons/resourcemod/node_modules/resourcemod/dist/node/stdout.js",
                    "../../csgo/addons/resourcemod/node_modules/resourcemod/dist/node/exceptions.js",
                    "../../csgo/addons/resourcemod/node_modules/resourcemod/dist/node/constants.js",
                    "../../csgo/addons/resourcemod/node_modules/resourcemod/dist/node/player.js",
                    "../../csgo/addons/resourcemod/node_modules/resourcemod/dist/node/events.js",
                    "../../csgo/addons/resourcemod/node_modules/resourcemod/dist/node/chat.js",
                    "../../csgo/addons/resourcemod/node_modules/resourcemod/dist/node/precache.js",
                    "../../csgo/addons/resourcemod/node_modules/resourcemod/dist/node/entrypoint.js"
            };

    int s = metacall_load_from_file("node", js_scripts, sizeof(js_scripts) / sizeof(js_scripts[0]), NULL);
    if (s != 0) {
        logger::log(logger::format("Cannot load initial scripts, code: %d, last error: %d", s, GetLastError()));
        return;
    }

    void *entrp = metacall("_LoadEntrypoint");
    std::string entryPoint = this->resourcemodFolder.c_str();
    entryPoint.append("/").append(metacall_value_to_string(entrp));

    // Array of scripts to be loaded by MetaCall
    const char *actualScript[] =
            {
                    entryPoint.c_str(),
            };
    // Load server scripts
    s = metacall_load_from_file("node", actualScript, sizeof(actualScript) / sizeof(actualScript[0]), NULL);
    if (s != 0) {
        logger::log(logger::format("Cannot load scripts, code: %d, last error: %d", s, GetLastError()));
        return;
    }
    void *precache = metacall("_LoadPrecache");
    void **cacheList = metacall_value_to_array(precache);
    for (int i = 0; i < metacall_value_size(precache) / sizeof(cacheList[0]); i++) {
        this->precacheList.push_back(metacall_value_to_string(cacheList[i]));
    }
}

extern EventManager *g_EventManager;

void Engine::Shutdown() {
    // todo: properly shutdown metacall
}
