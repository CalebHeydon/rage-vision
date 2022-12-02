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
#include <chrono>

Camera::Camera(int id, int width, int height, double fps) : mVideoCapture{id}
{
    mId = id;
    mCalibrated = true;

    if (!mVideoCapture.isOpened())
    {
        std::cerr << "Camera " << mId << " not found\n";
        std::exit(-1);
    }

    mVideoCapture.set(cv::CAP_PROP_FRAME_WIDTH, width);
    mVideoCapture.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    if (fps > 0)
        mVideoCapture.set(cv::CAP_PROP_FPS, fps);

    std::stringstream filename;
    filename << "./camera" << mId << ".yml";
    cv::FileStorage file{filename.str(), cv::FileStorage::READ};
    if (!file.isOpened())
    {
        mCalibrated = false;
        return;
    }

    file["cameraMatrix"] >> mCameraMatrix;
    file["distCoeffs"] >> mDistCoeffs;

    file.release();
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

bool Camera::calibrated()
{
    return mCalibrated;
}

double Camera::fx()
{
    return mCameraMatrix.at<double>(cv::Point{0, 0});
}

double Camera::fy()
{
    return mCameraMatrix.at<double>(cv::Point{1, 1});
}

double Camera::cx()
{
    return mCameraMatrix.at<double>(cv::Point{0, 2});
}

double Camera::cy()
{
    return mCameraMatrix.at<double>(cv::Point{1, 2});
}

const cv::Mat Camera::cameraMatrix()
{
    return mCameraMatrix;
}

const cv::Mat Camera::distCoeffs()
{
    return mDistCoeffs;
}
