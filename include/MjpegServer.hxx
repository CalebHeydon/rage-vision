/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#pragma once

#include <unordered_map>
#include <vector>
#include <opencv2/opencv.hpp>
#include <mutex>
#include <atomic>
#include <thread>

class MjpegServer
{
private:
    int mPort;
    int mServerFd;
    std::map<int, std::vector<cv::Mat>> mQueues;
    std::mutex mQueuesMutex;
    std::atomic<bool> mRunning;
    std::map<int, std::vector<int>> mClients;
    std::mutex mClientsMutex;

public:
    MjpegServer(int port);

    void run();
    void stop();
};
