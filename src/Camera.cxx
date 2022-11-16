/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#include "Camera.hxx"

#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <chrono>

Camera::Camera(int id) : mVideoCapture{id}
{
    mId = id;

    if (!mVideoCapture.isOpened())
    {
        std::cerr << "Camera " << id << " not found\n";
        std::exit(-1);
    }
}

int Camera::id()
{
    return mId;
}

double Camera::currentFrame(cv::Mat *frame, long startTime)
{
    long time;
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
