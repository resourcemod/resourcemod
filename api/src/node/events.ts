import { Player } from './player';
import { PREVENT_EVENT } from "./constants"
const events = new Map();

type IEvents = {
    'item_pickup': (data: ItemPickupEvent) => void;
    'client_put_in_server': (data: ClientPutInServerEvent) => void;
    'client_connected': (data: ClientConnectedEvent) => void;
    'client_connect': (data: ClientConnectEvent) => void;
    'player_activate': (data: PlayerActivateEvent) => void;
    'player_spawn': (data: PlayerSpawnEvent) => void;
    'player_team': (data: PlayerChangeTeamEvent) => void;
    'player_hurt': (data: PlayerHurtEvent) => void;
    'map_shutdown': (data: MapShutdownEvent) => void;
    'player_chat': (data: PlayerChatEvent) => void;
    'game_message': (data: GameMessageEvent) => void;
    'map_loaded': (data: MapLoadedEvent) => void;
    'round_end': (data: RoundEndEvent) => void;
    'round_start': (data: RoundStartEvent) => void;
    'round_freeze_end': (data: FreezeTimeEndedEvent) => void;
    'player_death': (data: PlayerDeathEvent) => void;
    'player_footstep': (data: PlayerFootstepEvent) => void;
    'break_prop': (data: BreakPropEvent) => void;
    'item_purchase': (data: ItemPurchaseEvent) => void;
    'bomb_beginplant': (data: BombBeginPlantEvent) => void;
    'bomb_planted': (data: BombPlantedEvent) => void;
    'bomb_defused': (data: BombDefusedEvent) => void;
    'bomb_exploded': (data: BombExplodedEvent) => void;
    'bomb_dropped': (data: BombDroppedEvent) => void;
    'bomb_pickup': (data: BombPickedUpEvent) => void;
    'defuser_dropped': (data: DefuserDroppedEvent) => void;
    'defuser_pickup': (data: DefuserPickupEvent) => void;
    'bomb_begindefuse': (data: BeginDefuseEvent) => void;
    'bomb_abortdefuse': (data: AbortDefuseEvent) => void;
    'hostage_follows': (data: HostageBeginsFollowingEvent) => void;
    'hostage_hurt': (data: HostageHurtEvent) => void;
    'hostage_rescued': (data: HostageRescuedEvent) => void;
    'hostage_stops_following': (data: HostageStopsFollowingEvent) => void;
    'weapon_fire': (data: WeaponFireEvent) => void;
    'weapon_reload': (data: WeaponReloadEvent) => void;
    'weapon_zoom': (data: WeaponZoomEvent) => void;
    'player_blind': (data: PlayerBlindEvent) => void;
    'client_disconnected': (data: ClientDisconnectedEvent) =>void;
}

// actual source engine callback
export const _onEventCall = (data: any) => {
    if (data.name === undefined) {
        return "undefined_event";
    } 
    let prevent = "event_no_prevent";
    if (events.has(data.name)) {
        // temporary hack (cuz classes works weird)
        if (data.player !== undefined && data.player !== null) {
            data.player = new Player(data.player.name, data.player.steamId, data.player.slot)
        }
        if (data.attacker !== undefined && data.attacker !== null) {
            data.attacker = new Player(data.attacker.name, data.attacker.steamId, data.attacker.slot)
        }
        if (data.assister !== undefined && data.assister !== null) {
            data.assister = new Player(data.assister.name, data.assister.steamId, data.assister.slot)
        }
        events.get(data.name).forEach((listener: Function) => {
            if(typeof listener === 'function') {
                if (listener(data) === PREVENT_EVENT) {
                    prevent = PREVENT_EVENT
                }
            }
        });
    }
    return prevent;
}
export function onEvent<T extends keyof IEvents>(name: T, callback: IEvents[T]) {
    if(typeof callback !== 'function') {
        throw 'Callback must be a function.'
    }

    if (events.has(name) && Array.isArray(events.get(name))) {
        events.get(name).push(callback)
        return
    }
    events.set(name, [callback])
}

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
        // this.getName = () => {
        //     return this.name
        // }
    
        // this.getItem = () => {
        //     return this.item
        // }
    
        // this.getPlayer = () => {
        //     return this.player
        // }
    
        // this.isSilent = () => {
        //     return this.isSilent
        // }
    
        // this.getDefIndex = () => {
        //     return this.defindex
        // }
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

    get handler() {
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

    get handler() {
        return this._player
    }
}

export class ClientConnectEvent {
    private readonly _name: string;
    private readonly _steamId?: string;

    constructor(name: string, steamId: string) {
        this._name = name
        this._steamId = steamId
    }

    get name() {
        return this._name
    }

    get steamId() {
        return this._steamId ?? undefined
    }

    get steamId64() {
        if (this.steamId == undefined) return undefined // bots
        return (BigInt(this.steamId) + BigInt(76561197960265728)).toString();
    }
}

export class ClientConnectedEvent {
    private readonly _name: string;
    private readonly _steamId?: string;
    private readonly _ip: string;
    private readonly _bot: boolean;

    constructor(name: string, steamId: string, ip: string, isBot: boolean) {
        this._name = name
        this._steamId = steamId
        this._ip = ip
        this._bot = isBot
    }

    get isBot() {
        return this._bot
    }

    get steamId() {
        return this._steamId ?? undefined
    }

    get steamId64() {
        if (this.steamId == undefined) return undefined // bots
        return (BigInt(this.steamId) + BigInt(76561197960265728)).toString();
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

    get handler() {
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

    get handler() {
        return this._player
    }
}

export class PlayerChangeTeamEvent {
    private readonly _name: string;
    private readonly _team: string;
    private readonly _oldTeam: string;
    private readonly _disconnect: boolean;
    private readonly _silent: boolean;
    private readonly _player: Player;

    constructor(name: string, team: string, oldTeam: string, disconnect: boolean, silent: boolean, player: Player) {
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

    get handler() {
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
    private readonly _hitGoup: number;
    private readonly _player: Player;
    private readonly _attacker: Player;

    constructor(name: string, hp: number, armor: number, weapon: string, damageHp: number, damageArmor: number, hitGoup: number, player: Player, attacker: Player) {
        this._name = name
        this._hp = hp
        this._armor = armor
        this._weapon = weapon
        this._damageHp = damageHp
        this._damageArmor = damageArmor
        this._hitGoup = hitGoup
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

    get hitGoup() {
        return this._hitGoup
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

    get handler() {
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
    private readonly _winner: string;
    private readonly _reason: string;
    private readonly _message: string;
    private readonly _legacy: boolean;
    private readonly _playerCount: number;
    private readonly _noMusic: boolean;

    constructor(name: string, winner: string, reason: string, message: string, legacy: boolean, playerCount: number, noMusic: boolean) {
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
    private readonly _weaponItemId: number;
    private readonly _weaponFauxItemId: number;
    private readonly _weaponOriginalOwnerSteamId: number;
    private readonly _headshot: boolean;
    private readonly _dominated: boolean;
    private readonly _revenge: boolean;
    private readonly _wipe: boolean;
    private readonly _penetrated: boolean;
    private readonly _noReplay: boolean;
    private readonly _noScope: boolean;
    private readonly _throughSmoke: boolean;
    private readonly _attackerBlind: boolean;
    private readonly _distance: number;
    private readonly _player: Player;
    private readonly _attacker: Player;
    private readonly _assister: Player;

    constructor(name: string, assistFlash: boolean, weapon: string, weaponItemId: number, weaponFauxItemId: number, weaponOriginalOwnerSteamId: number, headshot: boolean, dominated: boolean, revenge: boolean, wipe: boolean, penetrated: boolean, noReplay: boolean, noScope: boolean, throughSmoke: boolean, attackerBlind: boolean, distance: number, player: Player, attacker: Player, assister: Player) {
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
            originalOwnerSteamId64: () => {
                if (this._weaponOriginalOwnerSteamId === 0) {
                    return 0;
                }
                return (BigInt(this._weaponOriginalOwnerSteamId) + BigInt(76561197960265728)).toString();
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

    get handler() {
        return this._player
    }
}

export class BreakableBrokeEvent {
    private readonly _name: string;
    private readonly _entityId: number;
    private readonly _material: string;
    private readonly _player: Player;

    constructor(name: string, entityId: number, material: string, player: Player) {
        this._name = name
        this._entityId = entityId
        this._material = material
        this._player = player
    }

    get name() {
        return this._name
    }

    get entityId() {
        return this._entityId
    }

    get material() {
        return this._material
    }

    get handler() {
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

    get handler() {
        return this._player
    }
}

export class ItemPurchaseEvent {
    private readonly _name: string;
    private readonly _team: string;
    private readonly _loadout: string;
    private readonly _weapon: string;
    private readonly _player: Player;

    constructor(name: string, team: string, loadout: string, weapon: string, player: Player) {
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

    get handler() {
        return this._player
    }
}

export class BombBeginPlantEvent {
    private readonly _name: string;
    private readonly _site: string;
    private readonly _player: Player;

    constructor(name: string, site: string, player: Player) {
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

    get handler() {
        return this._player
    }
}

export class BombPlantedEvent {
    private readonly _name: string;
    private readonly _site: string;
    private readonly _player: Player;

    constructor(name: string, site: string, player: Player) {
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

    get handler() {
        return this._player
    }
}

export class BombDefusedEvent {
    private readonly _name: string;
    private readonly _site: string;
    private readonly _player: Player;

    constructor(name: string, site: string, player: Player) {
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

    get handler() {
        return this._player
    }
}

export class BombExplodedEvent {
    private readonly _name: string;
    private readonly _site: string;
    private readonly _player: Player;

    constructor(name: string, site: string, player: Player) {
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

    get handler() {
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

    get handler() {
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

    get handler() {
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

    get handler() {
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

    get withKit() {
        return this._hasKit
    }

    get handler() {
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

    get handler() {
        return this._player
    }
}

export class HostageBeginsFollowingEvent {
    private readonly _name: string;
    private readonly _hostage: string;
    private readonly _player: Player;

    constructor(name: string, hostage: string, player: Player) {
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

    get handler() {
        return this._player
    }
}

export class HostageHurtEvent {
    private readonly _name: string;
    private readonly _hostage: string;
    private readonly _player: Player;

    constructor(name: string, hostage: string, player: Player) {
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

    get handler() {
        return this._player
    }
}

export class HostageRescuedEvent {
    private readonly _name: string;
    private readonly _hostage: string;
    private readonly _site: string;
    private readonly _player: Player;

    constructor(name: string, hostage: string, site: string, player: Player) {
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

    get handler() {
        return this._player
    }
}

export class HostageStopsFollowingEvent {
    private readonly _name: string;
    private readonly _hostage: string;
    private readonly _player: Player;

    constructor(name: string, hostage: string, player: Player) {
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

    get handler() {
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

    get handler() {
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

    get handler() {
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

    get handler() {
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

export const _ClientConnectedEvent = (name: string, steamId: string, ip: string, isBot: boolean) => {
    return new ClientConnectedEvent(name, steamId, ip, isBot)
}

export const _ClientConnectEvent = (name: string, steamId: string) => {
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

export const _PlayerChangeTeamEvent = (name: string, team: string, oldTeam: string, disconnect: boolean, silent: boolean, player: Player) => {
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

export const _RoundEndEvent = (name: string, winner: string, reason: string, message: string, legacy: boolean, playerCount: number, noMusic: boolean) => {
    return new RoundEndEvent(name, winner, reason, message, legacy, playerCount, noMusic)
}

export const _RoundStartEvent = (name: string, timeLimit: number, fragLimit: number, objective: string) => {
    return new RoundStartEvent(name, timeLimit, fragLimit, objective)
}

export const _FreezeTimeEndedEvent = (name: string) => {
    return new FreezeTimeEndedEvent(name)
}

export const _PlayerDeathEvent = (name: string, assistFlash: boolean, weapon: string, weaponItemId: number, weaponFauxItemId: number, weaponOriginalOwnerSteamId: number, headshot: boolean, dominated: boolean, revenge: boolean, wipe: boolean, penetrated: boolean, noReplay: boolean, noScope: boolean, throughSmoke: boolean, attackerBlind: boolean, distance: number, player: Player, attacker: Player, assister: Player) => {
    return new PlayerDeathEvent(name, assistFlash, weapon, weaponItemId, weaponFauxItemId, weaponOriginalOwnerSteamId, headshot, dominated, revenge, wipe, penetrated, noReplay, noScope, throughSmoke, attackerBlind, distance, player, attacker, assister)
}

export const _PlayerFootstepEvent = (name: string, player: Player) => {
    return new PlayerFootstepEvent(name, player)
}

export const _BreakPropEvent = (name: string, entityId: number, player: Player) => {
    return new BreakPropEvent(name, entityId, player)
}

export const _ItemPurchaseEvent = (name: string, team: string, loadout: string, weapon: string, player: Player) => {
    return new ItemPurchaseEvent(name, team, loadout, weapon, player)
}

export const _BombBeginPlantEvent = (name: string, site: string, player: Player) => {
    return new BombBeginPlantEvent(name, site, player)
}

export const _BombPlantedEvent = (name: string, site: string, player: Player) => {
    return new BombPlantedEvent(name, site, player)
}

export const _BombDefusedEvent = (name: string, site: string, player: Player) => {
    return new BombDefusedEvent(name, site, player)
}

export const _BombExplodedEvent = (name: string, site: string, player: Player) => {
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

export const _HostageBeginsFollowingEvent = (name: string, hostage: string, player: Player) => {
    return new HostageBeginsFollowingEvent(name, hostage, player)
}

export const _HostageHurtEvent = (name: string, hostage: string, player: Player) => {
    return new HostageHurtEvent(name, hostage, player)
}

export const _HostageRescuedEvent = (name: string, hostage: string, site: string, player: Player) => {
    return new HostageRescuedEvent(name, hostage, site, player)
}

export const _HostageStopsFollowingEvent = (name: string, hostage: string, player: Player) => {
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

export default {
    _onEventCall,

    _ItemPickupEvent,
    ItemPickupEvent,

    _ClientPutInServerEvent,
    ClientPutInServerEvent,

    _ClientConnectedEvent,
    ClientConnectedEvent,
    
    _ClientConnectEvent,
    ClientConnectEvent,

    _PlayerActivateEvent,
    PlayerActivateEvent,

    _PlayerSpawnEvent,
    PlayerSpawnEvent,

    _PlayerChangeTeamEvent,
    PlayerChangeTeamEvent,

    _PlayerHurtEvent,
    PlayerHurtEvent,
    
    _MapShutdownEvent,
    MapShutdownEvent,
    
    _PlayerChatEvent,
    PlayerChatEvent,

    _GameMessageEvent,
    GameMessageEvent,

    _MapLoadedEvent,
    MapLoadedEvent,

    _RoundEndEvent,
    RoundEndEvent,

    _RoundStartEvent,
    RoundStartEvent,

    _FreezeTimeEndedEvent,
    FreezeTimeEndedEvent,

    _PlayerDeathEvent,
    PlayerDeathEvent,

    _PlayerFootstepEvent,
    PlayerFootstepEvent,

    _BreakPropEvent,
    BreakPropEvent,

    _ItemPurchaseEvent,
    ItemPurchaseEvent,

    _BombBeginPlantEvent,
    BombBeginPlantEvent,

    _BombPlantedEvent,
    BombPlantedEvent,

    _BombExplodedEvent,
    BombExplodedEvent,

    _BombDefusedEvent,
    BombDefusedEvent,

    _BombPickedUpEvent,
    BombPickedUpEvent,

    _BombDroppedEvent,
    BombDroppedEvent,

    _DefuserDroppedEvent,
    DefuserDroppedEvent,

    _DefuserPickupEvent,
    DefuserPickupEvent,

    _BeginDefuseEvent,
    BeginDefuseEvent,

    _AbortDefuseEvent,
    AbortDefuseEvent,

    _HostageBeginsFollowingEvent,
    HostageBeginsFollowingEvent,

    _HostageHurtEvent,
    HostageHurtEvent,

    _HostageRescuedEvent,
    HostageRescuedEvent,

    _HostageStopsFollowingEvent,
    HostageStopsFollowingEvent,

    _WeaponFireEvent,
    WeaponFireEvent,

    _WeaponReloadEvent,
    WeaponReloadEvent,

    _WeaponZoomEvent,
    WeaponZoomEvent,

    _PlayerBlindEvent,
    PlayerBlindEvent,

    _ClientDisconnectedEvent,
    ClientDisconnectedEvent,

    onEvent,
}