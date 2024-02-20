// @ts-ignore
import {metacall} from "metacall"

export enum Weapon {
    Ak47 = "weapon_ak47",
    Aug = "weapon_aug",
    Awp = "weapon_awp",
    Bizon = "weapon_bizon",
    C4 = "weapon_c4",
    Deagle = "weapon_deagle",
    Decoy = "weapon_decoy",
    Elite = "weapon_elite",
    Famas = "weapon_famas",
    FiveSeven = "weapon_fiveseven",
    Flash = "weapon_flashbang",
    G3SG1 = "weapon_g3sg1",
    Galilar = "weapon_galilar",
    Glock = "weapon_glock",
    HealShot = "weapon_healthshot",
    HEGrenade = "weapon_hegrenade",
    HKP2000 = "weapon_hkp2000",
    IncGrenade = "weapon_incgrenade",
    Knife = "weapon_knife",
    M249 = "weapon_m249",
    M4A1 = "weapon_m4a1_silencer",
    M4A4 = "weapon_m4a1",
    Mac10 = "weapon_mac10",
    Mag7 = "weapon_mag7",
    Molotov = "weapon_molotov",
    Mp7 = "weapon_mp7",
    Negev = "weapon_negev",
    Nova = "weapon_nova",
    P250 = "weapon_p250",
    P90 = "weapon_p90",
    SawedOff = "weapon_sawedoff",
    SCAR20 = "weapon_scar20",
    SG556 = "weapon_sg556",
    Smoke = "weapon_smokegrenade",
    SSG08 = "weapon_ssg08",
    Taser = "weapon_taser",
    Tec9 = "weapon_tec9",
    UMP45 = "weapon_ump45",
    XM1014 = "weapon_xm1014",
}

export enum GearSlot {
    Rifle = 0,
    Pistol = 1,
    Knife = 2,
    Grenades = 3,
    C4 = 4,
    ReservedSlot6 = 5,
    ReservedSlot7 = 6,
    ReservedSlot8 = 7,
    ReservedSlot9 = 8,
    ReservedSlot10 = 9,
    ReservedSlot11 = 10,
    Boosts = 11,
    Utility = 12,
    Count = 13,
    CurrentWeapon = 14,
    First = 15,
    Last = 16
}

export const give = (slot: number, weapon: Weapon) : boolean => {
    return metacall('_WeaponGive', slot, weapon)
}

export const getWeaponFromGearSlot = (slot: number, gearSlot: GearSlot) => {
    const name = metacall('_WeaponGet', slot, gearSlot);
    if (typeof name === 'string') {
        return name as Weapon
    }
    return null
}

export const drop = (slot: number, remove: boolean = false) : boolean => {
    return metacall('_WeaponDrop', slot, remove)
}
