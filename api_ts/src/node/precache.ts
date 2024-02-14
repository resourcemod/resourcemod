import fs from "fs"
import path from "path"

const mainPath = path.join(__dirname, '../../../resourcemod.config.js')
const config = fs.existsSync(mainPath) ? require(mainPath) : {}

export const _LoadPrecache = () => {
    return config.precache
}

export default _LoadPrecache