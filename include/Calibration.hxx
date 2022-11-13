/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#pragma once

#include <opencv2/opencv.hpp>
#include <atomic>
#include <vector>
#include <memory>

#include "Camera.hxx"
#include "MjpegServer.hxx"

class Calibration
{
private:
    Camera mCamera;
    cv::Size mCheckerboardSize;
    std::atomic<bool> mRunning;
    cv::Mat mFrame;
    std::atomic<bool> mFrameLock;
    cv::Size mSize;
    std::shared_ptr<MjpegServer> mMjpegServer;

public:
    Calibration(int camera, int width, int height, int mjpegPort);

    int run();
};
