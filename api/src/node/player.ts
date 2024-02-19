// @ts-ignore
import { metacall } from "metacall"
import { sayToSlot } from "./chat"
import { STEAM_USER_HIGH_VALUE, GameMessageTarget, Team } from "./constants"
import { Color } from "./color";
import { GearSlot, Weapon, drop, getWeaponFromGearSlot, give } from "../game/weapons";

export class Player {
    private readonly _name: string;
    private readonly _steamId: number;
    private readonly _slot: number;

    constructor(name: string, steamId: number, slot: number) {
        this._name = name;
        this._steamId = steamId;
        this._slot = slot;
    }

    get hp(): number {
        return metacall('_PlayerGetHP', this._slot) as number;
    }

    set hp(hp: number) {
        if (typeof hp !== 'number') {
            hp = Number(hp)
        }
        metacall('_PlayerSetHP', this._slot, hp);
    }

    get armor(): number {
        return metacall('_PlayerGetArmor', this._slot) as number;
    }

    set armor(hp: number) {
        if (typeof hp !== 'number') {
            hp = Number(hp)
        }
        metacall('_PlayerSetArmor', this._slot, hp);
    }

    get money(): number {
        return metacall('_PlayerGetMoney', this._slot) as number;
    }

    set money(amount: number) {
        if (typeof amount !== 'number') {
            amount = Number(amount)
        }
        metacall('_PlayerSetMoney', this._slot, amount);
    }

    get stats(): Stats {
        return new Stats(
            this._slot,
            metacall('_PlayerGetDamage', this._slot),
            metacall('_PlayerGetKills', this._slot),
            metacall('_PlayerGetAssists', this._slot),
            metacall('_PlayerGetDeaths', this._slot)
        )
    }

    set stats(stats: Stats) {
        metacall('_PlayerSetDamage', this._slot, stats.damage)
        metacall('_PlayerSetKills', this._slot, stats.kills)
        metacall('_PlayerSetAssists', this._slot, stats.assists)
        metacall('_PlayerSetDeaths', this._slot, stats.deaths)
    }

    get name() {
        return this._name;
    }

    get slot() {
        return this._slot;
    }

    get steamId() {
        return this._steamId === 0 ? undefined : this._steamId;
    }

    get steamId64() {
        if (!this.steamId) return 0; // bots
        return (BigInt(this.steamId) + STEAM_USER_HIGH_VALUE).toString();
    }

    get isBot() {
        return !this.steamId;
    }

    get isAlive() {
        return metacall('_PlayerGetIsAlive', this._slot) as boolean;
    }

    get isConnected() {
        return metacall('_PlayerGetIsConnected', this._slot) as boolean;
    }

    get isDisconnected() {
        return metacall('_PlayerGetIsDisconnected', this._slot) as boolean;
    }

    get isConnecting() {
        return metacall('_PlayerGetIsConnecting', this._slot) as boolean;
    }

    get isDisconnecting() {
        return metacall('_PlayerGetIsDisconnecting', this._slot) as boolean;
    }

    get isReserved() {
        return metacall('_PlayerGetIsReserved', this._slot) as boolean;
    }

    get isReconnecting() {
        return metacall('_PlayerGetIsReconnecting', this._slot) as boolean;
    }

    get currentWeapon() {
        return getWeaponFromGearSlot(this._slot, GearSlot.CurrentWeapon)
    }

    slap(hp: number) {
        metacall('_PlayerSlap', this._slot, hp);
    }

    slay() {
        metacall('_PlayerSlay', this._slot);
    }

    giveWeapon(weapon: Weapon) {
        return give(this._slot, weapon)
    }

    dropWeapon(remove: boolean = false) {
        return drop(this._slot, remove)
    }

    respawn() {
        metacall('_PlayerRespawn', this._slot);
    }

    get team() {
        return metacall('_PlayerGetTeam', this._slot) as number;
    }

    changeTeam(team: Team, kill: boolean) {
        metacall('_PlayerChangeTeam', this._slot, team, kill);
    }

    say(message: string) {
        sayToSlot(this._slot, message);
    }

    hint(message: string) {
        sayToSlot(this._slot, message, GameMessageTarget.Hint);
    }

    setModel(path: string) {
        metacall('_PlayerSetModel', this._slot, path);
    }

    setColor(color: Color) {
        metacall('_PlayerSetColor', this._slot, color);
    }

    playSound(path: string) {
        metacall('_PlayerPlaySound', this._slot, path);
    }

    kick() {
        metacall('_PlayerKick', this._slot);
    }
}

export class Stats {
    constructor(
        private readonly _slot: number,
        private readonly _damage: number,
        private readonly _kills: number,
        private readonly _assists: number,
        private readonly _deaths: number
    ){}

    get damage(): number {
        return this._damage
    }

    get kills(): number {
        return this._kills
    }

    get assists(): number {
        return this._assists
    }

    get deaths(): number {
        return this._deaths
    }

    set damage(amount: number) {
        metacall('_PlayerSetDamage', this._slot, amount)
    }

    set kills(amount: number) {
        metacall('_PlayerSetKills', this._slot, amount)
    }

    set assists(amount: number) {
        metacall('_PlayerSetAssists', this._slot, amount)
    }

    set deaths(amount: number) {
        metacall('_PlayerSetDeaths', this._slot, amount)
    }
}

export const _Player = (name: string, steamId: number, slot: number) => {
    return new Player(name, steamId, slot)
}