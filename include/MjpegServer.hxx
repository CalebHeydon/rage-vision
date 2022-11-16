/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#pragma once

#include <unordered_map>
#include <vector>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <atomic>
#include <thread>

class MjpegServer
{
private:
    int mPort;
    int mServerFd;
    std::atomic<bool> mRunning;
    std::vector<int> mCameras;
    std::mutex mCamerasMutex;
    std::map<int, std::vector<cv::Mat>> mQueues;
    std::mutex mQueuesMutex;
    std::map<int, std::vector<int>> mClients;
    std::mutex mClientsMutex;

public:
    MjpegServer(int port);

    void run();
    void stop();
    void sendFrame(int camera, cv::Mat frame);
};
