/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#include "Calibration.hxx"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>

#include "Camera.hxx"

Calibration::Calibration(Camera::CameraInfo camera)
{
    mCamera = camera;
}

int Calibration::run()
{
    double fx, fy, cx, cy;

    std::cout << "fx: ";
    std::cin >> fx;

    std::cout << "fy: ";
    std::cin >> fy;

    std::cout << "cx: ";
    std::cin >> cx;

    std::cout << "cy: ";
    std::cin >> cy;

    std::stringstream filename;
    filename << "./camera" << mCamera.mId << "_" << mCamera.mWidth << "x" << mCamera.mHeight << ".yml";
    cv::FileStorage file{filename.str(), cv::FileStorage::WRITE};
    file << "fx" << fx;
    file << "fy" << fy;
    file << "cx" << cx;
    file << "cy" << cy;
    file.release();

    return 0;
}
