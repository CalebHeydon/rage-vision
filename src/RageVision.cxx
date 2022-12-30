/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#include "RageVision.hxx"

#include <memory>
#include <opencv2/opencv.hpp>
#include <apriltag/apriltag.h>
#include <apriltag/tag16h5.h>
#include <apriltag/apriltag_pose.h>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <thread>

#include "Camera.hxx"
#include "MjpegServer.hxx"
#include "TimeServer.hxx"
#include "Constants.hxx"

bool RageVision::runPipeline(cv::Mat *frame, std::shared_ptr<Camera> camera, double *lastTimestamp)
{
    mSyncTimeMutex.lock();
    double timestamp = camera->currentFrame(frame, mSyncTime);
    mSyncTimeMutex.unlock();
    double fps = 1.0 / (timestamp - *lastTimestamp);
    *lastTimestamp = timestamp;

    if (camera->calibrated())
    {
        double startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1.0e9;

        cv::Mat gray;
        cv::cvtColor(*frame, gray, cv::COLOR_BGR2GRAY);

        image_u8_t image = {.width = gray.cols, .height = gray.rows, .stride = gray.cols, .buf = gray.data};
        zarray_t *tags = apriltag_detector_detect(mTagDetector, &image);

        for (int i = 0; i < tags->size; i++)
        {
            apriltag_detection_t *tag;
            zarray_get(tags, i, &tag);

            double error = -1.0;

            if (tag->hamming > Constants::kMaxHamming)
                goto PIPELINE_NEXT_TAG;

            apriltag_detection_info_t info;
            info.det = tag;
            info.tagsize = Constants::kTagSize;
            info.fx = camera->fx();
            info.fy = camera->fy();
            info.cx = camera->cx();
            info.cy = camera->cy();

            apriltag_pose_t pose;
            error = estimate_tag_pose(&info, &pose);

            if (error <= Constants::kMaxError)
            {
                double endTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1.0e9;
                double latency = endTime - (timestamp + mSyncTime / 1.0e9);
                double processingLatency = endTime - startTime;

                double translation[3];
                translation[0] = MATD_EL(pose.t, 0, 0);
                translation[1] = MATD_EL(pose.t, 1, 0);
                translation[2] = MATD_EL(pose.t, 2, 0);

                double rotation[4];
                rotation[0] = sqrt(1.0 + MATD_EL(pose.R, 0, 0) + MATD_EL(pose.R, 1, 1) + MATD_EL(pose.R, 2, 2)) / 2.0;
                rotation[1] = (MATD_EL(pose.R, 2, 1) - MATD_EL(pose.R, 1, 2)) / (4.0 * rotation[0]);
                rotation[2] = (MATD_EL(pose.R, 0, 2) - MATD_EL(pose.R, 2, 0)) / (4.0 * rotation[0]);
                rotation[3] = (MATD_EL(pose.R, 1, 0) - MATD_EL(pose.R, 0, 1)) / (4.0 * rotation[0]);

                mUdpSender->send(mIp, timestamp, tag->id, translation[0], translation[1], translation[2], rotation[0], rotation[1], rotation[2], rotation[3], processingLatency);

#ifndef NDEBUG
                std::cout << "fps: " << fps << ", latency: " << latency << ", processingLatency: " << processingLatency << ", id: " << tag->id << ", (" << translation[0] << ", " << translation[1] << ", " << translation[2] << "), (" << rotation[0] << ", " << rotation[1] << ", " << rotation[2] << ", " << rotation[3] << ")\n";
#endif

                int thickness = std::max(frame->rows / 200, 1);
                cv::line(*frame, cv::Point{(int)tag->p[0][0], (int)tag->p[0][1]}, cv::Point{(int)tag->p[1][0], (int)tag->p[1][1]}, cv::Scalar{0, 255, 0}, thickness);
                cv::line(*frame, cv::Point{(int)tag->p[1][0], (int)tag->p[1][1]}, cv::Point{(int)tag->p[2][0], (int)tag->p[2][1]}, cv::Scalar{0, 255, 0}, thickness);
                cv::line(*frame, cv::Point{(int)tag->p[2][0], (int)tag->p[2][1]}, cv::Point{(int)tag->p[3][0], (int)tag->p[3][1]}, cv::Scalar{0, 255, 0}, thickness);
                cv::line(*frame, cv::Point{(int)tag->p[3][0], (int)tag->p[3][1]}, cv::Point{(int)tag->p[0][0], (int)tag->p[0][1]}, cv::Scalar{0, 255, 0}, thickness);

                int minX = frame->cols - 1, minY = frame->rows - 1, maxX = 0, maxY = 0;
                for (int i = 0; i < 4; i++)
                {
                    if (tag->p[i][0] < minX)
                        minX = tag->p[i][0];
                    if (tag->p[i][0] > maxX)
                        maxX = tag->p[i][0];
                    if (tag->p[i][1] < minY)
                        minY = tag->p[i][1];
                    if (tag->p[i][1] > maxY)
                        maxY = tag->p[i][1];
                }

                thickness = std::max(frame->rows / 150, 1);
                cv::rectangle(*frame, cv::Rect{cv::Point{minX, minY}, cv::Point{maxX, maxY}}, cv::Scalar{0, 255, 0}, thickness);

                double range = std::sqrt(pose.t->data[0] * pose.t->data[0] + pose.t->data[1] * pose.t->data[1] + pose.t->data[2] * pose.t->data[2]);

                thickness = 2;
                int height = std::max(frame->rows / 75, 10);
                double scale;
                for (scale = 0; cv::getTextSize("", cv::FONT_HERSHEY_PLAIN, scale, thickness, nullptr).height < height; scale += 0.1)
                    ;
                int textX = minX, textY = minY - height;

                std::stringstream rangeText;
                rangeText << range;
                cv::putText(*frame, rangeText.str(), cv::Point{textX, textY}, cv::FONT_HERSHEY_PLAIN, scale, cv::Scalar{0, 255, 0}, thickness);

                textY = maxY + height * 2;

                std::stringstream idText;
                idText << tag->id;
                cv::putText(*frame, idText.str(), cv::Point{textX, textY}, cv::FONT_HERSHEY_PLAIN, scale, cv::Scalar{0, 255, 0}, thickness);
            }

            free(pose.t);
            free(pose.R);

        PIPELINE_NEXT_TAG:
            apriltag_detection_destroy(tag);
        }

        zarray_destroy(tags);
    }

    int thickness = 2;
    int height = std::max(frame->rows / 75, 10);
    double scale;
    for (scale = 0; cv::getTextSize("", cv::FONT_HERSHEY_PLAIN, scale, thickness, nullptr).height < height; scale += 0.1)
        ;
    int textX = height, textY = frame->rows - height;

    std::stringstream fpsText;
    fpsText.precision(1);
    fpsText << fps;
    cv::putText(*frame, fpsText.str(), cv::Point{textX, textY}, cv::FONT_HERSHEY_PLAIN, scale, cv::Scalar{0, 255, 0}, thickness);

    mMjpegServer->sendFrame(camera->id(), *frame);

    return true;
}

RageVision::RageVision(std::string ip, int mjpegPort, int syncPort, int dataPort, std::vector<Camera::CameraInfo> cameras)
{
    mIp = ip;
    mMjpegServer = std::make_shared<MjpegServer>(mjpegPort);

    for (Camera::CameraInfo camera : cameras)
        mCameras.push_back(std::make_shared<Camera>(camera));

    mTagDetector = apriltag_detector_create();
    mTagDetector->nthreads = Constants::kTagThreads;
    mTagDetector->quad_decimate = Constants::kTagDecimate;
    mTagDetector->debug = 0;
    mTagFamily = tag16h5_create();
    apriltag_detector_add_family(mTagDetector, mTagFamily);

    mSyncTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    mTimeServer = std::make_shared<TimeServer>(syncPort, &mSyncTime, &mSyncTimeMutex);

    mUdpSender = std::make_shared<UdpSender>(dataPort);
}

RageVision::~RageVision()
{
    tag16h5_destroy(mTagFamily);
    apriltag_detector_destroy(mTagDetector);
}

int RageVision::run()
{
    mMjpegServer->run();
    mTimeServer->run();

    for (int i = 1; i < mCameras.size(); i++)
    {
        std::shared_ptr<Camera> camera = mCameras[i];

        std::thread thread{[this, camera]
                           {
                               cv::Mat frame;
                               double lastTimestamp = 0;

                               while (runPipeline(&frame, camera, &lastTimestamp))
                                   ;
                           }};
        thread.detach();
    }

    std::shared_ptr<Camera> camera = mCameras[0];
    cv::Mat frame;
    double lastTimestamp = 0;

    while (runPipeline(&frame, camera, &lastTimestamp))
        ;

    return 0;
}
