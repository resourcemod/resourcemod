//
// Created by Twelvee on 13.11.2023.
//

#ifndef RESOURCEMOD_WEAPON_H
#define RESOURCEMOD_WEAPON_H

#include "../cs2/cbaseplayercontroller.h"
#include "../cs2/ccsplayercontroller.h"
#include <metacall/metacall.h>
#include <vector>

std::vector<std::string> weapons = {
        "weapon_ak47",
        "weapon_aug",
        "weapon_awp",
        "weapon_bizon",
        "weapon_c4",
        "weapon_deagle",
        "weapon_decoy",
        "weapon_elite",
        "weapon_famas",
        "weapon_fiveseven",
        "weapon_flashbang",
        "weapon_g3sg1",
        "weapon_galilar",
        "weapon_glock",
        "weapon_healthshot",
        "weapon_hegrenade",
        "weapon_hkp2000",
        "weapon_incgrenade",
        "weapon_knife",
        "weapon_m249",
        "weapon_m4a1",
        "weapon_mac10",
        "weapon_mag7",
        "weapon_molotov",
        "weapon_mp7",
        "weapon_mp9",
        "weapon_negev",
        "weapon_nova",
        "weapon_p250",
        "weapon_p90",
        "weapon_sawedoff",
        "weapon_scar20",
        "weapon_sg556",
        "weapon_smokegrenade",
        "weapon_ssg08",
        "weapon_taser",
        "weapon_tec9",
        "weapon_ump45",
        "weapon_xm1014"
};

class Weapon {
public:
    static void *Give(size_t argc, void *args[], void *data) {
        CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
        if (!c->GetPawn()) {
            return metacall_value_create_bool(false);
        }
        CCSPlayer_ItemServices *itemServices = c->GetPawn()->m_pItemServices();
        if (!itemServices) {
            return metacall_value_create_bool(false);
        }

        std::string name = metacall_value_to_string(args[1]);
        if (std::find(weapons.begin(), weapons.end(), name) != weapons.end()) {
            itemServices->GiveNamedItem(name.c_str());
            return metacall_value_create_bool(true);
        }
        return metacall_value_create_bool(false);
    }

    static void *Get(size_t argc, void *args[], void *data) {
        CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
        uint32_t gearSlot = (uint32) metacall_value_to_int(args[1]);
        if (!c->GetPawn()) {
            return metacall_value_create_null();
        }

        CPlayer_WeaponServices *weaponServices = c->GetPawn()->m_pWeaponServices();
        if (!weaponServices)
            return metacall_value_create_null();

        if (gearSlot == GEAR_SLOT_CURRENT_WEAPON) {
            CBasePlayerWeapon *weapon = weaponServices->m_hActiveWeapon();
            if (!weapon)
                return metacall_value_create_null();

            return metacall_value_create_string(weapon->GetClassname(), strlen(weapon->GetClassname()));
        }

        CUtlVector<CHandle<CBasePlayerWeapon>> *weapons = weaponServices->m_hMyWeapons();
        if (!weapons)
            return metacall_value_create_null();

        FOR_EACH_VEC(*weapons, i) {
            CHandle<CBasePlayerWeapon> &weaponHandle = (*weapons)[i];
            if (!weaponHandle.IsValid())
                continue;

            CBasePlayerWeapon *weapon = weaponHandle.Get();
            if (!weapon)
                continue;

            if (weapon->GetWeaponVData()->m_GearSlot() == gearSlot) {
                std::string w = weapon->GetClassname();
                return metacall_value_create_string(w.c_str(), w.length());
            }
        }

        return metacall_value_create_null();
    }

    static void *Drop(size_t argc, void *args[], void *data) {
        CCSPlayerController *player = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));

        CCSPlayerPawn *pawn = player->GetPawn();
        if (!pawn)
            return metacall_value_create_bool(false);

        CPlayer_WeaponServices *weaponServices = pawn->m_pWeaponServices();
        if (!weaponServices)
            return metacall_value_create_bool(false);

        CBasePlayerWeapon *weapon = weaponServices->m_hActiveWeapon();
        if (!weapon)
            return metacall_value_create_null();

        CCSPlayer_ItemServices *itemServices = pawn->m_pItemServices();
        if (!itemServices) {
            return metacall_value_create_bool(false);
        }

        itemServices->DropPlayerWeapon(weapon);

        metacall_value_create_bool(true);
    }
};

#endif //RESOURCEMOD_WEAPON_H
