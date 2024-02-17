import * as events from "./node/events"
import "./node/stdout"
import "./node/precache"
import * as chat from "./node/chat"
import { Color } from "./node/color"
import * as constants from "./node/constants"
import "./node/exceptions"
import { Player } from "./node/player"
import { server } from "./server/index"
import { GameMessage } from "./server/message"

declare global {
    // eslint-disable-next-line @typescript-eslint/no-namespace
    namespace NodeJS {
        interface WriteStream {
            _orig_write: NodeJS.WriteStream["write"];
        }
    }
}

export {
    events,
    chat,
    Color,
    constants,
    Player,
    server,
    GameMessage
}