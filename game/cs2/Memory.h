//
// Created by Twelvee on 16.11.2023.
//

#ifndef RESOURCEMOD_MEMORY_H
#define RESOURCEMOD_MEMORY_H

#include <map>
#include "../../logger/logger.h"
#include "Module.h"

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#ifdef _WIN32
#define ROOTBIN "/bin/win64/"
#define GAMEBIN "/csgo/bin/win64/"
#else
#define ROOTBIN "/bin/linuxsteamrt64/"
#define GAMEBIN "/csgo/bin/linuxsteamrt64/"
#endif

#define HUD_PRINTNOTIFY		1
#define HUD_PRINTCONSOLE	2
#define HUD_PRINTTALK		3
#define HUD_PRINTCENTER		4

#define RESOLVE_SIG(mem, name, variable) \
	variable = (decltype(variable))mem->ResolveSignature(name);	\
	if (!variable)														\
		return false;

class CBasePlayerController;
class CCSPlayerController;
class CCSPlayerPawn;
class CBaseModelEntity;

namespace SignatureCall {
    inline void(FASTCALL *UTIL_ClientPrint)(CBasePlayerController *player, int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4);
    inline void(FASTCALL *UTIL_ClientPrintAll)(int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4);
    inline void(FASTCALL *CBasePlayerController_SetPawn)(CBasePlayerController *pController, CCSPlayerPawn *pPawn, bool a3, bool a4);
    inline void(FASTCALL *CCSPlayerController_SwitchTeam)(CCSPlayerController *pController, uint32 team);
    inline void(FASTCALL *CBaseModelEntity_SetModel)(CBaseModelEntity *pModel, const char *szModel);
}

class Memory {
public:
    std::map<std::string, int> offsets;
    std::map<std::string, std::string> signatures;
    std::map<std::string, std::string> libraries;

    CModule *_engineModule;
    CModule *_tier0Module;
    CModule *_serverModule;
    CModule *_vscriptModule;

    void LoadOffsets(std::string path) {
        std::ifstream f(path);
        json list = json::parse(f);
        for (auto element = list["offsets"].begin(); element != list["offsets"].end(); ++element) {
            std::string v = element.value().find("windows").value();
            this->offsets[element.key()] = atoi(v.c_str());
        }
    }

    void LoadSignatures(std::string path) {
        std::ifstream f(path);
        json list = json::parse(f);
        for (auto& [k, v] : list["signatures"].items()) {
            this->signatures[k] = v["windows"].get<std::string>();
            this->libraries[k] = v["library"].get<std::string>();
        }
    }

    bool MakeSignaturesCallable() {
        _engineModule = new CModule(ROOTBIN, "engine2");
        _tier0Module = new CModule(ROOTBIN, "tier0");
        _serverModule = new CModule(GAMEBIN, "server");
        _vscriptModule = new CModule(ROOTBIN, "vscript");

        RESOLVE_SIG(this, "UTIL_ClientPrint", SignatureCall::UTIL_ClientPrint);
        RESOLVE_SIG(this, "UTIL_ClientPrintAll", SignatureCall::UTIL_ClientPrintAll);
        RESOLVE_SIG(this, "CBasePlayerController_SetPawn", SignatureCall::CBasePlayerController_SetPawn);
        RESOLVE_SIG(this, "CCSPlayerController_SwitchTeam", SignatureCall::CCSPlayerController_SwitchTeam);
        RESOLVE_SIG(this, "CBaseModelEntity_SetModel", SignatureCall::CBaseModelEntity_SetModel);
    }

    bool IsSymbol(const char *name) {
        const char *sigOrSymbol = this->signatures[name].c_str();
        if (!sigOrSymbol || strlen(sigOrSymbol) <= 0) {
            logger::log(logger::format("Missing signature or symbol (%s)", name));
            return false;
        }
        return sigOrSymbol[0] == '@';
    }

    const char *GetSymbol(const char *name) {
        const char *symbol = this->signatures[name].c_str();

        if (!symbol || strlen(symbol) <= 1) {
            logger::log(logger::format("Missing symbol (%s)", name));
            return nullptr;
        }
        return symbol + 1;
    }

    CModule **GetModule(std::string name) {
        const char *library = this->libraries[name].c_str();
        if (strcmp(library, "engine") == 0)
            return &_engineModule;
        else if (strcmp(library, "server") == 0) {
            return &_serverModule;
        }else if (strcmp(library, "vscript") == 0)
            return &_vscriptModule;
        else if (strcmp(library, "tier0") == 0)
            return &_tier0Module;
        return nullptr;
    }

    void *ResolveSignature(std::string name) {
        CModule **module = this->GetModule(name);
        if (!module || !(*module)) {
            return nullptr;
        }

        void *address = nullptr;
        if (this->IsSymbol(name.c_str())) {
            const char *symbol = this->GetSymbol(name.c_str());
            if (!symbol) {
                return nullptr;
            }
            address = dlsym((*module)->m_hModule, symbol);
        } else {
            const char *signature = this->signatures[name].c_str();
            if (!signature) {
                return nullptr;
            }

            size_t iLength = 0;
            byte *pSignature = HexToByte(signature, iLength);
            if (!pSignature)
                return nullptr;
            address = (*module)->FindSignature(pSignature, iLength);
        }

        if (!address) {
            logger::log(logger::format("Failed to find address (%s).", name.c_str()));
            return nullptr;
        }
        return address;
    }

    int HexStringToUint8Array(const char* hexString, uint8_t* byteArray, size_t maxBytes)
    {
        if (!hexString) {
            logger::log("Empty hex string.");
            return -1;
        }
        size_t hexStringLength = strlen(hexString);
        size_t byteCount = hexStringLength / 4; // Each "\\x" represents one byte.

        if (hexStringLength % 4 != 0 || byteCount == 0 || byteCount > maxBytes) {
            logger::log("Invalid hex string format or byte count.");
            return -1; // Return an error code.
        }

        for (size_t i = 0; i < hexStringLength; i += 4) {
            if (sscanf(hexString + i, "\\x%2hhX", &byteArray[i / 4]) != 1) {
                logger::log(logger::format("Failed to parse hex string at position %zu.", i));
                return -1; // Return an error code.
            }
        }
        byteArray[byteCount] = '\0'; // Add a null-terminating character.

        return byteCount; // Return the number of bytes successfully converted.
    }

    byte *HexToByte(const char *src, size_t &length)
    {
        if (!src || strlen(src) <= 0)
        {
            logger::log(logger::format("Invalid hex str (%s).", src));
            return nullptr;
        }

        length = strlen(src) / 4;
        uint8_t *dest = new uint8_t[length];
        int byteCount = HexStringToUint8Array(src, dest, length);
        if (byteCount <= 0)
        {
            logger::log(logger::format("Invalid hex str (%s).", src));
            return nullptr;
        }
        return (byte *)dest;
    }
};

#endif RESOURCEMOD_MEMORY_H
