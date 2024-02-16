// @ts-ignore
import { metacall } from "metacall"
import { sayToSlot } from "./chat"
import { STEAM_USER_HIGH_VALUE, GameMessageTarget, Team } from "./constants"
import { Color } from "./color";

export class Player {
    constructor(
        public readonly name: string,
        public readonly steamId: number,
        public readonly slot: number
    ) {

    }

    get hp() {
        return metacall('_PlayerGetHP', this.slot) as number;
    }

    set hp(hp: number) {
        metacall('_PlayerSetHP', this.slot, hp);
    }

    get steamId64(): string | undefined {
        if (!this.steamId) return; // bot
        return (BigInt(this.steamId) + STEAM_USER_HIGH_VALUE).toString();
    }

    get isBot() {
        return !this.steamId;
    }

    get isAlive() {
        return metacall('_PlayerGetIsAlive', this.slot) as boolean;
    }

    get isConnected() {
        return metacall('_PlayerGetIsConnected', this.slot) as boolean;
    }

    get isDisconnected() {
        return metacall('_PlayerGetIsDisconnected', this.slot) as boolean;
    }

    get isConnecting() {
        return metacall('_PlayerGetIsConnecting', this.slot) as boolean;
    }

    get isDisconnecting() {
        return metacall('_PlayerGetIsDisconnecting', this.slot) as boolean;
    }

    get isReserved() {
        return metacall('_PlayerGetIsReserved', this.slot) as boolean;
    }

    get isReconnecting() {
        return metacall('_PlayerGetIsReconnecting', this.slot) as boolean;
    }

    slap(hp: number) {
        metacall('_PlayerSlap', this.slot, hp);
    }

    slay() {
        metacall('_PlayerSlay', this.slot);
    }

    respawn() {
        metacall('_PlayerRespawn', this.slot);
    }

    get team(): Team {
        return metacall('_PlayerGetTeam', this.slot);
    }

    changeTeam(team: Team, kill: boolean) {
        metacall('_PlayerChangeTeam', this.slot, team, kill);
    }

    say(message: string) {
        sayToSlot(this.slot, message);
    }

    hint(message: string) {
        sayToSlot(this.slot, message, GameMessageTarget.Hint);
    }

    setModel(path: string) {
        metacall('_PlayerSetModel', this.slot, path);
    }

    setColor(color: Color) {
        metacall('_PlayerSetColor', this.slot, color);
    }

    playSound(path: string) {
        metacall('_PlayerPlaySound', this.slot, path);
    }

    kick() {
        metacall('_PlayerKick', this.slot);
    }
}

export const _Player = (name: string, steamId: number, slot: number) => {
    return new Player(name, steamId, slot)
}