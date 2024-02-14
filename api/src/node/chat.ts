// @ts-ignore
import { metacall } from "metacall"
import { GAME_MESSAGE_TARGET } from "./constants"

export const sayToEveryone = (message: string, destination: GAME_MESSAGE_TARGET = GAME_MESSAGE_TARGET.CHAT) => {
    metacall('_AllPrint', destination, message)
}

export const sayToSlot = (slot: number, message: string, destination: GAME_MESSAGE_TARGET = GAME_MESSAGE_TARGET.CHAT) => {
    metacall('_PlayerPrint', slot, destination, message)
}

export default {
    sayToEveryone,
    sayToSlot
}