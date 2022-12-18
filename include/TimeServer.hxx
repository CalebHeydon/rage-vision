/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#pragma once

#include <cstdint>
#include <mutex>
#include <thread>
#include <atomic>

class TimeServer
{
private:
    int mPort;
    uint64_t *mTime;
    std::mutex *mMutex;
    int mServerFd;
    std::atomic<bool> mRunning;

public:
    TimeServer(int port, uint64_t *time, std::mutex *mutex);

    void run();
    void stop();
};
