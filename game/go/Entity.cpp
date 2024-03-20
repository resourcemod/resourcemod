//
// Created by Twelvee on 20.02.2024.
//
#include "Entity.h"
#include "../../protobuf/generated/network_connection.pb.h"
#include "../cs2/cbaseplayercontroller.h"
#include "../cs2/csplayerpawn.h"
#include "../cs2/ccsplayercontroller.h"
#include "../ResourceMod.h"
#include <igameevents.h>

class Entity;

extern ResourceMod *g_ResourceMod;
extern IVEngineServer2 *g_SourceEngine;
extern Engine *g_Engine;

int Create(const char* type) {
    auto entity = SignatureCall::UTIL_CreateEntityByName(type, -1);
    if (entity == nullptr) {
        return -1;
    }
    int key = g_Engine->entities.size() + 1;
    g_Engine->entities[key] = entity;

    return key;
}

bool Spawn(int key, const char* model) {
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return false;
    }

    g_ResourceMod->NextFrame([entity, model]() {
        entity->SetModel(model); // may not work if model wasn't precached. Should we add additional checks?
    });

    entity->Spawn();
    return true;
}

bool SetModel(int key, const char* model) {
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return false;
    }
    g_ResourceMod->NextFrame([entity, model]() {
        entity->SetModel(model); // may not work if model wasn't precached. Should we add additional checks?
    });
    return true;
}

bool SetCollision(int key, int type) {
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return false;
    }

    entity->m_Collision.Get().m_nSolidType = (SolidType_t) type; // collision type. 6 = SOLID_VPHYSICS
    return true;
}

int GetCollision(int key) {
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return -1;
    }
    return entity->m_Collision.Get().m_nSolidType.Get();
}

bool SetColor(int key, int red, int green, int blue, int alpha) {
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return false;
    }

    Color color = Color(red, green, blue, alpha);
    g_ResourceMod->NextFrame([entity, color]() {
        entity->SetColor(color);
    });
    return true;
}

char* GetColor(int key) {
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return "0,0,0,0";
    }
    Color color = entity->m_clrRender.Get();
    char* result;
    sprintf(result, "%d,%d,%d,%d", color.r(), color.g(), color.b(), color.a());
    return result;
}

bool Remove(int key) {
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return false;
    }
    entity->Remove();
    g_Engine->entities.erase(key);
    return true;
}

char* GetCoords(int key, int slot) {
    if (slot == -1) {
        return GetEntityCoords(key);
    }
    return GetPlayerCoords(slot);
}

char* GetPlayerCoords(int slot) {
    CCSPlayerController* c = CCSPlayerController::FromSlot(slot);
    if (c == nullptr) {
        return "0.0,0.0,0.0";
    }

    Vector vector = c->GetPawn()->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin();
    char* result;
    sprintf(result, "%.6f,%.6f,%.6f", vector.x, vector.y, vector.z);
    return result;
}

char* GetEntityCoords(int key) {
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return "0.0,0.0,0.0";
    }

    Vector vector = entity->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin();
    char* result;
    sprintf(result, "%.6f,%.6f,%.6f", vector.x, vector.y, vector.z);
    return result;
}

bool SetCoords(int key, int slot, float x, float y, float z) {
    if (slot != -1) {
        return SetPlayerCoords(slot, x, y, z);
    }

    return SetEntityCoords(key, x, y, z);
}

bool SetPlayerCoords(int slot, float x, float y, float z) {
    CCSPlayerController* c = CCSPlayerController::FromSlot(slot);
    if (c == nullptr) {
        return false;
    }
    Vector vec(x, y, z);
    c->GetPawn()->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin = vec;
    return true;
}

bool SetEntityCoords(int key, float x, float y, float z) {
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return false;
    }
    Vector vec(x, y, z);
    entity->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin = vec;
    return true;
}

char* GetAngle(int key, int slot) {
    if (slot != -1) {
        return GetPlayerAngle(slot);
    }
    return GetEntityAngle(key);
}

char* GetPlayerAngle(int slot) {
    CCSPlayerController* c = CCSPlayerController::FromSlot(slot);
    if (c == nullptr) {
        return "0.0,0.0,0.0,0.0";
    }

    QAngle angle = c->GetPawn()->m_CBodyComponent->m_pSceneNode->m_angAbsRotation();
    char* result;
    sprintf(result, "%.6f,%.6f,%.6f,%.6f", 1.0f, angle.x, angle.y, angle.z);
    return result;
}

char* GetEntityAngle(int key) {
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return "0.0,0.0,0.0,0.0";
    }

    QAngle angle = entity->m_CBodyComponent->m_pSceneNode->m_angAbsRotation();
    char* result;
    sprintf(result, "%.6f,%.6f,%.6f,%.6f", 1.0f, angle.x, angle.y, angle.z);
    return result;
}

bool SetAngle(int key, int slot, float x, float y, float z, const char* name, const char* model) {
    if (slot != -1) {
        return SetPlayerAngle(slot, x, y, z);
    }
    return SetEntityAngle(key, x, y, z, name, model);
}

bool SetPlayerAngle(int slot, float x, float y, float z) {
    CCSPlayerController* c = CCSPlayerController::FromSlot(slot);
    if (c == nullptr) {
        return false;
    }
    QAngle angle(x, y, z);
    angle.x = 0;
    CALL_VIRTUAL(void, g_Memory->offsets["Teleport"], c->GetPawn(), nullptr, &angle, nullptr);
    SignatureCall::SnapViewAngles(c->GetPawn(), angle);
    return true;
}

bool SetEntityAngle(int key, float x, float y, float z, const char* name, const char* model) {
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return false;
    }
    QAngle angle(x, y, z);
    SolidType_t type = entity->m_pCollision->m_nSolidType();
    Vector vec = entity->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin();
    auto newEntity = SignatureCall::UTIL_CreateEntityByName(name, -1);
    if (newEntity == nullptr) {
        return false;
    }
    newEntity->m_pCollision->m_nSolidType = type;
    newEntity->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin = vec;
    newEntity->m_CBodyComponent->m_pSceneNode->m_angAbsRotation = angle;

    g_ResourceMod->NextFrame([key, model]() {
        Engine::SetEntityModel(key, model); // may not work if model wasn't precached. Should we add additional checks?
    });
    entity->Remove();
    newEntity->Spawn();
    g_Engine->entities[key] = newEntity;

    return true;
}