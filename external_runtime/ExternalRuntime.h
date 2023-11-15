//
// Created by Twelvee on 08.10.2023.
//

#ifndef EXTERNAL_RUNTIME_EXTERNALRUNTIME_H
#define EXTERNAL_RUNTIME_EXTERNALRUNTIME_H

#include <string>
#include "../engine/Engine.h"

class ExternalRuntime {
public:
    Engine* engine;
    static void RunExternalRuntime(bool console, std::string path);
    static void ZMQThreadPush();
    static void ZMQThreadPull();
};


#endif //EXTERNAL_RUNTIME_EXTERNALRUNTIME_H
