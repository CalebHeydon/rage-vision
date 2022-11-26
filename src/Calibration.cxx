/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#include "Calibration.hxx"

#include <opencv2/opencv.hpp>
#include <memory>
#include <thread>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <sstream>

#include "MjpegServer.hxx"

Calibration::Calibration(int camera, int width, int height, int mjpegPort) : mCamera{camera}
{
    mCheckerboardSize = cv::Size{width, height};
    mRunning = true;
    mFrameLock = false;
    mMjpegServer = std::make_shared<MjpegServer>(mjpegPort);
}

int Calibration::run()
{
    mMjpegServer->run();

    std::thread thread{[this]
                       {
                           while (mRunning)
                           {
                               while (mFrameLock.exchange(true, std::memory_order_acquire))
                                   ;
                               mCamera.currentFrame(&mFrame);
                               mFrameLock.store(false, std::memory_order_release);

                               mMjpegServer->sendFrame(mCamera.id(), mFrame);
                           }
                       }};

    // See https://learnopencv.com/camera-calibration-using-opencv/
    // and https://docs.opencv.org/4.x/dc/dbb/tutorial_py_calibration.html

    std::vector<std::vector<cv::Point3f>> objectPoints;
    std::vector<cv::Point3f> points;
    for (int i = 0; i < mCheckerboardSize.width; i++)
        for (int j = 0; j < mCheckerboardSize.height; j++)
            points.push_back(cv::Point3f{(float)i, (float)j, 0});

    std::vector<std::vector<cv::Point2f>> imagePoints;

    cv::Mat cameraMatrix, distCoeffs, rotations, translations;

    for (int i = 1; true; i++)
    {
        std::cout << "Press ENTER to capture frame " << i << " (enter any character first to end calibration)";
        if (std::cin.get() != '\n')
            break;

        while (mFrameLock.exchange(true, std::memory_order_acquire))
            ;
        cv::Mat frame = mFrame.clone();
        mFrameLock.store(false, std::memory_order_release);

        mSize.width = frame.cols;
        mSize.height = frame.rows;

        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

        std::vector<cv::Point2f> corners;
        bool result = cv::findChessboardCorners(frame, mCheckerboardSize, corners, 0);
        if (!result)
        {
            i--;
            std::cerr << "Fail\n";
            continue;
        }

        objectPoints.push_back(points);
        imagePoints.push_back(corners);

        double error = cv::calibrateCamera(objectPoints, imagePoints, mSize, cameraMatrix, distCoeffs, rotations, translations, cv::CALIB_FIX_ASPECT_RATIO | cv::CALIB_ZERO_TANGENT_DIST);

        std::cout << "Ok (error = " << error << ")\n";
    }
    mRunning = false;
    thread.join();
    mCamera.release();

    if (imagePoints.size() < 1)
    {
        std::cerr << "No calibration frames\n";
        return -1;
    }

    cv::Mat optimalCameraMatrix = cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, mSize, 1, mSize);

    std::stringstream filename;
    filename << "./camera" << mCamera.id() << ".yml";
    cv::FileStorage file{filename.str(), cv::FileStorage::WRITE};
    file << "cameraMatrix" << optimalCameraMatrix;
    file << "distCoeffs" << distCoeffs;
    file.release();

    return 0;
}
