import config from "../../../../resourcemod.config"

export const _LoadEntrypoint = () => {
    return config.entrypoint ?? "src/server.js"
}