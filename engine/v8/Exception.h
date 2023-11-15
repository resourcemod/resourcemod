//
// Created by Twelvee on 14.11.2023.
//

#ifndef RESOURCEMOD_EXCEPTION_H
#define RESOURCEMOD_EXCEPTION_H
#include <libplatform/libplatform.h>
#include <v8.h>
#include "v8_utils.h"

class v8Exception {
public:
    static void ReportException(v8::Isolate *isolate, v8::TryCatch *try_catch) {
        v8::HandleScope handle_scope(isolate);
        auto ext(try_catch->Exception());
        v8::String::Utf8Value exception(isolate, ext);
        const char *exception_string = v8str_to_cstr(exception);
        v8::Local <v8::Message> message = try_catch->Message();
        if (message.IsEmpty()) {
            fprintf(stderr, "%s\n", exception_string);
        } else {
            auto fn(message->GetScriptOrigin().ResourceName());
            v8::String::Utf8Value filename(isolate, fn);
            v8::Local <v8::Context> context(isolate->GetCurrentContext());
            const char *filename_string = v8str_to_cstr(filename);
            int linenum = message->GetLineNumber(context).FromJust();
            fprintf(stderr, "%s:%i: %s\n", filename_string, linenum, exception_string);

            auto sourceline(
                    message->GetSourceLine(context).ToLocalChecked());
            v8::String::Utf8Value line(isolate, sourceline);
            const char *sourceline_string = v8str_to_cstr(line);
            fprintf(stderr, "%s\n", sourceline_string);

            int start = message->GetStartColumn(context).FromJust();
            for (int i = 0; i < start; i++) {
                fprintf(stderr, " ");
            }
            int end = message->GetEndColumn(context).FromJust();
            for (int i = start; i < end; i++) {
                fprintf(stderr, "^");
            }
            fprintf(stderr, "\n");
            v8::Local <v8::Value> stack_trace_string;
            if (try_catch->StackTrace(context).ToLocal(&stack_trace_string) &&
                stack_trace_string->IsString() &&
                v8::Local<v8::String>::Cast(stack_trace_string)->Length() > 0) {
                auto stack_trace(stack_trace_string);
                v8::String::Utf8Value trace(isolate, stack_trace);
                const char *stack_trace_string = v8str_to_cstr(trace);
                fprintf(stderr, "%s\n", stack_trace_string);
            }
        }
    }
};

#endif //RESOURCEMOD_EXCEPTION_H
