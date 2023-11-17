//
// Created by Twelvee on 15.09.2023.
//

#include "Engine.h"
#include "../logger/logger.h"

#include "EventLoop.h"

extern Engine *g_Engine;
extern EventLoop *g_EventLoop;

void EventLoop::TaskThread() {
    while(g_EventLoop->isRunning) {
        if (g_EventLoop->tasks.empty()) {
            Sleep(100);
            continue;
        }
        Task* task = g_EventLoop->tasks.pop();
        if (task->GetType() == TaskType::TIMER) {
            if (task->GetTimeDiff() >= task->GetTimerMs()) {
                task->GetPlugin()->AsyncCallback(task->callback.Pass());
                delete task;
            } else {
                g_EventLoop->tasks.push(task);
            }
        }
        if (task->GetType() == TaskType::INTERVAL) {
            if (task->GetTimeDiff() >= task->GetTimerMs()) {
                v8::HandleScope handle_scope(g_Engine->isolate);
                Task* newTask = new Task(task->GetPlugin(), task->GetType(), task->GetMethod(), task->callback.Get(g_Engine->isolate), task->GetTimerMs());
                task->GetPlugin()->AsyncCallback(task->callback.Pass());
                delete task;
                g_EventLoop->tasks.push(newTask);
            } else {
                g_EventLoop->tasks.push(task);
            }
        }
        if (task->GetType() == TaskType::ASYNC) {
            // call method
            // call callback
        }
    }
}

void EventLoop::Run() {
    this->isRunning = true;
    std::thread t(EventLoop::TaskThread);
    t.detach();
}