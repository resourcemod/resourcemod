//
// Created by Twelvee on 08.10.2023.
//

#include "ExternalRuntime.h"
#include "../logger/logger.h"
#include <chrono>
#include <thread>
#include <mutex>
#include <zeromq/zmq.hpp>
#include <Windows.h>

#if defined _WIN32
#define EXPORT extern "C" __declspec(dllexport)
#elif defined __GNUC__
#define EXPORT extern "C" __attribute__ ((visibility("default")))
#endif

Engine *g_RMEngine;

//
// TODO: Ping request before actual handle the messages.
// _ Two socket connections has to be ready to handle messages, no matter what external runtime is running
// _ The main goal is to be able to handle tcp messages
//

void ExternalRuntime::RunExternalRuntime(bool console, std::string path) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    // Run zeromq listener
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    std::string m = path;
    m = m.append("/bin/node.exe external_runtime.js");
    LPSTR s = const_cast<char *>(m.c_str());
    CreateProcessA(
            nullptr,
            s,
            nullptr,                   // Process handle not inheritable
            nullptr,                   // Thread handle not inheritable
            FALSE,                  // Set handle inheritance to FALSE
            console ? CREATE_NEW_CONSOLE : NULL,     // Opens file in a separate console
            nullptr,           // Use parent's environment block
            path.c_str(),           // Use parent's starting directory
            &si,            // Pointer to STARTUPINFO structure
            &pi           // Pointer to PROCESS_INFORMATION structure
    );

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void ExternalRuntime::ZMQThreadPush() {
    zmq::context_t ctx(1);
    zmq::socket_t engineSocket(ctx, zmq::socket_type::push);

    engineSocket.bind("tcp://"+g_RMEngine->pullAddr);
    std::this_thread::sleep_for(std::chrono::microseconds(500)); // wait to establish connection

    for (;;) {
        if (!g_RMEngine->rtEvents.empty()) {
            std::string message = g_RMEngine->rtEvents.pop();
            engineSocket.send(zmq::buffer(message), zmq::send_flags::none);
        }
    }
    logger::log("Closing socket.!");
    engineSocket.close();
}

void ExternalRuntime::ZMQThreadPull() {
    zmq::context_t ctx(1);
    zmq::socket_t eventsSocket(ctx, zmq::socket_type::pull);

    eventsSocket.connect("tcp://"+g_RMEngine->pushAddr);
    std::this_thread::sleep_for(std::chrono::microseconds(500)); // wait to establish connection
    for (;;) {
        zmq::message_t message;
        // wait for new message to push
        eventsSocket.recv(&message);
        g_RMEngine->rtCallbacks.push(message.to_string().c_str());
    }
    logger::log("Closing socket.!");
    eventsSocket.close();
}

EXPORT void *Run(const char *path, bool console, Engine *engine) {
    g_RMEngine = new Engine();
    g_RMEngine = engine;

    // external runtime thread
    std::thread rt(ExternalRuntime::RunExternalRuntime, console, path);
    rt.detach();

    std::thread push(ExternalRuntime::ZMQThreadPush);
    push.detach();

    std::thread pull(ExternalRuntime::ZMQThreadPull);
    pull.detach();

    g_RMEngine->isExternalRuntimeRunning = true;

    return nullptr;
}