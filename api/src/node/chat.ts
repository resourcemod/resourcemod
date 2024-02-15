// @ts-ignore
import { metacall } from "metacall"
import { GameMessageTarget } from "./constants"

export const sayToEveryone = (message: string, destination: GameMessageTarget = GameMessageTarget.Chat) => {
    if (message.match(/^\\x\d+/gm)) message = ` ${message}`
    metacall('_AllPrint', destination, message)
}

export const sayToSlot = (slot: number, message: string, destination: GameMessageTarget = GameMessageTarget.Chat) => {
    if (message.match(/^\\x\d+/gm)) message = ` ${message}`
    metacall('_PlayerPrint', slot, destination, message)
}

export const colors = {
    DEFAULT: "\x01",
    DARK_RED: "\x02",
    LIGHT_PURPLE: "\x03",
    GREEN: "\x04",
    OLIVE: "\x05",
    LIME: "\x06",
    RED: "\x07",
    GREY: "\x08",
    YELLOW: "\x09",
    SILVER: "\x0A",
    LIGHT_BLUE: "\x0B",
    BLUE: "\x0C",
    PURPLE: "\x0E",
    LIGHT_RED: "\x0F",
    ORANGE: "\x10"
}

export default {
    sayToEveryone,
    sayToSlot,
    colors
}