const { metacall } = require("metacall")
const { HUD_PRINT_TALK } = require("./constants")

const sayToEveryone = (message, destination = HUD_PRINT_TALK) => {
    metacall('_AllPrint', destination, message)
}

const sayToSlot = (slot, message, destination = HUD_PRINT_TALK) => {
    metacall('_PlayerPrint', slot, destination, message)
}

module.exports = {
    sayToEveryone,
    sayToSlot
}