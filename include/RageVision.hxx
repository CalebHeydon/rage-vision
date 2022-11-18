/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <opencv2/opencv.hpp>

#include "MjpegServer.hxx"
#include "Camera.hxx"

class RageVision
{
private:
    std::string mIp;
    std::shared_ptr<MjpegServer> mMjpegServer;
    std::vector<std::shared_ptr<Camera>> mCameras;

    bool runPipeline(cv::Mat *frame, std::shared_ptr<Camera> camera);

public:
    static const std::string kVersion;
    static const int kHttpBufferSize = 4096;
    static const int kDefaultMjpegPort = 5800;
    static const int kMjpegConnectionBacklog = 128;
    static const int kDefaultSyncPort = 5801;

    RageVision(std::string ip, int mjpegPort, std::vector<int> cameras);

    int run();
};
