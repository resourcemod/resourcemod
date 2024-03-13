const config = require('../../../../resourcemod.config')

export const _LoadEntrypoint = () => {
    return config.entrypoint ?? "src/server.js"
}