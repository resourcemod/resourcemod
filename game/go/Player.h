//
// Created by Twelvee on 13.11.2023.
//

#ifndef RESOURCEMOD_PLAYER_H
#define RESOURCEMOD_PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif

extern int GetHP(int slot);

extern bool SetHP(int slot, int hp);

extern int GetMoney(int slot);

extern bool SetMoney(int slot, int money);

extern int GetDamage(int slot);

extern int GetKills(int slot);

extern int GetAssists(int slot);

extern int GetDeaths(int slot);

extern bool SetDamage(int slot, int damage);

extern bool SetKills(int slot, int kills);

extern bool SetAssists(int slot, int assists);

extern bool SetDeaths(int slot, int deaths);

extern int GetArmor(int slot);

extern bool SetArmor(int slot, int armor);

extern const char *GetName(int slot);

extern long GetSteamID(int slot);

extern bool GetIsAlive(int slot);

extern bool Slap(int slot, int damage);

extern bool Slay(int slot);

extern bool Respawn(int slot);

extern bool PrintGameMessage(int slot, char *message, int duration);

extern bool Play(int slot, char *sound);

extern bool Kick(int slot);

extern int GetTeam(int slot);

extern int ChangeTeam(int slot, int team, bool kill);

extern bool GetIsConnected(int slot);

extern bool GetIsConnecting(int slot);

extern bool GetIsDisconnected(int slot);

extern bool GetIsDisconnecting(int slot);

extern bool GetIsReserved(int slot);

extern bool GetIsReconnecting(int slot);

extern bool PrintAll(int dest, const char* msg);

extern bool Print(int slot, int dest, const char* msg);

#ifdef __cplusplus
}
#endif

#endif //RESOURCEMOD_PLAYER_H
