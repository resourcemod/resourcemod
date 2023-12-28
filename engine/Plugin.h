//
// Created by Twelvee on 22.09.2023.
//

#ifndef RESOURCEMOD_PLUGIN_H
#define RESOURCEMOD_PLUGIN_H

#define EXPORT __declspec(dllexport)

#include <string>
#include <mutex>
#include <utility>
#include <map>
#include "igameevents.h"
#include "v8.h"

struct eventCallback {
    std::string name;
    v8::Global<v8::Function> cb;
};

class Plugin {
public:
    Plugin(std::string n, std::string version, std::string folder, std::string website = "",
           std::string authors = "unknown") {
        this->name = std::move(n);
        this->version = std::move(version);
        this->folder = std::move(folder);
        this->website = std::move(website);
        this->authors = std::move(authors);
    };

    ~Plugin() {
        this->isLoaded = false;
    };

    // meta
    std::string name;
    std::string version;
    std::string website;
    std::string authors;

    std::string folder;

    bool isLoaded = false;

    // Subscribed event handlers (key is event name, value is event callback)
    std::map<std::string, eventCallback *> events;

    // v8 test
    void LoadPluginFS();

    void AsyncCallback(v8::Global<v8::Function> cb);

    void SubscribeOnEvent(std::string name, v8::Local<v8::Value> cb);

    bool FireEvent(std::string, std::string);

    bool FireGameEvent(IGameEvent *event);

    void *LoadExtension(std::string, v8::Isolate *);

    void LoadExtensions();

    EXPORT v8::Global<v8::Context> GetGlobalContext();

    v8::Global<v8::Context> v8context;
};


#endif //RESOURCEMOD_PLUGIN_H
