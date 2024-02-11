const config = require('../../../resourcemod.config.js')

const _LoadEntrypoint = () => {
    return config.entrypoint ?? "src/server.js"
}

module.exports = {
    _LoadEntrypoint
}