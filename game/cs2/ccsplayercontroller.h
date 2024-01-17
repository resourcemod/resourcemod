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

#include "cbaseplayercontroller.h"
#include "services.h"

#define CS_TEAM_NONE        0
#define CS_TEAM_SPECTATOR   1
#define CS_TEAM_T           2
#define CS_TEAM_CT          3

extern CEntitySystem *g_pEntitySystem;

class CCSPlayerController : public CBasePlayerController {
public:
    DECLARE_SCHEMA_CLASS(CCSPlayerController);

    SCHEMA_FIELD(CCSPlayerController_InGameMoneyServices*, m_pInGameMoneyServices)

    SCHEMA_FIELD(CCSPlayerController_ActionTrackingServices*, m_pActionTrackingServices)

    SCHEMA_FIELD(bool, m_bPawnIsAlive);

    SCHEMA_FIELD(CHandle<CCSPlayerPawn>, m_hPlayerPawn);

    static CCSPlayerController *FromPawn(CCSPlayerPawn *pawn) {
        return (CCSPlayerController *) pawn->m_hController().Get();
    }

    static CCSPlayerController *FromSlot(CPlayerSlot slot) {
        return (CCSPlayerController *) g_pEntitySystem->GetBaseEntity(CEntityIndex(slot.Get() + 1));
    }

    void ChangeTeam(int iTeam) {
        static int offset = g_Memory->offsets["CCSPlayerController_ChangeTeam"];
        CALL_VIRTUAL(void, offset, this, iTeam);
    }

    void SwitchTeam(int iTeam) {
        if (!IsController())
            return;

        if (iTeam == CS_TEAM_SPECTATOR) {
            ChangeTeam(iTeam);
        } else {
            SignatureCall::CCSPlayerController_SwitchTeam(this, iTeam);
        }
    }

    bool Respawn() {
        CCSPlayerPawn *pPawn = m_hPlayerPawn.Get();
        if (!pPawn || pPawn->IsAlive())
            return false;

        SetPawn(pPawn);
        static int offset = g_Memory->offsets["CCSPlayerController_Respawn"];
        CALL_VIRTUAL(void, offset, this);
        return true;
    }
};