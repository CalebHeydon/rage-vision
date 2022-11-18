/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#include "RageVision.hxx"

#include <memory>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <thread>

#include "Camera.hxx"
#include "MjpegServer.hxx"

const std::string RageVision::kVersion = "v0.1.0";

bool RageVision::runPipeline(cv::Mat *frame, std::shared_ptr<Camera> camera)
{
    camera->currentFrame(frame);

    if (camera->calibrated())
    {
        cv::Mat undistorted;
        cv::undistort(*frame, undistorted, camera->cameraMatrix(), camera->distCoeffs());
        undistorted.copyTo(*frame);
    }

    mMjpegServer->sendFrame(camera->id(), *frame);

    return true;
}

RageVision::RageVision(std::string ip, int mjpegPort, std::vector<int> cameras)
{
    mIp = ip;
    mMjpegServer = std::make_shared<MjpegServer>(mjpegPort);

    for (int camera : cameras)
        mCameras.push_back(std::make_shared<Camera>(camera));
}

int RageVision::run()
{
    mMjpegServer->run();

    for (int i = 1; i < mCameras.size(); i++)
    {
        std::shared_ptr<Camera> camera = mCameras[i];

        std::thread thread{[this, camera]
                           {
                               cv::Mat frame;

                               while (runPipeline(&frame, camera))
                                   ;
                           }};
        thread.detach();
    }

    std::shared_ptr<Camera> camera = mCameras[0];
    cv::Mat frame;

    while (runPipeline(&frame, camera))
        ;

    return 0;
}
