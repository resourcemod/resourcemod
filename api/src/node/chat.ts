// @ts-ignore
import { metacall } from "metacall"
import { GAME_MESSAGE_TARGET } from "./constants"

export const sayToEveryone = (message: string, destination: GAME_MESSAGE_TARGET = GAME_MESSAGE_TARGET.CHAT) => {
    if (message.match(/^\\x\d+/gm)) message = ` ${message}`
    metacall('_AllPrint', destination, message)
}

export const sayToSlot = (slot: number, message: string, destination: GAME_MESSAGE_TARGET = GAME_MESSAGE_TARGET.CHAT) => {
    if (message.match(/^\\x\d+/gm)) message = ` ${message}`
    metacall('_PlayerPrint', slot, destination, message)
}

export const colors = {
    DEFAULT: "\x01",
    DARK_RED: "\x02",
    PURPLE: "\x03",
    GREEN: "\x04",
    LIGHT_GREEN: "\x05",
    LIME_GREEN: "\x06",
    RED: "\x07",
    GREY: "\x08",
    ORANGE: "\x09",
}

export default {
    sayToEveryone,
    sayToSlot,
    colors
}