import { Player } from './player';
import { PREVENT_EVENT, STEAM_USER_HIGH_VALUE } from "./constants"
const events = new Map();

type IEvents = {
    item_pickup: (event: ItemPickupEvent) => void;
    client_put_in_server: (event: ClientPutInServerEvent) => void;
    client_connected: (event: ClientConnectedEvent) => void;
    client_connect: (event: ClientConnectEvent) => void;
    player_activate: (event: PlayerActivateEvent) => void;
    player_spawn: (event: PlayerSpawnEvent) => void;
    player_team: (event: PlayerChangeTeamEvent) => void;
    player_hurt: (event: PlayerHurtEvent) => void;
    map_shutdown: (event: MapShutdownEvent) => void;
    player_chat: (event: PlayerChatEvent) => void;
    game_message: (event: GameMessageEvent) => void;
    map_loaded: (event: MapLoadedEvent) => void;
    round_end: (event: RoundEndEvent) => void;
    round_start: (event: RoundStartEvent) => void;
    round_freeze_end: (event: FreezeTimeEndedEvent) => void;
    player_death: (event: PlayerDeathEvent) => void;
    player_footstep: (event: PlayerFootstepEvent) => void;
    break_prop: (event: BreakPropEvent) => void;
    item_purchase: (event: ItemPurchaseEvent) => void;
    bomb_beginplant: (event: BombBeginPlantEvent) => void;
    bomb_planted: (event: BombPlantedEvent) => void;
    bomb_defused: (event: BombDefusedEvent) => void;
    bomb_exploded: (event: BombExplodedEvent) => void;
    bomb_dropped: (event: BombDroppedEvent) => void;
    bomb_pickup: (event: BombPickedUpEvent) => void;
    defuser_dropped: (event: DefuserDroppedEvent) => void;
    defuser_pickup: (event: DefuserPickupEvent) => void;
    bomb_begindefuse: (event: BeginDefuseEvent) => void;
    bomb_abortdefuse: (event: AbortDefuseEvent) => void;
    hostage_follows: (event: HostageBeginsFollowingEvent) => void;
    hostage_hurt: (event: HostageHurtEvent) => void;
    hostage_rescued: (event: HostageRescuedEvent) => void;
    hostage_stops_following: (event: HostageStopsFollowingEvent) => void;
    weapon_fire: (event: WeaponFireEvent) => void;
    weapon_reload: (event: WeaponReloadEvent) => void;
    weapon_zoom: (event: WeaponZoomEvent) => void;
    player_blind: (event: PlayerBlindEvent) => void;
    client_disconnected: (event: ClientDisconnectedEvent) => void;
}

// actual source engine callback
export const _onEventCall = (data: any) => {
    if (!data._name) {
        return "undefined_event";
    }
    let prevent = "event_no_prevent";
    if (events.has(data._name)) {
        // temporary hack (cuz classes works weird)
        try {
            // @ts-expect-error
            data = Object.assign(new constructors[data._name], data);
            if (data._player) data._player = new Player(data._player._name, data._player._steamId, data._player._slot)
            if (data._attacker) data._attacker = new Player(data._attacker._name, data._attacker._steamId, data._attacker._slot)
            if (data._assister) data._assister = new Player(data._assister._name, data._assister._steamId, data._assister._slot)
        } catch (e: any) {
            console.error(e)
        }
        events.get(data._name).forEach((listener: Function) => {
            if (typeof listener === 'function') {
                try {
                    if (listener(data) === PREVENT_EVENT) {
                        prevent = PREVENT_EVENT
                    }
                } catch (e: any) {
                    console.error(e)
                }
            }
        });
    }
    return prevent;
}
export function onEvent<T extends keyof IEvents>(name: T, callback: IEvents[T]) {
    if (typeof callback !== 'function') throw 'Callback must be a function.'

    if (events.has(name) && Array.isArray(events.get(name))) {
        return events.get(name).push(callback)
    }
    events.set(name, [callback])
}

export const onItemPickup = (callback: IEvents['item_pickup']) => onEvent('item_pickup', callback)
export const onClientPutInServer = (callback: IEvents['client_put_in_server']) => onEvent('client_put_in_server', callback)
export const onClientConnected = (callback: IEvents['client_connected']) => onEvent('client_connected', callback)
export const onClientConnect = (callback: IEvents['client_connect']) => onEvent('client_connect', callback)
export const onPlayerActivate = (callback: IEvents['player_activate']) => onEvent('player_activate', callback)
export const onPlayerSpawn = (callback: IEvents['player_spawn']) => onEvent('player_spawn', callback)
export const onPlayerChangeTeam = (callback: IEvents['player_team']) => onEvent('player_team', callback)
export const onPlayerHurt = (callback: IEvents['player_hurt']) => onEvent('player_hurt', callback)
export const onMapShutdown = (callback: IEvents['map_shutdown']) => onEvent('map_shutdown', callback)
export const onPlayerChat = (callback: IEvents['player_chat']) => onEvent('player_chat', callback)
export const onGameMessage = (callback: IEvents['game_message']) => onEvent('game_message', callback)
export const onMapLoaded = (callback: IEvents['map_loaded']) => onEvent('map_loaded', callback)
export const onRoundEnded = (callback: IEvents['round_end']) => onEvent('round_end', callback)
export const onRoundStarted = (callback: IEvents['round_start']) => onEvent('round_start', callback)
export const onFreezeTimeEnded = (callback: IEvents['round_freeze_end']) => onEvent('round_freeze_end', callback)
export const onPlayerDeath = (callback: IEvents['player_death']) => onEvent('player_death', callback)
export const onPlayerFootstep = (callback: IEvents['player_footstep']) => onEvent('player_footstep', callback)
export const onPropBreak = (callback: IEvents['break_prop']) => onEvent('break_prop', callback)
export const onItemPurchase = (callback: IEvents['item_purchase']) => onEvent('item_purchase', callback)
export const onBombBeginPlant = (callback: IEvents['bomb_beginplant']) => onEvent('bomb_beginplant', callback)
export const onBombPlanted = (callback: IEvents['bomb_planted']) => onEvent('bomb_planted', callback)
export const onBombDefused = (callback: IEvents['bomb_defused']) => onEvent('bomb_defused', callback)
export const onBombExploded = (callback: IEvents['bomb_exploded']) => onEvent('bomb_exploded', callback)
export const onBombDropped = (callback: IEvents['bomb_dropped']) => onEvent('bomb_dropped', callback)
export const onBombPickup = (callback: IEvents['bomb_pickup']) => onEvent('bomb_pickup', callback)
export const onDefuserDropped = (callback: IEvents['defuser_dropped']) => onEvent('defuser_dropped', callback)
export const onDefuserPickup = (callback: IEvents['defuser_pickup']) => onEvent('defuser_pickup', callback)
export const onBeginDefuse = (callback: IEvents['bomb_begindefuse']) => onEvent('bomb_begindefuse', callback)
export const onAbortDefuse = (callback: IEvents['bomb_abortdefuse']) => onEvent('bomb_abortdefuse', callback)
export const onHostageFollows = (callback: IEvents['hostage_follows']) => onEvent('hostage_follows', callback)
export const onHostageHurt = (callback: IEvents['hostage_hurt']) => onEvent('hostage_hurt', callback)
export const onHostageRescued = (callback: IEvents['hostage_rescued']) => onEvent('hostage_rescued', callback)
export const onHostageStopsFollowing = (callback: IEvents['hostage_stops_following']) => onEvent('hostage_stops_following', callback)
export const onWeaponFire = (callback: IEvents['weapon_fire']) => onEvent('weapon_fire', callback)
export const onWeaponReload = (callback: IEvents['weapon_reload']) => onEvent('weapon_reload', callback)
export const onWeaponZoom = (callback: IEvents['weapon_zoom']) => onEvent('weapon_zoom', callback)
export const onPlayerBlind = (callback: IEvents['player_blind']) => onEvent('player_blind', callback)
export const onClientDisconnected = (callback: IEvents['client_disconnected']) => onEvent('client_disconnected', callback)

// Event classes
export class ItemPickupEvent {
    constructor(
        public readonly name: string,
        public readonly item: string,
        public readonly silent: boolean,
        public readonly defindex: number,
        public readonly player: Player
    ) {

    }
}
// Base class for equal or semi-equal
export class ClientPutInServerEvent {
    constructor(
        public readonly name: string,
        public readonly player: Player,
    ) {

    }
}

export class ClientDisconnectedEvent {
    constructor(
        public readonly name: string,
        public readonly player: Player,
    ) {

    }
}

export class ClientConnectEvent {
    constructor(
        public readonly name: string,
        public readonly steamId: number
    ) {

    }

    get steamId64(): string | undefined {
        if (!this.steamId) return; // bot
        return (BigInt(this.steamId) + STEAM_USER_HIGH_VALUE).toString();
    }
}

export class ClientConnectedEvent extends ClientConnectEvent {
    constructor(
        name: string,
        steamId: number,
        public readonly ip: string,
        public readonly isBot: boolean) {
        super(name, steamId)
    }
}

export class PlayerActivateEvent {
    constructor(
        public readonly name: string,
        public readonly player: Player,
    ) {

    }
}

export class PlayerSpawnEvent {
    constructor(
        public readonly name: string,
        public readonly player: Player,
    ) {

    }
}

export class PlayerChangeTeamEvent {
    constructor(
        public readonly name: string,
        public readonly team: number,
        public readonly oldTeam: number,
        public readonly becauseDisconnected: boolean,
        public readonly silent: boolean,
        public readonly player: Player
    ) {

    }
}

export class PlayerHurtEvent {
    constructor(
        public readonly name: string,
        public readonly hp: number,
        public readonly armor: number,
        public readonly weapon: string,
        public readonly damageHp: number,
        public readonly damageArmor: number,
        public readonly hitGroup: number,
        public readonly player: Player,
        public readonly attacker: Player
    ) {
    }

    get damage() {
        return {
            hp: this.damageHp,
            armor: this.damageArmor
        }
    }
}

export class MapShutdownEvent {
    constructor(
        public readonly name: string
    ) {
    }
}

export class PlayerChatEvent {
    constructor(
        public readonly name: string,
        public readonly message: string,
        public readonly teamOnly: boolean,
        public readonly player: Player
    ) {

    }
}

export class GameMessageEvent {
    constructor(
        public readonly name: string,
        public readonly message: string,
        public readonly target: string
    ) {
    }
}

export class MapLoadedEvent {
    constructor(
        public readonly name: string,
        public readonly map: string,
        public readonly oldMap: string
    ) {

    }
}

export class RoundEndEvent {
    constructor(
        public readonly name: string,
        public readonly winner: number,
        public readonly reason: number,
        public readonly message: string,
        public readonly legacy: number,
        public readonly playerCount: number,
        public readonly noMusic: number
    ) {

    }

    get isMusicPlaying() {
        return !this.noMusic
    }
}

export class RoundStartEvent {
    constructor(
        public readonly name: string,
        public readonly timeLimit: number,
        public readonly fragLimit: number,
        public readonly objective: string
    ) {
    }
}

export class FreezeTimeEndedEvent {
    constructor(
        public readonly name: string
    ) {
    }
}

export class PlayerDeathEvent {
    constructor(
        public readonly name: string,
        public readonly assistFlash: boolean,
        public readonly weapon: string,
        public readonly weaponItemId: string,
        public readonly weaponFauxItemId: string,
        public readonly weaponOriginalOwnerSteamId: string,
        public readonly headshot: boolean,
        public readonly dominated: number,
        public readonly revenge: number,
        public readonly wipe: number,
        public readonly penetrated: number,
        public readonly noReplay: boolean,
        public readonly noScope: boolean,
        public readonly throughSmoke: boolean,
        public readonly attackerBlind: boolean,
        public readonly distance: number,
        public readonly player: Player,
        public readonly attacker: Player,
        public readonly assister: Player
    ) {

    }

    get weaponDetails() {
        return {
            name: this.weapon,
            itemId: this.weaponItemId,
            fauxId: this.weaponFauxItemId,
            originalOwnerSteamId: this.weaponOriginalOwnerSteamId,
            // todo: move this function to external helper (steamid64(steamid: number): string | number). Then developers could get steamid64 from steamid if they want by just importing helper from resourcemod lib
            originalOwnerSteamId64: function () {
                if (!this.originalOwnerSteamId) return this.originalOwnerSteamId;
                return (BigInt(this.originalOwnerSteamId) + STEAM_USER_HIGH_VALUE).toString();
            }
        }
    }
}

export class PlayerFootstepEvent {
    constructor(
        public readonly name: string,
        public readonly player: Player
    ) {
    }
}

export class BreakPropEvent {
    constructor(
        public readonly name: string,
        public readonly entityId: number,
        public readonly player: Player
    ) {
    }
}

export class ItemPurchaseEvent {
    constructor(
        public readonly name: string,
        public readonly team: number,
        public readonly loadout: number,
        public readonly weapon: string,
        public readonly player: Player
    ) {

    }
}

export class BombBeginPlantEvent {
    constructor(
        public readonly name: string,
        public readonly site: number,
        public readonly player: Player
    ) {

    }
}

export class BombPlantedEvent {
    constructor(
        public readonly name: string,
        public readonly site: number,
        public readonly player: Player
    ) {

    }
}

export class BombDefusedEvent {
    constructor(
        public readonly name: string,
        public readonly site: number,
        public readonly player: Player
    ) {

    }
}

export class BombExplodedEvent {
    constructor(
        public readonly name: string,
        public readonly site: number,
        public readonly player: Player
    ) {

    }
}

export class BombDroppedEvent {
    constructor(
        public readonly name: string,
        public readonly entityId: number,
        public readonly player: Player
    ) {

    }
}

export class BombPickedUpEvent {
    constructor(
        public readonly name: string,
        public readonly player: Player,
    ) {

    }
}

export class DefuserDroppedEvent {
    constructor(
        public readonly name: string,
        public readonly entityId: number,
    ) {

    }
}

export class DefuserPickupEvent {
    constructor(
        public readonly name: string,
        public readonly entityId: number,
        public readonly player: Player
    ) {

    }
}

export class BeginDefuseEvent {
    constructor(
        public readonly name: string,
        public readonly hasKit: boolean,
        public readonly player: Player
    ) {

    }
}

export class AbortDefuseEvent {
    constructor(
        public readonly name: string,
        public readonly player: Player,
    ) {

    }
}

export class HostageBeginsFollowingEvent {
    constructor(
        public readonly name: string,
        public readonly hostage: number,
        public readonly player: Player,
    ) {

    }
}

export class HostageHurtEvent {
    constructor(
        public readonly name: string,
        public readonly hostage: number,
        public readonly player: Player,
    ) {

    }
}

export class HostageRescuedEvent {
    constructor(
        public readonly name: string,
        public readonly hostage: number,
        public readonly site: number,
        public readonly player: Player,
    ) {

    }
}

export class HostageStopsFollowingEvent {
    constructor(
        public readonly name: string,
        public readonly hostage: number,
        public readonly player: Player,
    ) {

    }
}

export class WeaponFireEvent {
    constructor(
        public readonly name: string,
        public readonly weapon: string,
        public readonly silenced: boolean,
        public readonly player: Player
    ) {
    }
}

export class WeaponReloadEvent {
    constructor(
        public readonly name: string,
        public readonly player: Player,
    ) {

    }
}

export class WeaponZoomEvent {
    constructor(
        public readonly name: string,
        public readonly player: Player,
    ) {

    }
}

export class PlayerBlindEvent {
    constructor(
        public readonly name: string,
        public readonly entityId: number,
        public readonly blindDuration: number,
        public readonly player: Player,
        public readonly attacker: Player
    ) {

    }
}

// Event constructors (metacall classes)
export const _ItemPickupEvent = (name: string, item: string, silent: boolean, defindex: number, player: Player) => {
    return new ItemPickupEvent(name, item, silent, defindex, player)
}

export const _ClientConnectedEvent = (name: string, steamId: number, ip: string, isBot: boolean) => {
    return new ClientConnectedEvent(name, steamId, ip, isBot)
}

export const _ClientConnectEvent = (name: string, steamId: number) => {
    return new ClientConnectEvent(name, steamId)
}

export const _ClientPutInServerEvent = (name: string, player: Player) => {
    return new ClientPutInServerEvent(name, player)
}

export const _ClientDisconnectedEvent = (name: string, player: Player) => {
    return new ClientDisconnectedEvent(name, player)
}

export const _PlayerActivateEvent = (name: string, player: Player) => {
    return new PlayerActivateEvent(name, player)
}

export const _PlayerSpawnEvent = (name: string, player: Player) => {
    return new PlayerSpawnEvent(name, player)
}

export const _PlayerChangeTeamEvent = (name: string, team: number, oldTeam: number, disconnect: boolean, silent: boolean, player: Player) => {
    return new PlayerChangeTeamEvent(name, team, oldTeam, disconnect, silent, player)
}

export const _PlayerHurtEvent = (name: string, hp: number, armor: number, weapon: string, damageHp: number, damageArmor: number, hitGoup: number, player: Player, attacker: Player) => {
    return new PlayerHurtEvent(name, hp, armor, weapon, damageHp, damageArmor, hitGoup, player, attacker)
}

export const _MapShutdownEvent = (name: string) => {
    return new MapShutdownEvent(name)
}

export const _PlayerChatEvent = (name: string, text: string, teamOnly: boolean, player: Player) => {
    return new PlayerChatEvent(name, text, teamOnly, player)
}

export const _GameMessageEvent = (name: string, text: string, target: string) => {
    return new GameMessageEvent(name, text, target)
}

export const _MapLoadedEvent = (name: string, map: string, oldMap: string) => {
    return new MapLoadedEvent(name, map, oldMap)
}

export const _RoundEndEvent = (name: string, winner: number, reason: number, message: string, legacy: number, playerCount: number, noMusic: number) => {
    return new RoundEndEvent(name, winner, reason, message, legacy, playerCount, noMusic)
}

export const _RoundStartEvent = (name: string, timeLimit: number, fragLimit: number, objective: string) => {
    return new RoundStartEvent(name, timeLimit, fragLimit, objective)
}

export const _FreezeTimeEndedEvent = (name: string) => {
    return new FreezeTimeEndedEvent(name)
}

export const _PlayerDeathEvent = (name: string, assistFlash: boolean, weapon: string, weaponItemId: string, weaponFauxItemId: string, weaponOriginalOwnerSteamId: string, headshot: boolean, dominated: number, revenge: number, wipe: number, penetrated: number, noReplay: boolean, noScope: boolean, throughSmoke: boolean, attackerBlind: boolean, distance: number, player: Player, attacker: Player, assister: Player) => {
    return new PlayerDeathEvent(name, assistFlash, weapon, weaponItemId, weaponFauxItemId, weaponOriginalOwnerSteamId, headshot, dominated, revenge, wipe, penetrated, noReplay, noScope, throughSmoke, attackerBlind, distance, player, attacker, assister)
}

export const _PlayerFootstepEvent = (name: string, player: Player) => {
    return new PlayerFootstepEvent(name, player)
}

export const _BreakPropEvent = (name: string, entityId: number, player: Player) => {
    return new BreakPropEvent(name, entityId, player)
}

export const _ItemPurchaseEvent = (name: string, team: number, loadout: number, weapon: string, player: Player) => {
    return new ItemPurchaseEvent(name, team, loadout, weapon, player)
}

export const _BombBeginPlantEvent = (name: string, site: number, player: Player) => {
    return new BombBeginPlantEvent(name, site, player)
}

export const _BombPlantedEvent = (name: string, site: number, player: Player) => {
    return new BombPlantedEvent(name, site, player)
}

export const _BombDefusedEvent = (name: string, site: number, player: Player) => {
    return new BombDefusedEvent(name, site, player)
}

export const _BombExplodedEvent = (name: string, site: number, player: Player) => {
    return new BombExplodedEvent(name, site, player)
}

export const _BombDroppedEvent = (name: string, entityId: number, player: Player) => {
    return new BombDroppedEvent(name, entityId, player)
}

export const _BombPickedUpEvent = (name: string, player: Player) => {
    return new BombPickedUpEvent(name, player)
}

export const _DefuserDroppedEvent = (name: string, entityId: number) => {
    return new DefuserDroppedEvent(name, entityId)
}

export const _DefuserPickupEvent = (name: string, entityId: number, player: Player) => {
    return new DefuserPickupEvent(name, entityId, player)
}

export const _BeginDefuseEvent = (name: string, hasKit: boolean, player: Player) => {
    return new BeginDefuseEvent(name, hasKit, player)
}

export const _AbortDefuseEvent = (name: string, player: Player) => {
    return new AbortDefuseEvent(name, player)
}

export const _HostageBeginsFollowingEvent = (name: string, hostage: number, player: Player) => {
    return new HostageBeginsFollowingEvent(name, hostage, player)
}

export const _HostageHurtEvent = (name: string, hostage: number, player: Player) => {
    return new HostageHurtEvent(name, hostage, player)
}

export const _HostageRescuedEvent = (name: string, hostage: number, site: number, player: Player) => {
    return new HostageRescuedEvent(name, hostage, site, player)
}

export const _HostageStopsFollowingEvent = (name: string, hostage: number, player: Player) => {
    return new HostageStopsFollowingEvent(name, hostage, player)
}

export const _WeaponFireEvent = (name: string, weapon: string, silenced: boolean, player: Player) => {
    return new WeaponFireEvent(name, weapon, silenced, player)
}

export const _WeaponReloadEvent = (name: string, player: Player) => {
    return new WeaponReloadEvent(name, player)
}

export const _WeaponZoomEvent = (name: string, player: Player) => {
    return new WeaponZoomEvent(name, player)
}

export const _PlayerBlindEvent = (name: string, entityId: number, blindDuration: number, player: Player, attacker: Player) => {
    return new PlayerBlindEvent(name, entityId, blindDuration, player, attacker)
}

// sadly we need this until metacall start working with javascript classes
// todo: remove this after metacall update
const constructors = {
    item_pickup: ItemPickupEvent,
    client_put_in_server: ClientPutInServerEvent,
    client_connected: ClientConnectedEvent,
    client_connect: ClientConnectEvent,
    player_activate: PlayerActivateEvent,
    player_spawn: PlayerSpawnEvent,
    player_team: PlayerChangeTeamEvent,
    player_hurt: PlayerHurtEvent,
    map_shutdown: MapShutdownEvent,
    player_chat: PlayerChatEvent,
    game_message: GameMessageEvent,
    map_loaded: MapLoadedEvent,
    round_end: RoundEndEvent,
    round_start: RoundStartEvent,
    round_freeze_end: FreezeTimeEndedEvent,
    player_death: PlayerDeathEvent,
    player_footstep: PlayerFootstepEvent,
    break_prop: BreakPropEvent,
    item_purchase: ItemPurchaseEvent,
    bomb_beginplant: BombBeginPlantEvent,
    bomb_planted: BombPlantedEvent,
    bomb_defused: BombDefusedEvent,
    bomb_exploded: BombExplodedEvent,
    bomb_dropped: BombDroppedEvent,
    bomb_pickup: BombPickedUpEvent,
    defuser_dropped: DefuserDroppedEvent,
    defuser_pickup: DefuserPickupEvent,
    bomb_begindefuse: BeginDefuseEvent,
    bomb_abortdefuse: AbortDefuseEvent,
    hostage_follows: HostageBeginsFollowingEvent,
    hostage_hurt: HostageHurtEvent,
    hostage_rescued: HostageRescuedEvent,
    hostage_stops_following: HostageStopsFollowingEvent,
    weapon_fire: WeaponFireEvent,
    weapon_reload: WeaponReloadEvent,
    weapon_zoom: WeaponZoomEvent,
    player_blind: PlayerBlindEvent,
    client_disconnected: ClientDisconnectedEvent
};