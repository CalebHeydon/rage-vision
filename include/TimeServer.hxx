/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#pragma once

#include <mutex>
#include <thread>
#include <atomic>
#include <memory>

class TimeServer
{
private:
    int mPort;
    long *mTime;
    std::mutex *mMutex;
    int mServerFd;
    std::atomic<bool> mRunning;

public:
    TimeServer(int port, long *time, std::mutex *mutex);

    void run();
    void stop();
};
