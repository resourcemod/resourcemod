// @ts-ignore
import { metacall } from "metacall"
import { HUD_PRINT_TALK } from "./constants"

export const sayToEveryone = (message: string, destination = HUD_PRINT_TALK) => {
    metacall('_AllPrint', destination, message)
}

export const sayToSlot = (slot: any, message: string, destination = HUD_PRINT_TALK) => {
    metacall('_PlayerPrint', slot, destination, message)
}

export default {
    sayToEveryone,
    sayToSlot
}