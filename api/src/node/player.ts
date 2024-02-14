// @ts-ignore
import { metacall } from "metacall"
import { sayToSlot } from "./chat"
import { HUD_PRINT_CENTER, STEAM_USER_HIGH_VALUE } from "./constants"

export class Player {
    private readonly _name: string;
    private readonly _steamId: string;
    private readonly _slot: number;

    constructor(name: string, steamId: string, slot: number) {
        this._name = name;
        this._steamId = steamId;
        this._slot = slot;
    }

    get hp() {
        return metacall('_PlayerGetHP', this._slot) as number;
    }

    set hp(hp: number) {
        metacall('_PlayerSetHP', this._slot, hp);
    }

    get name() {
        return this._name;
    }

    get slot() {
        return this._slot;
    }

    get steamId() {
        return this._steamId === "0" ? undefined : this._steamId;
    }

    get steamId64() {
        if (!this.steamId) return; // bots
        return (BigInt(this.steamId) + STEAM_USER_HIGH_VALUE).toString();
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

    slap(hp: number) {
        metacall('_PlayerSlap', this._slot, hp);
    }

    slay() {
        metacall('_PlayerSlay', this._slot);
    }

    respawn() {
        metacall('_PlayerRespawn', this._slot);
    }

    get team() {
        return metacall('_PlayerGetTeam', this._slot) as number;
    }

    changeTeam(team: number, kill: boolean) {
        metacall('_PlayerChangeTeam', this._slot, team, kill);
    }

    say(message: string) {
        sayToSlot(this._slot, message);
    }

    hint(message: string) {
        sayToSlot(this._slot, message, HUD_PRINT_CENTER);
    }

    setModel(path: string) {
        metacall('_PlayerSetModel', this._slot, path);
    }

    setColor(color: string) {
        metacall('_PlayerSetColor', this._slot, color);
    }

    playSound(path: string) {
        metacall('_PlayerPlaySound', this._slot, path);
    }

    kick() {
        metacall('_PlayerKick', this._slot);
    }
}

export const _Player = (name: string, steamId: string, slot: number) => {
    return new Player(name, steamId, slot)
}

export default {
    Player,
    _Player
}