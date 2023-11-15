//
// Created by Twelvee on 10.11.2023.
//

#include <v8.h>

#ifndef RESOURCEMOD_V8_UTILS_H
#define RESOURCEMOD_V8_UTILS_H

static const char* v8str_to_cstr(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

#endif //RESOURCEMOD_V8_UTILS_H
