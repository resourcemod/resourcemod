//
// Created by Twelvee on 10.11.2023.
//

#ifndef RESOURCEMOD_OBJECT_H
#define RESOURCEMOD_OBJECT_H

#include "v8-local-handle.h"

class V8Object {
public:
    virtual v8::Local<v8::Object> Wrap() = 0;
};

#endif //RESOURCEMOD_OBJECT_H
