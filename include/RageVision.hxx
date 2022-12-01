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
    static const std::string kVersion;
    static const int kHttpBufferSize = 4096;
    static const int kDefaultMjpegPort = 5800;
    static const int kMjpegConnectionBacklog = 128;
    static const int kDefaultTimePort = 5801;
    static const int kDefaultDataPort = 5800;
    static const int kMaxHamming = 0;
    static constexpr double kMaxError = 0.001;
    static constexpr double kTagSize = 0.1524;
    static const int kTagThreads = 4;
    static constexpr float kTagDecimate = 2.0f;
    static const int kTimeBufferSize = 2048;

    RageVision(std::string ip, int mjpegPort, int syncPort, int dataPort, std::vector<int> cameras);
    ~RageVision();

    int run();
};
