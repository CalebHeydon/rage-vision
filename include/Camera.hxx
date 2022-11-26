/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#pragma once

#include <opencv2/opencv.hpp>

class Camera
{
private:
    cv::VideoCapture mVideoCapture;
    int mId;
    bool mCalibrated;
    cv::Mat mCameraMatrix;
    cv::Mat mDistCoeffs;

public:
    Camera(int id);

    int id();
    double currentFrame(cv::Mat *frame, long startTime = 0);
    void release();
    bool calibrated();
    double fx();
    double fy();
    double cx();
    double cy();
    const cv::Mat cameraMatrix();
    const cv::Mat distCoeffs();
};
