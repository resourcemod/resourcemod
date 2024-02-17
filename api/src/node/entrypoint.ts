// @ts-expect-error Cannot find module '../../../../resourcemod.config'
import config from "../../../../resourcemod.config"

export const _LoadEntrypoint = () => {
    return config.entrypoint ?? "src/server.js"
}