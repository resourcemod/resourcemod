// @ts-ignore
import { metacall } from "metacall"
import { Players } from './players'
import { GameMessage } from "./message";

export default {
    players: Players,
    message: (slot: number, message: GameMessage, duration: number): boolean => {
        return metacall('_GameMessage', slot, message.message, duration);
    }
}
