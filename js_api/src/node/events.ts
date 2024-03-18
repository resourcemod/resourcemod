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
    client_disconnected: (event: ClientDisconnectedEvent) =>void;
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
            if (data._player) data._player = new Player(data._player._name, data._player._steamId, data._player._slot, data._player.id ?? -1)
            if (data._attacker) data._attacker = new Player(data._attacker._name, data._attacker._steamId, data._attacker._slot, data._player.id ?? -1)
            if (data._assister) data._assister = new Player(data._assister._name, data._assister._steamId, data._assister._slot, data._player.id ?? -1)
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
    private readonly _name: string;
    private readonly _item: string;
    private readonly _player: Player;
    private readonly _silent: boolean;
    private readonly _defindex: number;

    constructor(name: string, item: string, silent: boolean, defindex: number, player: Player) {
        this._name = name
        this._item = item
        this._player = player
        this._silent = silent
        this._defindex = defindex
    }

    get name() {
        return this._name
    }

    get item() {
        return this._item
    }

    get player() {
        return this._player
    }

    get silent() {
        return this._silent
    }

    get defindex() {
        return this._defindex
    }
}

export class ClientPutInServerEvent {
    private readonly _name: string;
    private readonly _player: Player;

    constructor(name: string, player: Player) {
        this._name = name
        this._player = player
    }

    get name() {
        return this._name
    }

    get player() {
        return this._player
    }
}

export class ClientDisconnectedEvent {
    private readonly _name: string;
    private readonly _player: Player;

    constructor(name: string, player: Player) {
        this._name = name
        this._player = player
    }

    get name() {
        return this._name
    }

    get player() {
        return this._player
    }
}

export class ClientConnectEvent {
    private readonly _name: string;
    private readonly _steamId: number;

    constructor(name: string, steamId: number) {
        this._name = name
        this._steamId = steamId
    }

    get name() {
        return this._name
    }

    get steamId() {
        return this._steamId
    }

    get steamId64(): string {
        return (BigInt(this.steamId) + STEAM_USER_HIGH_VALUE).toString();
    }
}

export class ClientConnectedEvent {
    private readonly _name: string;
    private readonly _steamId?: number;
    private readonly _ip: string;
    private readonly _bot: boolean;

    constructor(name: string, steamId: number, ip: string, isBot: boolean) {
        this._name = name
        this._steamId = steamId
        this._ip = ip
        this._bot = isBot
    }

    get name() {
        return this._name
    }

    get isBot() {
        return this._bot
    }

    get steamId() {
        return this._steamId ?? null
    }

    get steamId64() {
        const steamid = this.steamId;
        if (!steamid) return null// bots
        return (BigInt(steamid) + STEAM_USER_HIGH_VALUE).toString();
    }

    get ip() {
        return this._ip
    }
}

export class PlayerActivateEvent {
    private readonly _name: string;
    private readonly _player: Player;

    constructor(name: string, player: Player) {
        this._name = name
        this._player = player
    }

    get name() {
        return this._name
    }

    get player() {
        return this._player
    }
}

export class PlayerSpawnEvent {
    private readonly _name: string;
    private readonly _player: Player;

    constructor(name: string, player: Player) {
        this._name = name
        this._player = player
    }

    get name() {
        return this._name
    }

    get player() {
        return this._player
    }
}

export class PlayerChangeTeamEvent {
    private readonly _name: string;
    private readonly _team: number;
    private readonly _oldTeam: number;
    private readonly _disconnect: boolean;
    private readonly _silent: boolean;
    private readonly _player: Player;

    constructor(name: string, team: number, oldTeam: number, disconnect: boolean, silent: boolean, player: Player) {
        this._name = name
        this._team = team
        this._oldTeam = oldTeam
        this._disconnect = disconnect
        this._silent = silent
        this._player = player
    }

    get name() {
        return this._name
    }

    get team() {
        return this._team
    }

    get oldTeam() {
        return this._oldTeam
    }

    get becauseDisconnected() {
        return this._disconnect
    }

    get silent() {
        return this._silent
    }

    get player() {
        return this._player
    }
}

export class PlayerHurtEvent {
    private readonly _name: string;
    private readonly _hp: number;
    private readonly _armor: number;
    private readonly _weapon: string;
    private readonly _damageHp: number;
    private readonly _damageArmor: number;
    private readonly _hitGroup: number;
    private readonly _player: Player;
    private readonly _attacker: Player;

    constructor(name: string, hp: number, armor: number, weapon: string, damageHp: number, damageArmor: number, hitGroup: number, player: Player, attacker: Player) {
        this._name = name
        this._hp = hp
        this._armor = armor
        this._weapon = weapon
        this._damageHp = damageHp
        this._damageArmor = damageArmor
        this._hitGroup = hitGroup
        this._player = player
        this._attacker = attacker
    }

    get name() {
        return this._name
    }

    get hp() {
        return this._hp
    }

    get armor() {
        return this._armor
    }

    get weapon() {
        return this._weapon
    }

    get damage() {
        return {
            hp: this._damageHp,
            armor: this._damageArmor
        }
    }

    get hitGroup() {
        return this._hitGroup
    }

    get player() {
        return this._player
    }

    get attacker() {
        return this._attacker
    }
}

export class MapShutdownEvent {
    private readonly _name: string;

    constructor(name: string) {
        this._name = name
    }

    get name() {
        return this._name
    }
}

export class PlayerChatEvent {
    private readonly _name: string;
    private readonly _message: string;
    private readonly _teamOnly: boolean;
    private readonly _player: Player;

    constructor(name: string, text: string, teamOnly: boolean, player: Player) {
        this._name = name
        this._message = text
        this._teamOnly = teamOnly
        this._player = player
    }

    get name() {
        return this._name
    }

    get message() {
        return this._message
    }

    get teamOnly() {
        return this._teamOnly
    }

    get player() {
        return this._player
    }
}

export class GameMessageEvent {
    private readonly _name: string;
    private readonly _message: string;
    private readonly _target: string;

    constructor(name: string, text: string, target: string) {
        this._name = name
        this._message = text
        this._target = target
    }

    get name() {
        return this._name
    }

    get message() {
        return this._message
    }

    get target() {
        return this._target
    }
}

export class MapLoadedEvent {
    private readonly _name: string;
    private readonly _map: string;
    private readonly _oldMap: string;

    constructor(name: string, map: string, oldMap: string) {
        this._name = name
        this._map = map
        this._oldMap = oldMap
    }

    get name() {
        return this._name
    }

    get map() {
        return this._map
    }

    get oldMap() {
        return this._oldMap
    }
}

export class RoundEndEvent {
    private readonly _name: string;
    private readonly _winner: number;
    private readonly _reason: number;
    private readonly _message: string;
    private readonly _legacy: number;
    private readonly _playerCount: number;
    private readonly _noMusic: number;

    constructor(name: string, winner: number, reason: number, message: string, legacy: number, playerCount: number, noMusic: number) {
        this._name = name
        this._winner = winner
        this._reason = reason
        this._message = message
        this._legacy = legacy
        this._playerCount = playerCount
        this._noMusic = noMusic
    }

    get name() {
        return this._name
    }

    get winner() {
        return this._winner
    }

    get reason() {
        return this._reason
    }

    get message() {
        return this._message
    }

    get legacy() {
        return this._legacy
    }

    get playerCount() {
        return this._playerCount
    }

    get isMusicPlaying() {
        return !this._noMusic
    }
}

export class RoundStartEvent {
    private readonly _name: string;
    private readonly _timeLimit: number;
    private readonly _fragLimit: number;
    private readonly _objective: string;

    constructor(name: string, timeLimit: number, fragLimit: number, objective: string) {
        this._name = name
        this._timeLimit = timeLimit
        this._fragLimit = fragLimit
        this._objective = objective
    }

    get name() {
        return this._name
    }

    get timeLimit() {
        return this._timeLimit
    }

    get fragLimit() {
        return this._fragLimit
    }

    get objective() {
        return this._objective
    }
}

export class FreezeTimeEndedEvent {
    private readonly _name: string;

    constructor(name: string) {
        this._name = name
    }

    get name() {
        return this._name
    }
}

export class PlayerDeathEvent {
    private readonly _name: string;
    private readonly _assistFlash: boolean;
    private readonly _weapon: string;
    private readonly _weaponItemId: string;
    private readonly _weaponFauxItemId: string;
    private readonly _weaponOriginalOwnerSteamId: string;
    private readonly _headshot: boolean;
    private readonly _dominated: number;
    private readonly _revenge: number;
    private readonly _wipe: number;
    private readonly _penetrated: number;
    private readonly _noReplay: boolean;
    private readonly _noScope: boolean;
    private readonly _throughSmoke: boolean;
    private readonly _attackerBlind: boolean;
    private readonly _distance: number;
    private readonly _player: Player;
    private readonly _attacker: Player;
    private readonly _assister: Player;

    constructor(name: string, assistFlash: boolean, weapon: string, weaponItemId: string, weaponFauxItemId: string, weaponOriginalOwnerSteamId: string, headshot: boolean, dominated: number, revenge: number, wipe: number, penetrated: number, noReplay: boolean, noScope: boolean, throughSmoke: boolean, attackerBlind: boolean, distance: number, player: Player, attacker: Player, assister: Player) {
        this._name = name
        this._assistFlash = assistFlash
        this._weapon = weapon
        this._weaponItemId = weaponItemId
        this._weaponFauxItemId = weaponFauxItemId
        this._weaponOriginalOwnerSteamId = weaponOriginalOwnerSteamId
        this._headshot = headshot
        this._dominated = dominated
        this._revenge = revenge
        this._wipe = wipe
        this._penetrated = penetrated
        this._noReplay = noReplay
        this._noScope = noScope
        this._throughSmoke = throughSmoke
        this._attackerBlind = attackerBlind
        this._distance = distance
        this._player = player
        this._attacker = attacker
        this._assister = assister
    }

    get name() {
        return this._name
    }

    get assistFlash() {
        return this._assistFlash
    }

    get weapon() {
        return this._weapon
    }

    get weaponDetails() {
        return {
            name: this._weapon,
            itemId: this._weaponItemId,
            fauxId: this._weaponFauxItemId,
            originalOwnerSteamId: this._weaponOriginalOwnerSteamId,
            originalOwnerSteamId64: function() {
                if (!this.originalOwnerSteamId) return this.originalOwnerSteamId;
                return (BigInt(this.originalOwnerSteamId) + STEAM_USER_HIGH_VALUE).toString();
            }
        }
    }

    get headshot() {
        return this._headshot
    }

    get dominated() {
        return this._dominated
    }

    get revenge() {
        return this._revenge
    }

    get wipe() {
        return this._wipe
    }

    get penetrated() {
        return this._penetrated
    }

    get hasReplay() {
        return !this._noReplay
    }

    get noScope() {
        return this._noScope
    }

    get throughSmoke() {
        return this._throughSmoke
    }

    get attackerBlind() {
        return this._attackerBlind
    }

    get distance() {
        return this._distance
    }

    get player() {
        return this._player
    }

    get attacker() {
        return this._attacker
    }

    get assister() {
        return this._assister
    }
}

export class PlayerFootstepEvent {
    private readonly _name: string;
    private readonly _player: Player;

    constructor(name: string, player: Player) {
        this._name = name
        this._player = player
    }

    get name() {
        return this._name
    }

    get player() {
        return this._player
    }
}

export class BreakPropEvent {
    private readonly _name: string;
    private readonly _entityId: number;
    private readonly _player: Player;

    constructor(name: string, entityId: number, player: Player) {
        this._name = name
        this._entityId = entityId
        this._player = player
    }

    get name() {
        return this._name
    }

    get entityId() {
        return this._entityId
    }

    get player() {
        return this._player
    }
}

export class ItemPurchaseEvent {
    private readonly _name: string;
    private readonly _team: number;
    private readonly _loadout: number;
    private readonly _weapon: string;
    private readonly _player: Player;

    constructor(name: string, team: number, loadout: number, weapon: string, player: Player) {
        this._name = name
        this._team = team
        this._loadout = loadout
        this._weapon = weapon
        this._player = player
    }

    get name() {
        return this._name
    }

    get team() {
        return this._team
    }

    get loadout() {
        return this._loadout
    }

    get weapon() {
        return this._weapon
    }

    get player() {
        return this._player
    }
}

export class BombBeginPlantEvent {
    private readonly _name: string;
    private readonly _site: number;
    private readonly _player: Player;

    constructor(name: string, site: number, player: Player) {
        this._name = name
        this._site = site
        this._player = player
    }

    get name() {
        return this._name
    }

    get site() {
        return this._site
    }

    get player() {
        return this._player
    }
}

export class BombPlantedEvent {
    private readonly _name: string;
    private readonly _site: number;
    private readonly _player: Player;

    constructor(name: string, site: number, player: Player) {
        this._name = name
        this._site = site
        this._player = player
    }

    get name() {
        return this._name
    }

    get site() {
        return this._site
    }

    get player() {
        return this._player
    }
}

export class BombDefusedEvent {
    private readonly _name: string;
    private readonly _site: number;
    private readonly _player: Player;

    constructor(name: string, site: number, player: Player) {
        this._name = name
        this._site = site
        this._player = player
    }

    get name() {
        return this._name
    }

    get site() {
        return this._site
    }

    get player() {
        return this._player
    }
}

export class BombExplodedEvent {
    private readonly _name: string;
    private readonly _site: number;
    private readonly _player: Player;

    constructor(name: string, site: number, player: Player) {
        this._name = name
        this._site = site
        this._player = player
    }

    get name() {
        return this._name
    }

    get site() {
        return this._site
    }

    get player() {
        return this._player
    }
}

export class BombDroppedEvent {
    private readonly _name: string;
    private readonly _entityId: number;
    private readonly _player: Player;

    constructor(name: string, entityId: number, player: Player) {
        this._name = name
        this._entityId = entityId
        this._player = player
    }

    get name() {
        return this._name
    }

    get entityId() {
        return this._entityId
    }

    get player() {
        return this._player
    }
}

export class BombPickedUpEvent {
    private readonly _name: string;
    private readonly _player: Player;

    constructor(name: string, player: Player) {
        this._name = name
        this._player = player
    }

    get name() {
        return this._name
    }

    get player() {
        return this._player
    }
}

export class DefuserDroppedEvent {
    private readonly _name: string;
    private readonly _entityId: number;

    constructor(name: string, entityId: number) {
        this._name = name
        this._entityId = entityId
    }

    get name() {
        return this._name
    }

    get entityId() {
        return this._entityId
    }
}

export class DefuserPickupEvent {
    private readonly _name: string;
    private readonly _entityId: number;
    private readonly _player: Player;

    constructor(name: string, entityId: number, player: Player) {
        this._name = name
        this._entityId = entityId
        this._player = player
    }

    get name() {
        return this._name
    }

    get entityId() {
        return this._entityId
    }

    get player() {
        return this._player
    }
}

export class BeginDefuseEvent {
    private readonly _name: string;
    private readonly _hasKit: boolean;
    private readonly _player: Player;

    constructor(name: string, hasKit: boolean, player: Player) {
        this._name = name
        this._hasKit = hasKit
        this._player = player
    }

    get name() {
        return this._name
    }

    get hasKit() {
        return this._hasKit
    }

    get player() {
        return this._player
    }
}

export class AbortDefuseEvent {
    private readonly _name: string;
    private readonly _player: Player;

    constructor(name: string, player: Player) {
        this._name = name
        this._player = player
    }

    get name() {
        return this._name
    }

    get player() {
        return this._player
    }
}

export class HostageBeginsFollowingEvent {
    private readonly _name: string;
    private readonly _hostage: number;
    private readonly _player: Player;

    constructor(name: string, hostage: number, player: Player) {
        this._name = name
        this._hostage = hostage
        this._player = player
    }

    get name() {
        return this._name
    }

    get hostage() {
        return this._hostage
    }

    get player() {
        return this._player
    }
}

export class HostageHurtEvent {
    private readonly _name: string;
    private readonly _hostage: number;
    private readonly _player: Player;

    constructor(name: string, hostage: number, player: Player) {
        this._name = name
        this._hostage = hostage
        this._player = player
    }

    get name() {
        return this._name
    }

    get hostage() {
        return this._hostage
    }

    get player() {
        return this._player
    }
}

export class HostageRescuedEvent {
    private readonly _name: string;
    private readonly _hostage: number;
    private readonly _site: number;
    private readonly _player: Player;

    constructor(name: string, hostage: number, site: number, player: Player) {
        this._name = name
        this._hostage = hostage
        this._site = site
        this._player = player
    }

    get name() {
        return this._name
    }

    get hostage() {
        return this._hostage
    }

    get site() {
        return this._site
    }

    get player() {
        return this._player
    }
}

export class HostageStopsFollowingEvent {
    private readonly _name: string;
    private readonly _hostage: number;
    private readonly _player: Player;

    constructor(name: string, hostage: number, player: Player) {
        this._name = name
        this._hostage = hostage
        this._player = player
    }

    get name() {
        return this._name
    }

    get hostage() {
        return this._hostage
    }

    get player() {
        return this._player
    }
}

export class WeaponFireEvent {
    private readonly _name: string;
    private readonly _weapon: string;
    private readonly _silenced: boolean;
    private readonly _player: Player;

    constructor(name: string, weapon: string, silenced: boolean, player: Player) {
        this._name = name
        this._weapon = weapon
        this._silenced = silenced
        this._player = player
    }

    get name() {
        return this._name
    }

    get weapon() {
        return this._weapon
    }

    get silenced() {
        return this._silenced
    }

    get player() {
        return this._player
    }
}

export class WeaponReloadEvent {
    private readonly _name: string;
    private readonly _player: Player;

    constructor(name: string, player: Player) {
        this._name = name
        this._player = player
    }

    get name() {
        return this._name
    }

    get player() {
        return this._player
    }
}

export class WeaponZoomEvent {
    private readonly _name: string;
    private readonly _player: Player;

    constructor(name: string, player: Player) {
        this._name = name
        this._player = player
    }

    get name() {
        return this._name
    }

    get player() {
        return this._player
    }
}

export class PlayerBlindEvent {
    private readonly _name: string;
    private readonly _entityId: number;
    private readonly _blindDuration: number;
    private readonly _player: Player;
    private readonly _attacker: Player;

    constructor(name: string, entityId: number, blindDuration: number, player: Player, attacker: Player) {
        this._name = name
        this._entityId = entityId
        this._blindDuration = blindDuration
        this._player = player
        this._attacker = attacker
    }

    get name() {
        return this._name
    }

    get entityId() {
        return this._entityId
    }

    get blindDuration() {
        return this._blindDuration
    }

    get player() {
        return this._player
    }

    get attacker() {
        return this._attacker
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