//
// Created by Twelvee on 13.11.2023.
//

#ifndef RESOURCEMOD_WEAPON_H
#define RESOURCEMOD_WEAPON_H

#ifdef __cplusplus
extern "C" {
#endif

extern bool Give(int slot, char *weapon);

extern const char *Get(int slot, int gearSlot);

extern bool Drop(int slot, bool remove);

#ifdef __cplusplus
}
#endif

#endif //RESOURCEMOD_WEAPON_H
