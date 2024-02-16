/**
 * =============================================================================
 * CS2Fixes
 * Copyright (C) 2023 Source2ZE
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "cbaseentity.h"
#include "cbasemodelentity.h"
#include "cbaseplayercontroller.h"
#include "Memory.h"
#include "services.h"

extern Memory* g_Memory;

class CBasePlayerPawn : public CBaseModelEntity
{
public:
    DECLARE_SCHEMA_CLASS(CBasePlayerPawn);

    SCHEMA_FIELD(CPlayer_MovementServices*, m_pMovementServices)
    SCHEMA_FIELD(CPlayer_WeaponServices*, m_pWeaponServices)
    SCHEMA_FIELD(CCSPlayer_ItemServices*, m_pItemServices)
    SCHEMA_FIELD(CHandle<CBasePlayerController>, m_hController)

    void CommitSuicide(bool bExplode, bool bForce)
    {
        static int offset = g_Memory->offsets["CBasePlayerPawn_CommitSuicide"];
        CALL_VIRTUAL(void, offset, this, bExplode, bForce);
    }

    void Slap(int hp) {
        if (m_iHealth() - hp <= 0) {
            this->CommitSuicide(true, true);
            return;
        }
        this->SetHP(m_iHealth() - hp);
    }

    int GetHP() {
        return m_iHealth();
    }

    void SetHP(int health) {
        if (health <= 0) {
            this->CommitSuicide(false, true);
            return;
        }
        m_iHealth = health;
    }

    CBasePlayerController *GetController() { return m_hController.Get(); }
};