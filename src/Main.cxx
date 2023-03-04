/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <cstddef>

#include "Constants.hxx"
#include "Calibration.hxx"
#include "Camera.hxx"
#include "RageVision.hxx"

int main(int argc, char **argv)
{
    std::cout << "RageVision " << Constants::kVersion << "\n";
    std::cout << "Copyright (c) 2023, Caleb Heydon\n";
    std::cout << "All rights reserved.\n\n";

    if (argc < 1)
    {
        std::cerr << "usage: RageVision destIp [mjpegPort] [syncPort] [camera:width:height:fps...]\n";
        std::cerr << "       Calibrate camera:width:height\n";
        return -1;
    }

    std::string name{argv[0]};

    if (name.find("Calibrate") != -1)
    {
        int camera = 0, frameWidth = Constants::kDefaultFrameWidth, frameHeight = Constants::kDefaultFrameHeight;
        double fps = -1.0;

        if (argc < 2)
        {
            std::cerr << "usage: " << argv[0] << " camera:width:height\n";
            return -1;
        }

        if (argc > 1)
        {
            std::string cameraString{argv[1]};
            size_t i = cameraString.find(":");
            if (i != -1)
            {
                std::string cameraIndexString = cameraString.substr(0, i);
                camera = atoi(cameraIndexString.c_str());

                cameraString = cameraString.substr(i + 1);
                i = cameraString.find(":");
                if (i != -1)
                {
                    std::string widthString = cameraString.substr(0, i + 1);
                    frameWidth = atoi(widthString.c_str());

                    cameraString = cameraString.substr(i + 1);
                    i = cameraString.find(":");
                    if (i != -1)
                    {
                        std::string heightString = cameraString.substr(0, i + 1);
                        frameHeight = atoi(heightString.c_str());
                    }
                    else
                    {
                        std::string heightString = cameraString;
                        frameHeight = atoi(heightString.c_str());
                    }
                }
            }
        }

        Calibration calibration{Camera::CameraInfo{camera, frameWidth, frameHeight, -1.0}};
        return calibration.run();
    }

    if (argc < 2)
    {
        std::cerr << "usage: " << argv[0] << " destIp [mjpegPort] [syncPort] [dataPort] [camera:width:height:fps...]\n";
        return -1;
    }

    std::string ip{argv[1]};
    int mjpegPort = Constants::kDefaultMjpegPort;
    int syncPort = Constants::kDefaultTimePort;
    int dataPort = Constants::kDefaultDataPort;
    std::vector<Camera::CameraInfo> cameras;

    if (argc > 2)
        mjpegPort = atoi(argv[2]);

    if (argc > 3)
        syncPort = atoi(argv[3]);

    if (argc > 4)
        dataPort = atoi(argv[4]);

    for (int i = 5; i < argc; i++)
    {
        int camera = 0, frameWidth = Constants::kDefaultFrameWidth, frameHeight = Constants::kDefaultFrameHeight;
        double fps = -1.0;
        std::string cameraString{argv[i]};
        size_t j = cameraString.find(":");
        if (j != -1)
        {
            std::string cameraIndexString = cameraString.substr(0, j);
            camera = atoi(cameraIndexString.c_str());

            cameraString = cameraString.substr(j + 1);
            j = cameraString.find(":");
            if (j != -1)
            {
                std::string widthString = cameraString.substr(0, j + 1);
                frameWidth = atoi(widthString.c_str());

                cameraString = cameraString.substr(j + 1);
                j = cameraString.find(":");
                if (j != -1)
                {
                    std::string heightString = cameraString.substr(0, j + 1);
                    frameHeight = atoi(heightString.c_str());

                    std::string fpsString = cameraString.substr(j + 1);
                    fps = atof(fpsString.c_str());
                }
                else
                {
                    std::string heightString = cameraString;
                    frameHeight = atoi(heightString.c_str());
                }
            }
        }
        bool exists = false;

        for (int k = 0; k < cameras.size(); k++)
            if (cameras[k].mId == camera)
            {
                exists = true;
                break;
            }

        if (exists)
            continue;

        cameras.push_back(Camera::CameraInfo{camera, frameWidth, frameHeight, fps});
    }

    if (cameras.size() == 0)
        cameras.push_back(Camera::CameraInfo{});

    RageVision rageVision{ip, mjpegPort, syncPort, dataPort, cameras};
    return rageVision.run();
}
