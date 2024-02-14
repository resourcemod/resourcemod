import events from './node/events'
import './node/stdout'
import './node/precache'
import chat from './node/chat'
import { Color } from './node/color'
import constants from './node/constants'
import './node/exceptions'
import { Player } from './node/player'

declare global {
    namespace NodeJS {
        interface WriteStream {
            _orig_write: NodeJS.WriteStream['write'];
        }
    }
}

export {
    events,
    chat,
    Color,
    constants,
    Player
}

export default {
    events,
    chat,
    Color,
    constants,
    Player
}