//
// Created by Twelvee on 13.11.2023.
//

#ifndef RESOURCEMOD_ENTITY_H
#define RESOURCEMOD_ENTITY_H

#ifdef __cplusplus
extern "C" {
#endif

extern int Create(const char* type);

extern bool Spawn(int key, const char* model);

extern bool Remove(int key);

extern bool SetModel(int key, const char* model);

extern int GetCollision(int key);

extern bool SetCollision(int key, int type);

extern bool SetColor(int key, int red, int green, int blue, int alpha);

extern char* GetColor(int key);

extern char* GetAngle(int key, int slot);

extern char* GetPlayerAngle(int slot);

extern char* GetEntityAngle(int key);

extern bool SetAngle(int key, int slot, float x, float y, float z, const char* name, const char* model);

extern bool SetPlayerAngle(int slot, float x, float y, float z);

extern bool SetEntityAngle(int key, float x, float y, float z, const char* name, const char* model);

extern char* GetCoords(int key, int slot);

extern char* GetPlayerCoords(int slot);

extern char* GetEntityCoords(int key);

extern bool SetPlayerCoords(int slot, float x, float y, float z);

extern bool SetEntityCoords(int key, float x, float y, float z);

extern bool SetCoords(int key, int slot, float x, float y, float z);

#ifdef __cplusplus
}
#endif

#endif //RESOURCEMOD_ENTITY_H
