//
// Created by Twelvee on 20.02.2024.
//
#include "Entity.h"
#include "../../protobuf/generated/network_connection.pb.h"
#include <metacall/metacall.h>

class Entity;

extern ResourceMod *g_ResourceMod;
extern IVEngineServer2 *g_SourceEngine;
extern Engine *g_Engine;

void *Entity::Create(size_t argc, void *args[], void *data) {
    std::string type = metacall_value_to_string(args[0]);
    auto entity = SignatureCall::UTIL_CreateEntityByName(type.c_str(), -1);
    if (entity == nullptr) {
        return metacall_value_create_int(-1);
    }
    int key = g_Engine->entities.size() + 1;
    g_Engine->entities[key] = entity;

    return metacall_value_create_int(key);
}

void *Entity::Spawn(size_t argc, void *args[], void *data) {
    int key = metacall_value_to_int(args[0]);
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return metacall_value_create_bool(false);
    }
    entity->Spawn();
    return metacall_value_create_bool(true);
}

void *Entity::SetModel(size_t argc, void *args[], void *data) {
    int key = metacall_value_to_int(args[0]);
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return metacall_value_create_bool(false);
    }
    std::string model = metacall_value_to_string(args[1]);
    logger::log(logger::format("Model: %s, entity_Id: %d", model.c_str(), key));
    g_ResourceMod->NextFrame([entity, model]() {
        entity->SetModel(model.c_str()); // may not work if model wasn't precached. Should we add additional checks?
    });
    return metacall_value_create_bool(true);
}

void *Entity::SetCollision(size_t argc, void *args[], void *data) {
    int key = metacall_value_to_int(args[0]);
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return metacall_value_create_bool(false);
    }

    entity->m_Collision.Get().m_nSolidType = (SolidType_t)metacall_value_to_int(args[1]); // collision type. 6 = SOLID_VPHYSICS
    return metacall_value_create_bool(true);
}

void *Entity::GetCollision(size_t argc, void *args[], void *data) {
    int key = metacall_value_to_int(args[0]);
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return metacall_value_create_int(-1);
    }
    return metacall_value_create_int(entity->m_Collision.Get().m_nSolidType.Get());
}

void *Entity::SetColor(size_t argc, void *args[], void *data) {
    int key = metacall_value_to_int(args[0]);
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return metacall_value_create_bool(false);
    }

    void **v_module_map = static_cast<void **>(metacall_value_to_map(args[1]));
    int red = 0;
    int green = 0;
    int blue = 0;
    int alpha = 0;
    // fucked up but metacall can't work with classes objects properly for now
    for (int i = 0; i < metacall_value_size(args[1]) / sizeof(v_module_map[0]); ++i) {
        auto v = metacall_value_to_array(v_module_map[i]);
        if (V_strcmp(metacall_value_to_string(v[0]), "red") == 0) {
            red = int(metacall_value_to_double(v[1]));
        } else if (V_strcmp(metacall_value_to_string(v[0]), "green") == 0) {
            green = int(metacall_value_to_double(v[1]));
        } else if (V_strcmp(metacall_value_to_string(v[0]), "blue") == 0) {
            blue = int(metacall_value_to_double(v[1]));
        } else if (V_strcmp(metacall_value_to_string(v[0]), "alpha") == 0) {
            alpha = int(metacall_value_to_double(v[1]));
        }
    }
    Color color = Color(red, green, blue, alpha);
    g_ResourceMod->NextFrame([entity, color]() {
        entity->SetColor(color);
    });
    return metacall_value_create_bool(true);
}

void *Entity::GetColor(size_t argc, void *args[], void *data) {
    int key = metacall_value_to_int(args[0]);
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        void *colorArgs[] = {
                metacall_value_create_int(0),
                metacall_value_create_int(0),
                metacall_value_create_int(0),
                metacall_value_create_int(0)
        };

        void *vObj = metacallv("_CreateColor", colorArgs);

        metacall_value_destroy(colorArgs[0]);
        metacall_value_destroy(colorArgs[1]);
        metacall_value_destroy(colorArgs[2]);
        metacall_value_destroy(colorArgs[3]);

        return vObj;
    }
    Color color = entity->m_clrRender.Get();
    void *colorArgs[] = {
            metacall_value_create_int(color.r()),
            metacall_value_create_int(color.g()),
            metacall_value_create_int(color.b()),
            metacall_value_create_int(color.a())
    };

    void *vObj = metacallv("_CreateColor", colorArgs);

    metacall_value_destroy(colorArgs[0]);
    metacall_value_destroy(colorArgs[1]);
    metacall_value_destroy(colorArgs[2]);
    metacall_value_destroy(colorArgs[3]);

    return vObj;
}

void *Entity::Remove(size_t argc, void *args[], void *data) {
    int key = metacall_value_to_int(args[0]);
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return metacall_value_create_bool(false);
    }
    entity->Remove();
    g_Engine->entities.erase(key);
    return metacall_value_create_bool(true);
}

void *Entity::GetCoords(size_t argc, void *args[], void *data) {
    int key = metacall_value_to_int(args[0]);
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        void *vectorArgs[] = {
                metacall_value_create_float(0),
                metacall_value_create_float(0),
                metacall_value_create_float(0)
        };

        void *vObj = metacallv("_CreateCoords", vectorArgs);

        metacall_value_destroy(vectorArgs[0]);
        metacall_value_destroy(vectorArgs[1]);
        metacall_value_destroy(vectorArgs[2]);

        return vObj;
    }

    auto vector = entity->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin();
    void *vectorArgs[] = {
            metacall_value_create_float(vector.x),
            metacall_value_create_float(vector.y),
            metacall_value_create_float(vector.z)
    };

    void *vObj = metacallv("_CreateCoords", vectorArgs);

    metacall_value_destroy(vectorArgs[0]);
    metacall_value_destroy(vectorArgs[1]);
    metacall_value_destroy(vectorArgs[2]);

    return vObj;
}

void *Entity::SetCoords(size_t argc, void *args[], void *data) {
    int key = metacall_value_to_int(args[0]);
    auto entity = g_Engine->entities[key];
    if (entity == nullptr) {
        return metacall_value_create_bool(false);
    }
    auto x = metacall_value_to_float(args[1]);
    auto y = metacall_value_to_float(args[2]);
    auto z = metacall_value_to_float(args[3]);
    Vector vec(x, y, z);
    entity->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin = vec;
    return metacall_value_create_bool(true);
}