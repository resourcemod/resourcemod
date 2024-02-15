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

export enum Colors {
    Default = "\x01",
    DarkRed = "\x02",
    LightPurple = "\x03",
    Green = "\x04",
    Olive = "\x05",
    Lime = "\x06",
    Red = "\x07",
    Grey = "\x08",
    Yellow = "\x09",
    Silver = "\x0A",
    LightBlue = "\x0B",
    Blue = "\x0C",
    Purple = "\x0E",
    LightRed = "\x0F",
    Orange = "\x10"
}