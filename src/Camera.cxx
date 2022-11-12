/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#include "Camera.hxx"

#include <iostream>
#include <opencv2/opencv.hpp>

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

void Camera::currentFrame(cv::Mat *frame)
{
    while (true)
    {
        if (!mVideoCapture.grab())
        {
            std::cerr << "Unable to get frame from camera " << mId << "\n";
            continue;
        }

        mVideoCapture.retrieve(*frame);
        break;
    }
}

void Camera::release()
{
    mVideoCapture.release();
}
