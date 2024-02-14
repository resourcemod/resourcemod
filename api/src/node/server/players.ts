import { Player } from '../player';
import { onEvent } from '../events';

const players = new Map<number, Player>();

onEvent('client_put_in_server', (event) => {
    if (players.has(event.player.slot)) {
        console.error(`[Metamod] Slot (${event.player.slot}) already exists in players map. This should not happen.`)
    }
    players.set(event.player.slot, event.player);
});

onEvent('client_disconnected', (event) => {
    if (!players.has(event.player.slot)) {
        return console.error(`[Metamod] Slot (${event.player.slot}) does not exist in players map. This should not happen.`)
    }
    players.delete(event.player.slot);
});

export class Players {
    static getPlayer(slot: number) {
        return players.get(slot);
    }

    static get allPlayers() {
        return Array.from(players.values());
    }

    static get allHumanPlayers() {
        return Array.from(players.values()).filter(player => !player.isBot);
    }
}

export default Players;