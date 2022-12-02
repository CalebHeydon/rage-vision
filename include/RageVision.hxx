/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <apriltag/apriltag.h>
#include <mutex>
#include <opencv2/opencv.hpp>

#include "MjpegServer.hxx"
#include "Camera.hxx"
#include "TimeServer.hxx"

class RageVision
{
private:
    std::string mIp;
    std::shared_ptr<MjpegServer> mMjpegServer;
    std::vector<std::shared_ptr<Camera>> mCameras;
    apriltag_detector_t *mTagDetector;
    apriltag_family_t *mTagFamily;
    long mSyncTime;
    std::mutex mSyncTimeMutex;
    std::shared_ptr<TimeServer> mTimeServer;

    bool runPipeline(cv::Mat *frame, std::shared_ptr<Camera> camera, std::vector<double> *timestamps);

public:
    RageVision(std::string ip, int mjpegPort, int syncPort, int dataPort, std::vector<int> cameras);
    ~RageVision();

    int run();
};
