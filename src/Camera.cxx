/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#include "Camera.hxx"

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <string>
#include <cstdint>
#include <chrono>

Camera::Camera(CameraInfo cameraInfo) : mVideoCapture{cameraInfo.mId}
{
    mId = cameraInfo.mId;
    mCalibrated = true;

    if (!mVideoCapture.isOpened())
    {
        std::cerr << "Camera " << mId << " not found\n";
        exit(-1);
    }

    mVideoCapture.set(cv::CAP_PROP_FRAME_WIDTH, cameraInfo.mWidth);
    mVideoCapture.set(cv::CAP_PROP_FRAME_HEIGHT, cameraInfo.mHeight);
    if (cameraInfo.mFps > 0)
        mVideoCapture.set(cv::CAP_PROP_FPS, cameraInfo.mFps);

    std::stringstream filename;
    filename << "./camera" << mId << "_" << cameraInfo.mWidth << "x" << cameraInfo.mHeight << ".yml";
    cv::FileStorage file{filename.str(), cv::FileStorage::READ};
    if (!file.isOpened())
    {
        mCalibrated = false;
        return;
    }

    file["fx"] >> mFx;
    file["fy"] >> mFy;
    file["cx"] >> mCx;
    file["cy"] >> mCy;

    file.release();
}

int Camera::id()
{
    return mId;
}

double Camera::currentFrame(cv::Mat *frame, uint64_t startTime)
{
    uint64_t time;
    while (true)
    {
        if (!mVideoCapture.grab())
        {
            std::cerr << "Unable to get frame from camera " << mId << "\n";
            continue;
        }

        time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        mVideoCapture.retrieve(*frame);
        break;
    }

    return (time - startTime) / 1.0e9;
}

void Camera::release()
{
    mVideoCapture.release();
}

bool Camera::calibrated()
{
    return mCalibrated;
}

double Camera::fx()
{
    return mFx;
}

double Camera::fy()
{
    return mFy;
}

double Camera::cx()
{
    return mCx;
}

double Camera::cy()
{
    return mCy;
}
