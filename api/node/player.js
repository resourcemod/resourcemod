const { metacall } = require("metacall")
const { sayToSlot } = require("./chat")
const { HUD_PRINT_CENTER } = require("./constants")

class Player {
    name
    steamId
    slot
    constructor(name, steamId, slot) {
        this.name = name
        this.steamId = steamId
        this.slot = slot

        this.getHp = () => {
            return metacall('_PlayerGetHP', this.slot)
        }
    
        this.setHp = (hp) => {
            return metacall('_PlayerSetHP', this.slot, hp)
        }
    
        this.getName = () => {
            return this.name
        }
    
        this.getSteamId = () => {
            return this.steamId === 0 ? undefined : this.steamId
        }
    
        this.getSlot = () => {
            return this.slot
        }
    
        this.getSteamId64 = () => {
            if (this.getSteamId() == undefined) return undefined // bots
            return (BigInt(this.getSteamId()) + BigInt(76561197960265728)).toString();
        }
    
        this.isAlive = () => {
            return metacall('_PlayerGetIsAlive', this.slot)
        }
    
        this.isConnected = () => {
            return metacall('_PlayerGetIsConnected', this.slot)
        }
    
        this.isDisconnected = () => {
            return metacall('_PlayerGetIsDisconnected', this.slot)
        }
    
        this.isConnecting = () => {
            return metacall('_PlayerGetIsConnecting', this.slot)
        }
    
        this.isDisconnecting = () => {
            return metacall('_PlayerGetIsDisconnecting', this.slot)
        }
    
        this.isReserved = () => {
            return metacall('_PlayerGetIsReserved', this.slot)
        }
    
        this.isReconnecting = () => {
            return metacall('_PlayerGetIsReconnecting', this.slot)
        }
    
        this.slap = (hp) => {
            return metacall('_PlayerSlap', this.slot, hp)
        }
    
        this.slay = () => {
            return metacall('_PlayerSlay', this.slot)
        }
    
        this.respawn = () => {
            return metacall('_PlayerRespawn', this.slot)
        }
    
        this.getTeam = () => {
            return metacall('_PlayerGetTeam', this.slot)
        }
    
        this.changeTeam = (team, kill) => {
            return metacall('_PlayerChangeTeam', this.slot, team, kill)
        }
    
        this.say = (message) => {
            return sayToSlot(this.slot, message)
        }
    
        this.hint = (message) => {
            return sayToSlot(this.slot, message, HUD_PRINT_CENTER)
        }

        this.setModel = (path) => {
            return metacall('_PlayerSetModel', this.slot, path)
        }

        this.setColor = (color) => {
            return metacall('_PlayerSetColor', this.slot, color)
        }

        this.playSound = (path) => {
            return metacall('_PlayerPlaySound', this.slot, path)
        }

        this.kick = () => {
            return metacall('_PlayerKick', this.slot)
        }
    }
}

const _Player = (name, steamId, slot) => {
    return new Player(name, steamId, slot)
}

module.exports = {
    _Player,
    Player
}