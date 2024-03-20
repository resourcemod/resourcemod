//
// Created by Twelvee on 21.03.2024.
//
#include "Weapon.h"
#include "../cs2/cbaseplayercontroller.h"
#include "../cs2/ccsplayercontroller.h"

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
        "weapon_m4a1_silencer",
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

bool Give(int slot, char* weapon) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    if (!c->GetPawn()) {
        return false;
    }
    CCSPlayer_ItemServices *itemServices = c->GetPawn()->m_pItemServices();
    if (!itemServices) {
        return false;
    }

    std::string name = weapon;
    if (std::find(weapons.begin(), weapons.end(), name) != weapons.end()) {
        itemServices->GiveNamedItem(name.c_str());
        return true;
    }
    return false;
}

const char *Get(int slot, int gearSlot) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    if (!c->GetPawn()) {
        return nullptr;
    }

    CPlayer_WeaponServices *weaponServices = c->GetPawn()->m_pWeaponServices();
    if (!weaponServices)
        return nullptr;

    if (gearSlot == GEAR_SLOT_CURRENT_WEAPON) {
        CBasePlayerWeapon *weapon = weaponServices->m_hActiveWeapon();
        if (!weapon)
            return nullptr;

        return weapon->GetClassname();
    }

    CUtlVector<CHandle<CBasePlayerWeapon>> *weapons = weaponServices->m_hMyWeapons();
    if (!weapons)
        return nullptr;

    FOR_EACH_VEC(*weapons, i) {
        CHandle<CBasePlayerWeapon> &weaponHandle = (*weapons)[i];
        if (!weaponHandle.IsValid())
            continue;

        CBasePlayerWeapon *weapon = weaponHandle.Get();
        if (!weapon)
            continue;

        if (weapon->GetWeaponVData()->m_GearSlot() == gearSlot) {
            return weapon->GetClassname();
        }
    }

    return nullptr;
}

bool Drop(int slot, bool remove) {
    CCSPlayerController *player = CCSPlayerController::FromSlot(slot);

    CCSPlayerPawn *pawn = player->GetPawn();
    if (!pawn)
        return false;

    CPlayer_WeaponServices *weaponServices = pawn->m_pWeaponServices();
    if (!weaponServices)
        return false;

    CBasePlayerWeapon *weapon = weaponServices->m_hActiveWeapon();
    if (!weapon)
        return false;

    CCSPlayer_ItemServices *itemServices = pawn->m_pItemServices();
    if (!itemServices) {
        return false;
    }

    itemServices->DropPlayerWeapon(weapon);
    if (remove) {
        weaponServices->RemoveWeapon(weapon);
        weapon->Remove();
    }

    return true;
}