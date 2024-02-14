const { Player } = require('./player');
const { PREVENT_EVENT, STEAM_USER_HIGH_VALUE } = require('./constants');
const events = new Map();

// actual source engine callback
const _onEventCall = (data) => {
    if (!data.name) {
        return "undefined_event";
    }
    let prevent = "event_no_prevent";
    if (events.has(data.name)) {
        // temporary hack (cuz classes works weird)
        if (data.player) {
            data.player = new Player(data.player.name, data.player.steamId, data.player.slot)
        }
        if (data.attacker) {
            data.attacker = new Player(data.attacker.name, data.attacker.steamId, data.attacker.slot)
        }
        if (data.assister) {
            data.assister = new Player(data.assister.name, data.assister.steamId, data.assister.slot)
        }
        events.get(data.name).forEach(listener => {
            if (typeof listener === 'function') {
                if (listener(data) === PREVENT_EVENT) {
                    prevent = PREVENT_EVENT
                }
            }
        });
    }
    return prevent;
}
const onEvent = (name, callback) => {
    if (typeof callback !== 'function') {
        throw 'Callback must be a function.'
    }

    if (events.has(name) && Array.isArray(events.get(name))) {
        events.get(name).push(callback)
        return
    }
    events.set(name, [callback])
}


// Event subscribers
const onItemPickup = (callback) => {
    onEvent('item_pickup', callback)
}

const onClientPutInServer = (callback) => {
    onEvent('client_put_in_server', callback)
}

const onClientConnected = (callback) => {
    onEvent('client_connected', callback)
}

const onClientConnect = (callback) => {
    onEvent('client_connect', callback)
}

const onPlayerActivate = (callback) => {
    onEvent('player_activate', callback)
}

const onPlayerSpawn = (callback) => {
    onEvent('player_spawn', callback)
}

const onPlayerChangeTeam = (callback) => {
    onEvent('player_team', callback)
}

const onPlayerHurt = (callback) => {
    onEvent('player_hurt', callback)
}

const onMapShutdown = (callback) => {
    onEvent('map_shutdown', callback)
}

const onPlayerChat = (callback) => {
    onEvent('player_chat', callback)
}

const onGameMessage = (callback) => {
    onEvent('game_message', callback)
}

const onMapLoaded = (callback) => {
    onEvent('map_loaded', callback)
}

const onRoundEnded = (callback) => {
    onEvent('round_end', callback)
}

const onRoundStarted = (callback) => {
    onEvent('round_start', callback)
}

const onFreezeTimeEnded = (callback) => {
    onEvent('round_freeze_end', callback)
}

const onPlayerDeath = (callback) => {
    onEvent('player_death', callback)
}

const onPlayerFootstep = (callback) => {
    onEvent('player_footstep', callback)
}

const onPropBreak = (callback) => {
    onEvent('break_prop', callback)
}

const onItemPurchase = (callback) => {
    onEvent('item_purchase', callback)
}

const onBombBeginPlant = (callback) => {
    onEvent('bomb_beginplant', callback)
}

const onBombPlanted = (callback) => {
    onEvent('bomb_planted', callback)
}

const onBombDefused = (callback) => {
    onEvent('bomb_defused', callback)
}

const onBombExploded = (callback) => {
    onEvent('bomb_exploded', callback)
}

const onBombDropped = (callback) => {
    onEvent('bomb_dropped', callback)
}

const onBombPickup = (callback) => {
    onEvent('bomb_pickup', callback)
}

const onDefuserDropped = (callback) => {
    onEvent('defuser_dropped', callback)
}

const onDefuserPickup = (callback) => {
    onEvent('defuser_pickup', callback)
}

const onBeginDefuse = (callback) => {
    onEvent('bomb_begindefuse', callback)
}

const onAbortDefuse = (callback) => {
    onEvent('bomb_abortdefuse', callback)
}

const onHostageBeginsFollowing = (callback) => {
    onEvent('hostage_follows', callback)
}

const onHostageHurt = (callback) => {
    onEvent('hostage_hurt', callback)
}

const onHostageRescued = (callback) => {
    onEvent('hostage_rescued', callback)
}

const onHostageStopsFollowing = (callback) => {
    onEvent('hostage_stops_following', callback)
}

const onWeaponFire = (callback) => {
    onEvent('weapon_fire', callback)
}

const onWeaponReload = (callback) => {
    onEvent('weapon_reload', callback)
}

const onWeaponZoom = (callback) => {
    onEvent('weapon_zoom', callback)
}

const onPlayerBlind = (callback) => {
    onEvent('player_blind', callback)
}

const onClientDisconnected = (callback) => {
    onEvent('client_disconnected', callback)
}

// Event classes
class ItemPickupEvent {
    constructor(name, item, silent, defindex, player) {
        this.name = name
        this.item = item
        this.player = player
        this.silent = silent
        this.defindex = defindex
        this.getName = () => {
            return this.name
        }

        this.getItem = () => {
            return this.item
        }

        this.getPlayer = () => {
            return this.player
        }

        this.isSilent = () => {
            return this.isSilent
        }

        this.getDefIndex = () => {
            return this.defindex
        }
    }
}

class ClientPutInServerEvent {
    constructor(name, player) {
        this.name = name
        this.player = player
        this.getName = () => {
            return this.name
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class ClientDisconnectedEvent {
    constructor(name, player) {
        this.name = name
        this.player = player
        this.getName = () => {
            return this.name
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class ClientConnectEvent {
    constructor(name, steamId) {
        this.name = name
        this.steamId = steamId
        this.getName = () => {
            return this.name
        }

        this.getSteamId = () => {
            return this.steamId
        }

        this.getSteamId64 = () => {
            const steamid = this.getSteamId();
            if (!steamid) return // bots
            return (BigInt(steamid) + STEAM_USER_HIGH_VALUE).toString();
        }
    }
}

class ClientConnectedEvent {
    constructor(name, steamId, ip, isBot) {
        this.name = name
        this.steamId = steamId
        this.ip = ip
        this._bot = isBot
        this.isBot = () => {
            return this._bot
        }

        this.getSteamId = () => {
            return this.steamId
        }

        this.getSteamId64 = () => {
            if (!this.getSteamId()) return // bots
            return (BigInt(this.getSteamId()) + STEAM_USER_HIGH_VALUE).toString();
        }

        this.getIp = () => {
            return this.ip
        }
    }
}

class PlayerActivateEvent {
    constructor(name, player) {
        this.name = name
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class PlayerSpawnEvent {
    constructor(name, player) {
        this.name = name
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class PlayerChangeTeamEvent {
    constructor(name, team, oldTeam, disconnect, silent, player) {
        this.name = name
        this.team = team
        this.oldTeam = oldTeam
        this.disconnect = disconnect
        this.silent = silent
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getTeam = () => {
            return this.team
        }

        this.getOldTeam = () => {
            return this.oldTeam
        }

        this.isBecauseDisconnected = () => {
            return this.disconnect
        }

        this.isSilent = () => {
            return this.silent
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class PlayerHurtEvent {
    constructor(name, hp, armor, weapon, damageHp, damageArmor, hitGoup, player, attacker) {
        this.name = name
        this.hp = hp
        this.armor = armor
        this.weapon = weapon
        this.damageHp = damageHp
        this.damageArmor = damageArmor
        this.hitGoup = hitGoup
        this.attacker = attacker
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getHp = () => {
            return this.hp
        }

        this.getArmor = () => {
            return this.armor
        }

        this.getWeapon = () => {
            return this.weapon
        }

        this.getDamage = () => {
            return {
                hp: this.damageHp,
                armor: this.damageArmor
            }
        }

        this.getHitGroup = () => {
            return this.hitGoup
        }

        this.getPlayer = () => {
            return this.player
        }

        this.getAttacker = () => {
            return this.attacker
        }
    }
}

class MapShutdownEvent {
    constructor(name) {
        this.name = name
        this.getName = () => {
            return this.name
        }
    }
}

class PlayerChatEvent {
    constructor(name, text, teamOnly, player) {
        this.name = name
        this.message = text
        this.teamOnly = teamOnly;
        this.player = player

        this.getMessage = () => {
            return this.message
        }

        this.getName = () => {
            return this.name
        }

        this.isTeamOnly = () => {
            return this.teamOnly
        }

        this.getPlayer = () => {
            this.player
        }
    }
}

class GameMessageEvent {
    constructor(name, text, target) {
        this.name = name
        this.message = text
        this.target = target

        this.getName = () => {
            return this.name
        }

        this.getMessage = () => {
            return this.message
        }

        this.getTarget = () => {
            return this.target
        }
    }
}

class MapLoadedEvent {
    constructor(name, map, oldMap) {
        this.name = name
        this.map = map
        this.oldMap = oldMap

        this.getName = () => {
            return this.name
        }

        this.getMap = () => {
            return this.map
        }

        this.getOldMap = () => {
            return this.oldMap
        }
    }
}

class RoundEndEvent {
    constructor(name, winner, reason, message, legacy, playerCount, noMusic) {
        this.name = name
        this.winner = winner
        this.reason = reason
        this.message = message
        this.legacy = legacy
        this.playerCount = playerCount
        this.noMusic = noMusic

        this.getName = () => {
            return this.name
        }

        this.getWinnerTeam = () => {
            return this.winner
        }

        this.getReason = () => {
            return this.reason
        }

        this.getMessage = () => {
            return this.message
        }

        this.getLegacy = () => {
            return this.legacy
        }

        this.getPlayersCount = () => {
            return this.playerCount
        }

        this.isMusicPlaying = () => {
            return !this.noMusic
        }
    }
}

class RoundStartEvent {
    constructor(name, timeLimit, fragLimit, objective) {
        this.name = name
        this.timeLimit = timeLimit
        this.fragLimit = fragLimit
        this.objective = objective

        this.getName = () => {
            return this.name
        }

        this.getTimeLimit = () => {
            return this.timeLimit
        }

        this.getFragLimit = () => {
            return this.fragLimit
        }

        this.getObjective = () => {
            return this.objective
        }
    }
}

class FreezeTimeEndedEvent {
    constructor(name) {
        this.name = name

        this.getName = () => {
            return this.name
        }
    }
}

class PlayerDeathEvent {
    constructor(name, assistFlash, weapon, weaponItemId, weaponFauxItemId, weaponOriginalOwnerSteamId, headshot, dominated, revenge, wipe, penetrated, noReplay, noScope, throughSmoke, attackerBlind, distance, player, attacker, assister) {
        this.name = name
        this.assistFlash = assistFlash
        this.weapon = weapon
        this.weaponItemId = weaponItemId
        this.weaponFauxItemId = weaponFauxItemId
        this.weaponOriginalOwnerSteamId = weaponOriginalOwnerSteamId
        this.headshot = headshot
        this.dominated = dominated
        this.revenge = revenge
        this.wipe = wipe
        this.penetrated = penetrated
        this.noReplay = noReplay
        this.noScope = noScope
        this.throughSmoke = throughSmoke
        this.attackerBlind = attackerBlind
        this.distance = distance

        this.player = player
        this.attacker = attacker
        this.assister = assister

        this.getName = () => {
            return this.name
        }

        this.isFlashAssisted = () => {
            return this.assistFlash
        }

        this.getWeapon = () => {
            return this.weapon
        }

        this.getWeaponDetails = () => {
            return {
                name: this.weapon,
                itemId: this.weaponItemId,
                fauxId: this.weaponFauxItemId,
                originalOwnerSteamId: this.originalOwnerSteamId,
                originalOwnerSteamId64: () => {
                    if (this.originalOwnerSteamId === 0) {
                        return this.originalOwnerSteamId;
                    }
                    return (BigInt(this.originalOwnerSteamId) + STEAM_USER_HIGH_VALUE).toString();
                }
            }
        }

        this.isHeadshot = () => {
            return this.headshot
        }

        this.getDominated = () => {
            return this.dominated
        }

        this.getRevenge = () => {
            return this.revenge
        }

        this.getWipe = () => {
            return this.wipe
        }

        this.getPenetrated = () => {
            return this.penetrated
        }

        this.hasReplay = () => {
            return !this.noReplay
        }

        this.isNoScope = () => {
            return this.noScope
        }

        this.isThroughSmoke = () => {
            return this.throughSmoke
        }

        this.isAttackerWasBlind = () => {
            return this.attackerBlind
        }

        this.getDistance = () => {
            return this.distance
        }

        this.getPlayer = () => {
            return this.player
        }

        this.getAttacker = () => {
            return this.attacker
        }

        this.getAssister = () => {
            return this.assister
        }
    }
}

class PlayerFootstepEvent {
    constructor(name, player) {
        this.name = name
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getPlayer = () => {
            this.player
        }
    }
}

class BreakableBrokeEvent {
    constructor(name, entityId, material, player) {
        this.name = name
        this.entityId = entityId
        this.material = material
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getEntityId = () => {
            return this.entityId
        }

        this.getMaterial = () => {
            return this.material
        }

        this.getPlayer = () => {
            this.player
        }
    }
}

class BreakPropEvent {
    constructor(name, entityId, player) {
        this.name = name
        this.entityId = entityId
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getEntityId = () => {
            return this.entityId
        }

        this.getPlayer = () => {
            this.player
        }
    }
}

class ItemPurchaseEvent {
    constructor(name, team, loadout, weapon, player) {
        this.name = name
        this.team = team
        this.loadout = loadout
        this.weapon = weapon
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getTeam = () => {
            return this.team
        }

        this.getLoadout = () => {
            return this.loadout
        }

        this.getWeapon = () => {
            return this.weapon
        }

        this.getPlayer = () => {
            this.player
        }
    }
}

class BombBeginPlantEvent {
    constructor(name, site, player) {
        this.name = name
        this.site = site
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getSite = () => {
            return this.site
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class BombPlantedEvent {
    constructor(name, site, player) {
        this.name = name
        this.site = site
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getSite = () => {
            return this.site
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class BombDefusedEvent {
    constructor(name, site, player) {
        this.name = name
        this.site = site
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getSite = () => {
            return this.site
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class BombExplodedEvent {
    constructor(name, site, player) {
        this.name = name
        this.site = site
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getSite = () => {
            return this.site
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class BombDroppedEvent {
    constructor(name, entityId, player) {
        this.name = name
        this.entityId = entityId
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getEntityId = () => {
            return this.entityId
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class BombPickedUpEvent {
    constructor(name, player) {
        this.name = name
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class DefuserDroppedEvent {
    constructor(name, entityId) {
        this.name = name
        this.entityId = entityId

        this.getName = () => {
            return this.name
        }

        this.getEntityId = () => {
            return this.entityId
        }
    }
}

class DefuserPickupEvent {
    constructor(name, entityId, player) {
        this.name = name
        this.entityId = entityId
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getEntityId = () => {
            return this.entityId
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class BeginDefuseEvent {
    constructor(name, hasKit, player) {
        this.name = name
        this.hasKit = hasKit
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.withKit = () => {
            return this.hasKit
        }

        this.player = () => {
            return this.player
        }
    }
}

class AbortDefuseEvent {
    constructor(name, player) {
        this.name = name
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class HostageBeginsFollowingEvent {
    constructor(name, hostage, player) {
        this.name = name
        this.hostage = hostage
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getHostage = () => {
            return this.hostage
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class HostageHurtEvent {
    constructor(name, hostage, player) {
        this.name = name
        this.hostage = hostage
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getHostage = () => {
            return this.hostage
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class HostageRescuedEvent {
    constructor(name, hostage, site, player) {
        this.name = name
        this.hostage = hostage
        this.site = site
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getHostage = () => {
            return this.hostage
        }

        this.getSite = () => {
            return this.site
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class HostageStopsFollowingEvent {
    constructor(name, hostage, player) {
        this.name = name
        this.hostage = hostage
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getHostage = () => {
            return this.hostage
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class WeaponFireEvent {
    constructor(name, weapon, silenced, player) {
        this.name = name
        this.weapon = weapon
        this.silenced = silenced
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getWeapon = () => {
            return this.weapon
        }

        this.isSilenced = () => {
            return this.silenced
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class WeaponReloadEvent {
    constructor(name, player) {
        this.name = name
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class WeaponZoomEvent {
    constructor(name, player) {
        this.name = name
        this.player = player

        this.getName = () => {
            return this.name
        }

        this.getPlayer = () => {
            return this.player
        }
    }
}

class PlayerBlindEvent {
    constructor(name, entityId, blindDuration, player, attacker) {
        this.name = name
        this.entityId = entityId
        this.blindDuration = blindDuration
        this.player = player
        this.attacker = attacker

        this.getName = () => {
            return this.name
        }

        this.getEntityId = () => {
            return this.entityId
        }

        this.getDuration = () => {
            return this.blindDuration
        }

        this.getPlayer = () => {
            return this.player
        }

        this.getAttacker = () => {
            return this.attacker
        }
    }
}

// Event constructors (metacall classes)
const _ItemPickupEvent = (name, item, silent, defindex, player) => {
    return new ItemPickupEvent(name, item, silent, defindex, player)
}

const _ClientConnectedEvent = (name, steamId, ip, isBot) => {
    return new ClientConnectedEvent(name, steamId, ip, isBot)
}

const _ClientConnectEvent = (name, steamId) => {
    return new ClientConnectEvent(name, steamId)
}

const _ClientPutInServerEvent = (name, player) => {
    return new ClientPutInServerEvent(name, player)
}

const _ClientDisconnectedEvent = (name, player) => {
    return new ClientDisconnectedEvent(name, player)
}

const _PlayerActivateEvent = (name, player) => {
    return new PlayerActivateEvent(name, player)
}

const _PlayerSpawnEvent = (name, player) => {
    return new PlayerSpawnEvent(name, player)
}

const _PlayerChangeTeamEvent = (name, team, oldTeam, disconnect, silent, player) => {
    return new PlayerChangeTeamEvent(name, team, oldTeam, disconnect, silent, player)
}

const _PlayerHurtEvent = (name, hp, armor, weapon, damageHp, damageArmor, hitGoup, player, attacker) => {
    return new PlayerHurtEvent(name, hp, armor, weapon, damageHp, damageArmor, hitGoup, player, attacker)
}

const _MapShutdownEvent = (name) => {
    return new MapShutdownEvent(name)
}

const _PlayerChatEvent = (name, text, teamOnly, player) => {
    return new PlayerChatEvent(name, text, teamOnly, player)
}

const _GameMessageEvent = (name, text, target) => {
    return new GameMessageEvent(name, text, target)
}

const _MapLoadedEvent = (name, map, oldMap) => {
    return new MapLoadedEvent(name, map, oldMap)
}

const _RoundEndEvent = (name, winner, reason, message, legacy, playerCount, noMusic) => {
    return new RoundEndEvent(name, winner, reason, message, legacy, playerCount, noMusic)
}

const _RoundStartEvent = (name, timeLimit, fragLimit, objective) => {
    return new RoundStartEvent(name, timeLimit, fragLimit, objective)
}

const _FreezeTimeEndedEvent = (name) => {
    return new FreezeTimeEndedEvent(name)
}

const _PlayerDeathEvent = (name, assistFlash, weapon, weaponItemId, weaponFauxItemId, weaponOriginalOwnerSteamId, headshot, dominated, revenge, wipe, penetrated, noReplay, noScope, throughSmoke, attackerBlind, distance, player, attacker, assister) => {
    return new PlayerDeathEvent(name, assistFlash, weapon, weaponItemId, weaponFauxItemId, weaponOriginalOwnerSteamId, headshot, dominated, revenge, wipe, penetrated, noReplay, noScope, throughSmoke, attackerBlind, distance, player, attacker, assister)
}

const _PlayerFootstepEvent = (name, player) => {
    return new PlayerFootstepEvent(name, player)
}

const _BreakPropEvent = (name, entityId, player) => {
    return new BreakPropEvent(name, entityId, player)
}

const _ItemPurchaseEvent = (name, team, loadout, weapon, player) => {
    return new ItemPurchaseEvent(name, team, loadout, weapon, player)
}

const _BombBeginPlantEvent = (name, site, player) => {
    return new BombBeginPlantEvent(name, site, player)
}

const _BombPlantedEvent = (name, site, player) => {
    return new BombPlantedEvent(name, site, player)
}

const _BombDefusedEvent = (name, site, player) => {
    return new BombDefusedEvent(name, site, player)
}

const _BombExplodedEvent = (name, site, player) => {
    return new BombExplodedEvent(name, site, player)
}

const _BombDroppedEvent = (name, entityId, player) => {
    return new BombDroppedEvent(name, entityId, player)
}

const _BombPickedUpEvent = (name, player) => {
    return new BombPickedUpEvent(name, player)
}

const _DefuserDroppedEvent = (name, entityId) => {
    return new DefuserDroppedEvent(name, entityId)
}

const _DefuserPickupEvent = (name, entityId, player) => {
    return new DefuserPickupEvent(name, entityId, player)
}

const _BeginDefuseEvent = (name, hasKit, player) => {
    return new BeginDefuseEvent(name, hasKit, player)
}

const _AbortDefuseEvent = (name, player) => {
    return new AbortDefuseEvent(name, player)
}

const _HostageBeginsFollowingEvent = (name, hostage, player) => {
    return new HostageBeginsFollowingEvent(name, hostage, player)
}

const _HostageHurtEvent = (name, hostage, player) => {
    return new HostageHurtEvent(name, hostage, player)
}

const _HostageRescuedEvent = (name, hostage, site, player) => {
    return new HostageRescuedEvent(name, hostage, site, player)
}

const _HostageStopsFollowingEvent = (name, hostage, player) => {
    return new HostageStopsFollowingEvent(name, hostage, player)
}

const _WeaponFireEvent = (name, weapon, silenced, player) => {
    return new WeaponFireEvent(name, weapon, silenced, player)
}

const _WeaponReloadEvent = (name, player) => {
    return new WeaponReloadEvent(name, player)
}

const _WeaponZoomEvent = (name, player) => {
    return new WeaponZoomEvent(name, player)
}

const _PlayerBlindEvent = (name, entityId, blindDuration, player, attacker) => {
    return new PlayerBlindEvent(name, entityId, blindDuration, player, attacker)
}

module.exports = {
    _onEventCall,

    _ItemPickupEvent,
    ItemPickupEvent,
    onItemPickup,

    _ClientPutInServerEvent,
    ClientPutInServerEvent,
    onClientPutInServer,

    _ClientConnectedEvent,
    ClientConnectedEvent,
    onClientConnected,

    _ClientConnectEvent,
    ClientConnectEvent,
    onClientConnect,

    _PlayerActivateEvent,
    PlayerActivateEvent,
    onPlayerActivate,

    _PlayerSpawnEvent,
    PlayerSpawnEvent,
    onPlayerSpawn,

    _PlayerChangeTeamEvent,
    PlayerChangeTeamEvent,
    onPlayerChangeTeam,

    _PlayerHurtEvent,
    PlayerHurtEvent,
    onPlayerHurt,

    _MapShutdownEvent,
    MapShutdownEvent,
    onMapShutdown,

    _PlayerChatEvent,
    PlayerChatEvent,
    onPlayerChat,

    _GameMessageEvent,
    GameMessageEvent,
    onGameMessage,

    _MapLoadedEvent,
    MapLoadedEvent,
    onMapLoaded,

    _RoundEndEvent,
    RoundEndEvent,
    onRoundEnded,

    _RoundStartEvent,
    RoundStartEvent,
    onRoundStarted,

    _FreezeTimeEndedEvent,
    FreezeTimeEndedEvent,
    onFreezeTimeEnded,

    _PlayerDeathEvent,
    PlayerDeathEvent,
    onPlayerDeath,

    _PlayerFootstepEvent,
    PlayerFootstepEvent,
    onPlayerFootstep,

    _BreakPropEvent,
    BreakPropEvent,
    onPropBreak,

    _ItemPurchaseEvent,
    ItemPurchaseEvent,
    onItemPurchase,

    _BombBeginPlantEvent,
    BombBeginPlantEvent,
    onBombBeginPlant,

    _BombPlantedEvent,
    BombPlantedEvent,
    onBombPlanted,

    _BombExplodedEvent,
    BombExplodedEvent,
    onBombExploded,

    _BombDefusedEvent,
    BombDefusedEvent,
    onBombDefused,

    _BombPickedUpEvent,
    BombPickedUpEvent,
    onBombPickup,

    _BombDroppedEvent,
    BombDroppedEvent,
    onBombDropped,

    _DefuserDroppedEvent,
    DefuserDroppedEvent,
    onDefuserDropped,

    _DefuserPickupEvent,
    DefuserPickupEvent,
    onDefuserPickup,

    _BeginDefuseEvent,
    BeginDefuseEvent,
    onBeginDefuse,

    _AbortDefuseEvent,
    AbortDefuseEvent,
    onAbortDefuse,

    _HostageBeginsFollowingEvent,
    HostageBeginsFollowingEvent,
    onHostageBeginsFollowing,

    _HostageHurtEvent,
    HostageHurtEvent,
    onHostageHurt,

    _HostageRescuedEvent,
    HostageRescuedEvent,
    onHostageRescued,

    _HostageStopsFollowingEvent,
    HostageStopsFollowingEvent,
    onHostageStopsFollowing,

    _WeaponFireEvent,
    WeaponFireEvent,
    onWeaponFire,

    _WeaponReloadEvent,
    WeaponReloadEvent,
    onWeaponReload,

    _WeaponZoomEvent,
    WeaponZoomEvent,
    onWeaponZoom,

    _PlayerBlindEvent,
    PlayerBlindEvent,
    onPlayerBlind,

    _ClientDisconnectedEvent,
    ClientDisconnectedEvent,
    onClientDisconnected,

    onEvent,
}