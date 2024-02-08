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

#include "cs2/Memory.h"
#include "GameSystem.h"

#include <utility>
#include "../engine/Engine.h"

extern Memory *g_Memory;

CBaseGameSystemFactory **CBaseGameSystemFactory::sm_pFirst = nullptr;

CGameSystem g_GameSystem;
IGameSystemFactory *CGameSystem::sm_Factory = nullptr;

std::vector<std::string> precache;

// This mess is needed to get the pointer to sm_pFirst so we can insert game systems
bool InitGameSystems(std::vector<std::string> list)
{
    precache = std::move(list);
    // This signature directly points to the instruction referencing sm_pFirst, and the opcode is 3 bytes so we skip those
    uint8 *ptr = (uint8*)g_Memory->ResolveSignature("IGameSystem_InitAllSystems_pFirst") + 3;

    if (!ptr)
    {
        return false;
    }

    // Grab the offset as 4 bytes
    uint32 offset = *(uint32*)ptr;

    // Go to the next instruction, which is the starting point of the relative jump
    ptr += 4;

    // Now grab our pointer
    CBaseGameSystemFactory::sm_pFirst = (CBaseGameSystemFactory **)(ptr + offset);

    // And insert the game system(s)
    CGameSystem::sm_Factory = new CGameSystemStaticFactory<CGameSystem>("CS2Fixes_GameSystem", &g_GameSystem);

    return true;
}

GS_EVENT_MEMBER(CGameSystem, BuildGameSessionManifest)
{
    IEntityResourceManifest *pResourceManifest = msg->m_pResourceManifest;
    for (std::string item: precache) {
        pResourceManifest->AddResource(item.c_str());
    }
}