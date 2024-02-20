import {Vec3} from 'vec3'
// @ts-expect-error
import {metacall} from 'metacall'

export class Transform {
    constructor(
        private readonly _id: number,
        private readonly _slot: number
    ) {}

    get position() : Vec3 {
        // again, no classes :'(
        const metacallObject = metacall('_EntityGetCoords', this._id, this._slot)
        return new Vec3(metacallObject.x, metacallObject.y, metacallObject.z)
    }

    set position(position: Vec3) {
        metacall('_EntitySetCoords', this._id, this._slot, position.x, position.y, position.z)
    }
}