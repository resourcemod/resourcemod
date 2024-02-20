//
// Created by Twelvee on 20.02.2024.
//
#include "Player.h"
#include "../../protobuf/generated/network_connection.pb.h"
#include <metacall/metacall.h>

class Player;

extern ResourceMod *g_ResourceMod;
extern IVEngineServer2 *g_SourceEngine;
extern Engine *g_Engine;

void *Player::GetHP(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    return metacall_value_create_int(c->GetPawn()->GetHP());
}

void *Player::SetHP(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    c->GetPawn()->SetHP(metacall_value_to_int(args[1]));
    return metacall_value_create_bool(true);
}

void *Player::GetMoney(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    return metacall_value_create_int(c->m_pInGameMoneyServices->m_iAccount());
}

void *Player::SetMoney(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    int money = metacall_value_to_int(args[1]);
    c->m_pInGameMoneyServices->m_iAccount = money;
    return metacall_value_create_bool(true);
}

void *Player::GetDamage(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    CSMatchStats_t *matchStats = &c->m_pActionTrackingServices->m_matchStats();
    return metacall_value_create_int(matchStats->m_iDamage.Get());
}

void *Player::GetKills(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    CSMatchStats_t *matchStats = &c->m_pActionTrackingServices->m_matchStats();
    return metacall_value_create_int(matchStats->m_iKills.Get());
}

void *Player::GetAssists(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    CSMatchStats_t *matchStats = &c->m_pActionTrackingServices->m_matchStats();
    return metacall_value_create_int(matchStats->m_iAssists.Get());
}

void *Player::GetDeaths(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    CSMatchStats_t *matchStats = &c->m_pActionTrackingServices->m_matchStats();
    return metacall_value_create_int(matchStats->m_iDeaths.Get());
}

void *Player::SetDamage(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    c->m_pActionTrackingServices->m_matchStats().m_iDamage = metacall_value_to_int(args[1]);
    return metacall_value_create_bool(true);
}

void *Player::SetKills(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    int amount = metacall_value_to_int(args[1]);
    c->m_pActionTrackingServices->m_matchStats().m_iKills = amount;
    return metacall_value_create_bool(true);
}

void *Player::SetAssists(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    c->m_pActionTrackingServices->m_matchStats().m_iAssists = metacall_value_to_int(args[1]);
    return metacall_value_create_bool(true);
}

void *Player::SetDeaths(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    c->m_pActionTrackingServices->m_matchStats().m_iDeaths = metacall_value_to_int(args[1]);
    return metacall_value_create_bool(true);
}

void *Player::GetArmor(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    CCSPlayerPawnBase *pawn = (CCSPlayerPawnBase *) c->m_hPlayerPawn().Get();
    if (!pawn) {
        return metacall_value_create_int(0);
    }
    return metacall_value_create_int(pawn->GetArmor());
}

void *Player::SetArmor(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    CCSPlayerPawnBase *pawn = (CCSPlayerPawnBase *) c->m_hPlayerPawn().Get();
    if (!pawn) {
        return metacall_value_create_bool(false);
    }
    pawn->SetArmor(metacall_value_to_int(args[1]));
    return metacall_value_create_bool(true);
}

void *Player::GetName(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    std::string name = c->GetPlayerName();
    return metacall_value_create_string(name.c_str(), name.length());
}

void *Player::GetSteamID(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    uint64 value = c->m_steamID;
    return metacall_value_create_long(value);
}

void *Player::GetIsAlive(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    return metacall_value_create_bool(c->IsAlive());
}

void *Player::Slap(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    c->GetPawn()->Slap(metacall_value_to_int(args[1]));
    return metacall_value_create_bool(true);
}

void *Player::Slay(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    c->GetPawn()->CommitSuicide(true, true);
    return metacall_value_create_bool(true);
}

void *Player::Respawn(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    return metacall_value_create_bool(c->Respawn());
}

void *Player::PrintGameMessage(size_t argc, void *args[], void *data) {
    int slot = metacall_value_to_int(args[0]);
    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    std::string message = metacall_value_to_string(args[1]);
    int duration = metacall_value_to_int(args[2]);

    if (message.size() == 0) {
        return metacall_value_create_bool(false);
    }

    if (duration == 0) {
        return metacall_value_create_bool(false);
    }

    std::pair<uint64_t, std::string> pair = std::make_pair(Engine::Now() + (duration * 1000), message);
    g_Engine->gameMessages[slot] = pair;

    return metacall_value_create_bool(true);
}

void *Player::Play(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));

    std::string soundPath = "play ";
    soundPath.append(metacall_value_to_string(args[1]));
    if (c->GetPawn() != nullptr && c->m_steamID > 0)
        g_SourceEngine->ClientCommand(c->GetPlayerSlot(), soundPath.c_str());
    return metacall_value_create_bool(true);
}

void *Player::SetModel(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));

    std::string modelPath = metacall_value_to_string(args[1]);
    g_ResourceMod->NextFrame([c, modelPath]() {
        if (c->GetPawn() != nullptr)
            c->GetPawn()->SetModel(modelPath.c_str());
    });
    return metacall_value_create_bool(true);
}

void *Player::Kick(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));

    g_SourceEngine->DisconnectClient(c->GetPlayerSlot(), NETWORK_DISCONNECT_KICKED); //disconnect by server
    return metacall_value_create_bool(true);
}

void *Player::SetColor(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    void **v_module_map = static_cast<void **>(metacall_value_to_map(args[1]));
    int red = 0;
    int green = 0;
    int blue = 0;
    int alpha = 0;
    // fucked up but metacall can't work with classes objects properly for now
    for (int i = 0; i < metacall_value_size(args[1]) / sizeof(v_module_map[0]); ++i) {
        auto v = metacall_value_to_array(v_module_map[i]);
        if (V_strcmp(metacall_value_to_string(v[0]), "red") == 0) {
            red = int(metacall_value_to_double(v[1]));
        } else if (V_strcmp(metacall_value_to_string(v[0]), "green") == 0) {
            green = int(metacall_value_to_double(v[1]));
        } else if (V_strcmp(metacall_value_to_string(v[0]), "blue") == 0) {
            blue = int(metacall_value_to_double(v[1]));
        } else if (V_strcmp(metacall_value_to_string(v[0]), "alpha") == 0) {
            alpha = int(metacall_value_to_double(v[1]));
        }
    }
    Color color = Color(red, green, blue, alpha);
    g_ResourceMod->NextFrame([c, color]() {
        if (c->GetPawn() != nullptr)
            c->GetPawn()->SetColor(color);
    });
    return metacall_value_create_bool(true);
}

void *Player::GetTeam(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    return metacall_value_create_int(c->m_iTeamNum.Get());
}

void *Player::ChangeTeam(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));

    int team = metacall_value_to_int(args[1]);
    if (team < 0 || team > 3) {
        void *throwArgs[] = {
                metacall_value_create_string("Invalid team parameter. 0-3 range expected.",
                                             strlen("Invalid team parameter. 0-3 range expected.")),
        };
        metacallv("_throw_exception", throwArgs);
        return metacall_value_create_bool(false);
    }
    if (argc > 2) {
        bool kill = metacall_value_to_bool(args[2]);
        if (kill) {
            c->ChangeTeam(team);
        } else {
            c->SwitchTeam(team);
        }
    } else {
        c->SwitchTeam(team);
    }

    return metacall_value_create_bool(true);
}

void *Player::GetIsConnected(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    return metacall_value_create_bool(c->IsConnected());
}

void *Player::GetIsConnecting(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    return metacall_value_create_bool(c->IsConnecting());
}

void *Player::GetIsDisconnected(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    return metacall_value_create_bool(c->IsDisconnected());
}

void Player::ClientPrintAll(int hud_dest, const char *msg, ...) {
    va_list args;
            va_start(args, msg);
    char buf[256];
    V_vsnprintf(buf, sizeof(buf), msg, args);
            va_end(args);

    return SignatureCall::UTIL_ClientPrintAll(hud_dest, buf, nullptr, nullptr, nullptr, nullptr);
}

void Player::ClientPrint(CCSPlayerController *player, int dest, const char *text, ...) {
    va_list args;
            va_start(args, text);
    char buf[256];
    V_vsnprintf(buf, sizeof(buf), text, args);
            va_end(args);

    g_ResourceMod->NextFrame([player, buf, dest]() {
        if (player->m_hPawn() && player->m_steamID() > 0)
            SignatureCall::UTIL_ClientPrint(player, dest, buf, nullptr, nullptr, nullptr, nullptr);
    });
}

void *Player::PrintAll(size_t argc, void *args[], void *data) {
    int dest = metacall_value_to_int(args[0]);
    std::string msg = metacall_value_to_string(args[1]);

    Player::ClientPrintAll(dest, msg.c_str());

    return metacall_value_create_bool(true);
}

void *Player::Print(size_t argc, void *args[], void *data) {
    int slot = metacall_value_to_int(args[0]);
    int dest = metacall_value_to_int(args[1]);
    std::string msg = metacall_value_to_string(args[2]);

    CCSPlayerController *c = CCSPlayerController::FromSlot(slot);
    Player::ClientPrint(c, dest, msg.c_str());

    return metacall_value_create_bool(true);
}

void *Player::GetIsDisconnecting(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    return metacall_value_create_bool(c->IsDisconnecting());
}

void *Player::GetIsReserved(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    return metacall_value_create_bool(c->IsReserved());
}

void *Player::GetIsReconnecting(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    return metacall_value_create_bool(c->IsReconnecting());
}

void *Player::GetCoords(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    if (!c->GetPawn())
    {
        void *vectorArgs[] = {
                metacall_value_create_float(0),
                metacall_value_create_float(0),
                metacall_value_create_float(0)
        };

        void *vObj = metacallv("_CreateCoords", vectorArgs);

        metacall_value_destroy(vectorArgs[0]);
        metacall_value_destroy(vectorArgs[1]);
        metacall_value_destroy(vectorArgs[2]);

        return vObj;
    }

    auto vector = c->GetPawn()->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin();
    void *vectorArgs[] = {
            metacall_value_create_float(vector.x),
            metacall_value_create_float(vector.y),
            metacall_value_create_float(vector.z)
    };

    void *vObj = metacallv("_CreateCoords", vectorArgs);

    metacall_value_destroy(vectorArgs[0]);
    metacall_value_destroy(vectorArgs[1]);
    metacall_value_destroy(vectorArgs[2]);

    return vObj;
}

void *Player::SetCoords(size_t argc, void *args[], void *data) {
    CCSPlayerController *c = CCSPlayerController::FromSlot(metacall_value_to_int(args[0]));
    CCSPlayerPawn *pawn = c->GetPawn();
    if (!pawn)
        return metacall_value_create_bool(false);
    auto x = metacall_value_to_float(args[1]);
    auto y = metacall_value_to_float(args[2]);
    auto z = metacall_value_to_float(args[3]);
    logger::log(logger::format("%f, %f, %f", x, y, z));
    Vector vec(x, y, z);
    pawn->m_CBodyComponent->m_pSceneNode->m_vecAbsOrigin = vec;
    return metacall_value_create_bool(true);
}