//
// Created by Twelvee on 13.09.2023.
//

#include "Engine.h"
#include "../logger/logger.h"
#include "../game/hooks/EventManager.h"
#include "../game/ResourceMod.h"
#include <cstdlib>
#include "../../game/js/Weapon.h"
#include <api.h>
#include <linux/limits.h>
#include <csignal>

Engine *g_Engine;

void Engine::Init() {
    // Get current exe path
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        this->rootPath = cwd;
    }

    this->resourcemodFolder = "../../csgo/addons/resourcemod";

    this->gameDataPath = this->resourcemodFolder.c_str();
    this->gameDataPath.append("/gamedata/rmod.cs2.json");
    g_Engine = this;
    this->InitResourceModApi();
}

void *msg(size_t argc, void *args[]) {
    for (int i = 0; i < argc; i++) {
        ConMsg(go_value_to_string(args[i]));
    }

    (void) argc;
    return go_value_create_bool(true);
}

void* err(size_t argc, void *args[]) {
    for (int i = 0; i < argc; i++) {
        ConColorMsg(Color(255,0,0,1), go_value_to_string(args[i]));
    }

    (void) argc;
    return go_value_create_bool(true);
}

void Engine::InitResourceModApi() {
    go_register("log", msg);
    go_register("err", err);

    go_register("_GameMessage", Player::PrintGameMessage); // slot, message, duration
    go_register("_AllPrint", Player::PrintAll);
    go_register("_PlayerPrint", Player::Print);

    go_register("_PlayerGetHP", Player::GetHP);
    go_register("_PlayerSetHP", Player::SetHP);
    go_register("_PlayerGetName", Player::GetName);
    go_register("_PlayerGetSteamID", Player::GetSteamID);
    go_register("_PlayerGetIsAlive", Player::GetIsAlive);
    go_register("_PlayerSlap", Player::Slap);
    go_register("_PlayerSlay", Player::Slay);
    go_register("_PlayerRespawn", Player::Respawn);
    go_register("_PlayerKick", Player::Kick);
    go_register("_PlayerSetModel", Player::SetModel);
    go_register("_PlayerSetColor", Player::SetColor);
    go_register("_PlayerPlaySound", Player::Play);
    go_register("_PlayerGetTeam", Player::GetTeam);
    go_register("_PlayerChangeTeam", Player::ChangeTeam);


    go_register("_PlayerGetIsConnected", Player::GetIsConnected);
    go_register("_PlayerGetIsConnecting", Player::GetIsConnecting);
    go_register("_PlayerGetIsDisconnected", Player::GetIsDisconnected);
    go_register("_PlayerGetIsDisconnecting", Player::GetIsDisconnecting);
    go_register("_PlayerGetIsReserved", Player::GetIsReserved);
    go_register("_PlayerGetIsReconnecting", Player::GetIsReconnecting);

    // weapons
    go_register("_WeaponGive", Weapon::Give, nullptr);
    go_register("_WeaponGet", Weapon::Get);
    go_register("_WeaponDrop", Weapon::Drop);

    // armor
    go_register("_PlayerGetArmor", Player::GetArmor);
    go_register("_PlayerSetArmor", Player::SetArmor);

    // money
    go_register("_PlayerGetMoney", Player::GetMoney);
    go_register("_PlayerSetMoney", Player::SetMoney);

    // stats
    go_register("_PlayerGetDamage", Player::GetDamage);
    go_register("_PlayerGetKills", Player::GetKills);
    go_register("_PlayerGetAssists", Player::GetAssists);
    go_register("_PlayerGetDeaths", Player::GetDeaths);

    go_register("_PlayerSetDamage", Player::SetDamage);
    go_register("_PlayerSetKills", Player::SetKills);
    go_register("_PlayerSetAssists", Player::SetAssists);
    go_register("_PlayerSetDeaths", Player::SetDeaths);

    // entities
    // model_path, x, y, z
    go_register("_EntityCreate", Entity::Create, nullptr, METACALL_INT, 1, METACALL_STRING); // returns entity id from Engine->entities map
    go_register("_EntitySpawn", Entity::Spawn, nullptr, METACALL_INT, 2, METACALL_INT, METACALL_STRING);
    go_register("_EntityRemove", Entity::Remove, nullptr, METACALL_BOOL, 1, METACALL_INT);

    go_register("_EntitySetModel", Entity::SetModel, nullptr, METACALL_BOOL, 2, METACALL_INT, METACALL_STRING);
    go_register("_EntitySetColor", Entity::SetColor, nullptr, METACALL_BOOL, 2, METACALL_INT, METACALL_OBJECT);
    go_register("_EntityGetColor", Entity::GetColor, nullptr, METACALL_OBJECT, 2, METACALL_INT);
    go_register("_EntityGetCollision", Entity::GetCollision, nullptr, METACALL_INT, 1, METACALL_INT);
    go_register("_EntitySetCollision", Entity::SetCollision, nullptr, METACALL_BOOL, 2, METACALL_INT, METACALL_INT);

    // coords & angles
    go_register("_EntityGetCoords", Entity::GetCoords, nullptr, METACALL_OBJECT, 2, METACALL_INT, METACALL_INT);
    go_register("_EntitySetCoords", Entity::SetCoords, nullptr, METACALL_BOOL, 5, METACALL_INT, METACALL_INT, METACALL_FLOAT, METACALL_FLOAT, METACALL_FLOAT);

    go_register("_EntityGetAngle", Entity::GetAngle, nullptr, METACALL_OBJECT, 2, METACALL_INT, METACALL_INT);
    go_register("_EntitySetAngle", Entity::SetAngle, nullptr, METACALL_BOOL, 7, METACALL_INT, METACALL_INT, METACALL_FLOAT, METACALL_FLOAT, METACALL_FLOAT, METACALL_STRING, METACALL_STRING);
    // todo: load entrypoint configuration (json)
    // todo: precache resources
    // todo: bootstrap server script function (entrypoint)

    this->isRunning = true;
}

void Engine::Tick() {
    void* args = metacall("_EventTick");
    metacall_value_destroy(args);
}

void Engine::SetEntityModel(int key, const char* model) {
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return;
    }
    entity->SetModel(model);
}

extern EventManager *g_EventManager;

void Engine::Shutdown() {
    // todo: properly shutdown go runtime (free memory, close channels etc)
}
