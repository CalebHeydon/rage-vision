/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#pragma once

#include <opencv2/opencv.hpp>
#include <atomic>
#include <vector>
#include <memory>

#include "Camera.hxx"

class Calibration
{
private:
    Camera::CameraInfo mCamera;

public:
    Calibration(Camera::CameraInfo camera);

    int run();
};
