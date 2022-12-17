/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#pragma once

#include <opencv2/opencv.hpp>

#include "Constants.hxx"

class Camera
{
private:
    cv::VideoCapture mVideoCapture;
    int mId;
    bool mCalibrated;
    double mFx;
    double mFy;
    double mCx;
    double mCy;

public:
    typedef struct CameraInfo
    {
        int mId, mWidth, mHeight;
        double mFps;

        CameraInfo()
        {
            mId = 0;
            mWidth = Constants::kDefaultFrameWidth;
            mHeight = Constants::kDefaultFrameHeight;
            mFps = -1.0;
        }

        CameraInfo(int id, int width, int height, double fps)
        {
            mId = id;
            mWidth = width;
            mHeight = height;
            mFps = fps;
        }
    } CameraInfo;

    Camera(CameraInfo cameraInfo);

    int id();
    double currentFrame(cv::Mat *frame, long startTime = 0);
    void release();
    bool calibrated();
    double fx();
    double fy();
    double cx();
    double cy();
};
