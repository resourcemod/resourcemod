//
// Created by Twelvee on 13.11.2023.
//

#ifndef RESOURCEMOD_MODULE_H
#define RESOURCEMOD_MODULE_H

#include <libplatform/libplatform.h>
#include <v8.h>

class Module {
public:
    static std::string ReadFile(std::string);
    static v8::MaybeLocal<v8::Module> LoadModule(std::string code,
                                          v8::Local<v8::Context> cx);
    static v8::Local<v8::Module> CheckModule(v8::MaybeLocal<v8::Module> maybeModule,
                                      v8::Local<v8::Context> cx);
    static v8::Local<v8::Value> ExecModule(v8::Local<v8::Module> mod,
                                    v8::Local<v8::Context> cx,
                                    bool nsObject = false);
    static v8::MaybeLocal<v8::Module> CallResolve(v8::Local<v8::Context> context,
                                           v8::Local<v8::String> specifier,
                                           v8::Local<v8::FixedArray> arr,
                                           v8::Local<v8::Module> referrer);
    static v8::MaybeLocal<v8::Promise> CallDynamic(v8::Local<v8::Context> context, v8::Local<v8::Data> host_defined_options,
                                                   v8::Local<v8::Value> resource_name, v8::Local<v8::String> specifier,
                                                   v8::Local<v8::FixedArray> import_assertions);
    static void CallMeta(v8::Local<v8::Context> context,
                  v8::Local<v8::Module> module,
                  v8::Local<v8::Object> meta);
};


#endif //RESOURCEMOD_MODULE_H
