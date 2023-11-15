//
// Created by Twelvee on 22.09.2023.
//

#ifndef RESOURCEMOD_PLUGIN_H
#define RESOURCEMOD_PLUGIN_H

#include <string>
#include <mutex>
#include <utility>
#include <map>
#include "igameevents.h"
#include "v8.h"

struct eventCallback{
    std::string name;
    v8::Global<v8::Function> cb;
};

class Plugin {
public:
    Plugin(std::string n, std::string version) {
        this->name = std::move(n);
        this->version = std::move(version);
    };
    ~Plugin() {
        this->isLoaded = false;
    };

    // meta
    std::string name;
    std::string version;
    bool isLoaded = false;

    // Subscribed event handlers (key is event name, value is event callback)
    std::map<std::string, eventCallback*> events;

    // v8 test
    void LoadPluginFS();
    void SubscribeOnEvent(std::string name, v8::Local<v8::Value> cb);
    bool FireEvent(std::string, std::string);
    bool FireGameEvent(IGameEvent* event);
    v8::Global<v8::Context> v8context;
};


#endif //RESOURCEMOD_PLUGIN_H
