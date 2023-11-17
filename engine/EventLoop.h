//
// Created by Twelvee on 17.11.2023.
//

#ifndef RESOURCEMOD_EVENTLOOP_H
#define RESOURCEMOD_EVENTLOOP_H

#include "Plugin.h"
#include <chrono>

extern Engine *g_Engine;

enum TaskType {
    TIMER,
    INTERVAL,
    ASYNC
};

class Task {
    Plugin *plugin;
    TaskType type;
    void *method;
    std::chrono::time_point<std::chrono::system_clock> created;
    int timerMs;
public:
    Task(Plugin *plugin, TaskType type, void *method, v8::Local<v8::Function> callback, int ms = 0) {
        this->plugin = plugin;
        this->type = type;
        this->method = method;
        this->callback.Reset(g_Engine->isolate, callback);
        this->created = std::chrono::system_clock::now();
        this->timerMs = ms;
    };

    v8::Global<v8::Function> callback;

    Plugin *GetPlugin() {
        return this->plugin;
    }

    TaskType GetType() {
        return this->type;
    }

    void *GetMethod() {
        return this->method;
    }

    void SetFreshlyCreated() {
        this->created = std::chrono::system_clock::now();
    }

    int GetTimerMs() {
        return this->timerMs;
    }

    int GetTimeDiff() {
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - this->created);

        return milliseconds.count();
    }
};

class EventLoop {
    TSQueue<Task *> tasks;
    bool isRunning = false;

public:
    EventLoop() {

    };

    ~EventLoop() {
        while (!this->tasks.empty()) {
            this->tasks.pop();
        }
        this->isRunning = false;
    };

    void PushTask(Task *task) {
        this->tasks.push(task);
    }

    static void TaskThread();

    void Run(); // run event loop
};

#endif //RESOURCEMOD_EVENTLOOP_H
