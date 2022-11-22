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
#include <thread>

#include "Camera.hxx"
#include "MjpegServer.hxx"

const std::string RageVision::kVersion = "v0.1.0";

bool RageVision::runPipeline(cv::Mat *frame, std::shared_ptr<Camera> camera)
{
    camera->currentFrame(frame);

    if (camera->calibrated())
    {
        cv::cvtColor(*frame, *frame, cv::COLOR_BGR2GRAY);

        cv::Mat undistorted;
        cv::undistort(*frame, undistorted, camera->cameraMatrix(), camera->distCoeffs());
        undistorted.copyTo(*frame);

        image_u8_t image = {.width = frame->cols, .height = frame->rows, .stride = frame->cols, .buf = frame->data};
        zarray_t *tags = apriltag_detector_detect(mTagDetector, &image);

        for (int i = 0; i < tags->size; i++)
        {
            apriltag_detection_t *tag;
            zarray_get(tags, i, &tag);

            double error = -1;

            if (tag->hamming > kMaxHamming)
                goto PIPELINE_NEXT_TAG;

            apriltag_detection_info_t info;
            info.det = tag;
            info.tagsize = kTagSize;
            info.fx = camera->fx();
            info.fy = camera->fy();
            info.cx = camera->cx();
            info.cy = camera->cy();

            apriltag_pose_t pose;
            error = estimate_tag_pose(&info, &pose);
            std::cout << "id: " << tag->id << ", hamming: " << tag->hamming << ", error: " << error << "\n";

            free(pose.t);
            free(pose.R);

        PIPELINE_NEXT_TAG:
            apriltag_detection_destroy(tag);
        }

        zarray_destroy(tags);
    }

    mMjpegServer->sendFrame(camera->id(), *frame);

    return true;
}

RageVision::RageVision(std::string ip, int mjpegPort, std::vector<int> cameras)
{
    mIp = ip;
    mMjpegServer = std::make_shared<MjpegServer>(mjpegPort);

    for (int camera : cameras)
        mCameras.push_back(std::make_shared<Camera>(camera));

    mTagDetector = apriltag_detector_create();
    mTagDetector->nthreads = kTagThreads;
    mTagFamily = tag16h5_create();
    apriltag_detector_add_family(mTagDetector, mTagFamily);
}

int RageVision::run()
{
    mMjpegServer->run();

    for (int i = 1; i < mCameras.size(); i++)
    {
        std::shared_ptr<Camera> camera = mCameras[i];

        std::thread thread{[this, camera]
                           {
                               cv::Mat frame;

                               while (runPipeline(&frame, camera))
                                   ;
                           }};
        thread.detach();
    }

    std::shared_ptr<Camera> camera = mCameras[0];
    cv::Mat frame;

    while (runPipeline(&frame, camera))
        ;

    return 0;
}

RageVision::~RageVision()
{
    tag16h5_destroy(mTagFamily);
    apriltag_detector_destroy(mTagDetector);
}
