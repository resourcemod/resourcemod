//
// Created by Twelvee on 23.01.2024.
//

#ifndef RESOURCEMOD__RESOURCEMOD_LOADER_H
#define RESOURCEMOD__RESOURCEMOD_LOADER_H

#include <ISmmPlugin.h>
#include <functional>
#include <deque>

#ifdef _WIN32
#define WIN_LINUX(win, linux) win
#else
#define WIN_LINUX(win, linux) linux
#endif

class _resourcemod_loader final : public ISmmPlugin, public IMetamodListener
{
public:
    bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late);
    bool Unload(char* error, size_t maxlen);
    void LoadDependencies();

private:
    const char* GetAuthor();
    const char* GetName();
    const char* GetDescription();
    const char* GetURL();
    const char* GetLicense();
    const char* GetVersion();
    const char* GetDate();
    const char* GetLogTag();
};


#endif //RESOURCEMOD__RESOURCEMOD_LOADER_H
