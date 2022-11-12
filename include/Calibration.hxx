/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#pragma once

#include <opencv2/opencv.hpp>
#include <atomic>
#include <vector>

#include "Camera.hxx"

class Calibration
{
private:
    Camera mCamera;
    cv::Size mCheckerboardSize;
    int mMjpegPort;
    std::atomic<bool> mRunning;
    cv::Mat mFrame;
    std::atomic<bool> mFrameLock;
    cv::Size mSize;

public:
    Calibration(int camera, int width, int height, int mjpegPort);

    int run();
};
