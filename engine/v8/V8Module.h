//
// Created by Twelvee on 13.11.2023.
//

#include <libplatform/libplatform.h>
#include <v8.h>

class V8Module {
public:
    static std::string ReadFile(std::string);
    static v8::MaybeLocal<v8::Module> LoadModule(std::string code,
                                                 std::string file_name,
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

    template <class T>
    static v8::MaybeLocal<T> CompileString(v8::Isolate* isolate, v8::Local<v8::Context> context,
                                            v8::Local<v8::String> source,
                                            const v8::ScriptOrigin& origin);

    static v8::MaybeLocal<v8::Module> Compile(v8::Local<v8::Context> context,
                                              v8::ScriptCompiler::Source* source,
                                              v8::ScriptCompiler::CompileOptions options) {
        return v8::ScriptCompiler::CompileModule(context->GetIsolate(), source, options);
    }
};

