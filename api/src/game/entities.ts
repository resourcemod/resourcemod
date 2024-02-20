// @ts-ignore
import {metacall} from "metacall"
import {Transform} from './transform'
import {Vec3} from 'vec3'
import { Color } from "../node/color"

export enum EntityType {
    Player = '',
    PropDynamicOverride = 'prop_dynamic_override',
    PropPhysicsOverride = 'prop_physics_override',
}

export enum CollisionType {
    None = 0,
    BSP = 1,
    BBOX= 2,
    Sphere = 4,
    Point = 5,
    VPhysics = 6,
    Capsule = 7
}

export class Entity {
    private readonly id: number;
    private readonly player_slot: number;
    public spawned: boolean;
    constructor(type: EntityType = EntityType.Player, entityId: number = -1, slot: number = -1) {
        if (entityId === -1 && type !== EntityType.Player) { // to be able to spawn new props just by creating a new object
            const id = metacall('_EntityCreate', type)
            this.id = id;
            this.spawned = false;
            this.player_slot = slot;
            return;
        }
        this.spawned = true;
        this.player_slot = slot;
        this.id = entityId;
    }

    set model(model: string) {
        metacall('_EntitySetModel', this.id, model)
    }

    get transform(): Transform {
        return new Transform(this.id, this.player_slot);
    }

    set color(color: Color) {
        metacall('_EntitySetColor', this.id, color)
    }

    get color(): Color {
        const color = metacall('_EntityGetColor', this.id);
        return new Color(color.red, color.green, color.blue, color.alpha);
    }

    set collision(type: CollisionType) {
        metacall('_EntitySetCollision', this.id, type)
    }

    get collision(): CollisionType {
        return metacall('_EntityGetCollision', this.id) as CollisionType;
    }

    spawn(model: string): boolean {
        this.spawned = !!metacall('_EntitySpawn', this.id, model);
        return this.spawned
    }

    remove(): boolean {
        this.spawned = false;
        return metacall('_EntityRemove', this.id)
    }
}