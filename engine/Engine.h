//
// Created by Twelvee on 13.09.2023.
//

#ifndef RESOURCEMOD_ENGINE_H
#define RESOURCEMOD_ENGINE_H

#include <string>
#include <windows.h>
#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include "TSQueue.h"

#ifndef IS_INTERNAL_RUNTIME
#else

#include "Plugin.h"
#include "igameevents.h"
#include "libplatform/libplatform.h"
#include "v8-context.h"
#include "v8-initialization.h"
#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"
#include "v8-script.h"

#endif

#define EXPORT __declspec(dllexport)

#define CS_TEAM_NONE        0
#define CS_TEAM_SPECTATOR   1
#define CS_TEAM_T           2
#define CS_TEAM_CT          3

#define HUD_PRINTNOTIFY        1
#define HUD_PRINTCONSOLE    2
#define HUD_PRINTTALK        3
#define HUD_PRINTCENTER        4

EXPORT class Engine {
public:
    const std::string RESOURCEMOD_META = "resourcemod.json";
    const std::string RESOURCEMOD_PLUGINS = "plugins.json";
    //const std::string RESOURCEMOD_EXTENSIONS = "extensions.json";

    std::string rootPath = "";
    std::string resourcemodFolder = "";
    std::string pluginsFolder = "";
    //std::string extensionsFolder = "";
    std::string gameDataPath = "";
    std::string metaPath = "";
    std::string pluginsPath = "";
    //std::string extensionsPath = "";

    bool isRunning = false;
    bool isExternalRuntimeRunning = false;

    void Init();

    // Wait for async jobs like timers or web requests
    static void RunEventLoop();

    // External Runtime
    static bool RunExternalRuntime(bool console);

    std::string pushAddr;
    std::string pullAddr;

    // event name and json data
    static void SendNodeMessage(std::string, std::string);

    TSQueue<const char *> rtEvents;
    TSQueue<const char *> rtCallbacks;

    static void HandleSocketMessage();

    // Utils
    static std::string GetFileContent(std::string);

    static std::string LoadPluginFile(std::string, std::string);

#ifndef IS_INTERNAL_RUNTIME
#else
    std::vector<Plugin *> plugins;

    void InitV8();

    void LoadMeta();

    void LoadPlugins();

    std::unique_ptr<v8::Platform> platform;
    v8::Isolate::CreateParams create_params;
    v8::Isolate *isolate;

    EXPORT v8::Isolate *GetIsolate();

    bool FireGameEvent(IGameEvent *event);

    void FireEvent(std::string, std::string);

    static v8::MaybeLocal<v8::Module> ResolveModule(v8::Local<v8::Context> context,
                                                    v8::Local<v8::String> specifier,
                                                    v8::Local<v8::FixedArray> arr,
                                                    v8::Local<v8::Module> referrer);

#endif

    static bool HasSuffix(std::string string, std::string suffix);

    void Shutdown();
};


#endif //RESOURCEMOD_ENGINE_H
