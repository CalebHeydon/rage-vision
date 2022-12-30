/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <apriltag/apriltag.h>
#include <cstdint>
#include <mutex>
#include <opencv2/opencv.hpp>

#include "MjpegServer.hxx"
#include "TimeServer.hxx"
#include "Camera.hxx"
#include "UdpSender.hxx"

class RageVision
{
private:
    std::string mIp;
    std::shared_ptr<MjpegServer> mMjpegServer;
    std::vector<std::shared_ptr<Camera>> mCameras;
    apriltag_detector_t *mTagDetector;
    apriltag_family_t *mTagFamily;
    uint64_t mSyncTime;
    std::mutex mSyncTimeMutex;
    std::shared_ptr<TimeServer> mTimeServer;
    std::shared_ptr<UdpSender> mUdpSender;

    bool runPipeline(cv::Mat *frame, std::shared_ptr<Camera> camera, std::vector<double> *timestamps);

public:
    RageVision(std::string ip, int mjpegPort, int syncPort, int dataPort, std::vector<Camera::CameraInfo> cameras);
    ~RageVision();

    int run();
};
