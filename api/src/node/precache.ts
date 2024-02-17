// @ts-expect-error Cannot find module '../../../../resourcemod.config'
import config from "../../../../resourcemod.config"

export const _LoadPrecache = () => {
    return config.precache
}