//
// Created by Twelvee on 08.10.2023.
//

#ifndef RESOURCEMOD_RESOURCEMOD_H
#define RESOURCEMOD_RESOURCEMOD_H


#include <ISmmPlugin.h>
#include <functional>
#include <deque>

#ifdef _WIN32
#define WIN_LINUX(win, linux) win
#else
#define WIN_LINUX(win, linux) linux
#endif

class ResourceMod final : public ISmmPlugin, public IMetamodListener
{
public:
    bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late);
    void Hook_StartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession*, const char*);
    void Hook_GameFrame(bool simulating, bool bFirstTick, bool bLastTick);
    void OnLevelInit(char const *pMapName, char const *pMapEntities, char const *pOldLevel, char const *pLandmarkName, bool loadGame, bool background);
    bool Unload(char* error, size_t maxlen);
    void LoadDependencies();
    void NextFrame(std::function<void()> fn);
    void RMFrame();

private:
    const char* GetAuthor();
    const char* GetName();
    const char* GetDescription();
    const char* GetURL();
    const char* GetLicense();
    const char* GetVersion();
    const char* GetDate();
    const char* GetLogTag();
    std::deque<std::function<void()>> m_nextFrame;
};

#endif //RESOURCEMOD_RESOURCEMOD_H
