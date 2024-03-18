//
// Created by Twelvee on 13.11.2023.
//

#ifndef RESOURCEMOD_ENTITY_H
#define RESOURCEMOD_ENTITY_H

#include <vector>
#include <cstdio>

static int Create(const char* type);

static bool Spawn(int key, const char* model);

static bool Remove(int key);

static bool SetModel(int key, const char* model);

static int GetCollision(int key);

static bool SetCollision(int key, int type);

static bool SetColor(int key, int red, int green, int blue, int alpha);

static char* GetColor(int key);

static char* GetAngle(int key, int slot);

static char* GetPlayerAngle(int slot);

static char* GetEntityAngle(int key);

static bool SetAngle(int key, int slot, float x, float y, float z, const char* name, const char* model);

static bool SetPlayerAngle(int slot, float x, float y, float z);

static bool SetEntityAngle(int key, float x, float y, float z, const char* name, const char* model);

static char* GetCoords(int key, int slot);

static char* GetPlayerCoords(int slot);

static char* GetEntityCoords(int key);

static bool SetPlayerCoords(int slot, float x, float y, float z);

static bool SetEntityCoords(int key, float x, float y, float z);

static bool SetCoords(int key, int slot, float x, float y, float z);

#endif //RESOURCEMOD_ENTITY_H
