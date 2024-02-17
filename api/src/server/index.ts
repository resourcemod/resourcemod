// @ts-expect-error Could not find a declaration file for module 'metacall'
import { metacall } from "metacall"
import { Players } from "./players"
import { GameMessage } from "./message";

export const server = {
    players: Players,
    message: (slot: number, message: GameMessage, duration: number): boolean => {
        return metacall("_GameMessage", slot, message.message, duration);
    }
}
