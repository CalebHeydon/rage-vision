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

public:
    Camera(int id);

    int id();
    void currentFrame(cv::Mat *frame);
    void release();
};
