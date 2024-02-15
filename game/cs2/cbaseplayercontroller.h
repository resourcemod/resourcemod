#pragma once

#include "ehandle.h"
#include "cbaseentity.h"
#include "ccsplayerpawn.h"

extern IVEngineServer2 *g_SourceEngine;

enum class PlayerConnectedState : uint32_t {
    PlayerNeverConnected = 0xFFFFFFFF,
    PlayerConnected = 0x0,
    PlayerConnecting = 0x1,
    PlayerReconnecting = 0x2,
    PlayerDisconnecting = 0x3,
    PlayerDisconnected = 0x4,
    PlayerReserved = 0x5,
};

extern CEntitySystem *g_pEntitySystem;

class CBasePlayerController : public Z_CBaseEntity {
public:
    DECLARE_SCHEMA_CLASS(CBasePlayerController);

    SCHEMA_FIELD(uint64, m_steamID)

    SCHEMA_FIELD(CHandle<CCSPlayerPawn>, m_hPawn)

    SCHEMA_FIELD_POINTER(char, m_iszPlayerName)

    SCHEMA_FIELD(PlayerConnectedState, m_iConnected)

    CCSPlayerPawn *GetPawn() { return m_hPawn.Get(); }

    const char *GetPlayerName() { return m_iszPlayerName(); }

    int GetPlayerSlot() { return entindex() - 1; }

    bool IsConnected() { return m_iConnected() == PlayerConnectedState::PlayerConnected; }
    bool IsConnecting() { return m_iConnected() == PlayerConnectedState::PlayerConnecting; }
    bool IsReconnecting() { return m_iConnected() == PlayerConnectedState::PlayerReconnecting; }
    bool IsDisconnected() { return m_iConnected() == PlayerConnectedState::PlayerDisconnected; }
    bool IsDisconnecting() { return m_iConnected() == PlayerConnectedState::PlayerDisconnecting; }
    bool IsReserved() { return m_iConnected() == PlayerConnectedState::PlayerReserved; }

    void SetPawn(CCSPlayerPawn *pawn) {
        SignatureCall::CBasePlayerController_SetPawn(this, pawn, true, false);
    }
};