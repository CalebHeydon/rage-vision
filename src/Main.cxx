/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>

#include "Constants.hxx"
#include "Calibration.hxx"
#include "RageVision.hxx"

int main(int argc, char **argv)
{
    std::cout << "RageVision " << Constants::kVersion << "\n";
    std::cout << "Copyright (c) 2022, Caleb Heydon\n";
    std::cout << "All rights reserved.\n\n";

    if (argc < 1)
    {
        std::cerr << "usage: RageVision destIp [mjpegPort] [syncPort] [camera...]\n";
        std::cerr << "       Calibrate width height [mjpegPort] [camera]\n";
        return -1;
    }

    std::string name{argv[0]};

    if (name.find("Calibrate") != -1)
    {
        int width, height;
        int mjpegPort = Constants::kDefaultMjpegPort;
        int camera = 0;

        if (argc < 3)
        {
            std::cerr << "usage: " << argv[0] << " width height [mjpegPort] [camera]\n";
            return -1;
        }

        width = atoi(argv[1]);
        height = atoi(argv[2]);

        if (argc > 3)
            mjpegPort = atoi(argv[3]);

        if (argc > 4)
            camera = atoi(argv[4]);

        Calibration calibration{camera, width, height, mjpegPort};
        return calibration.run();
    }

    if (argc < 2)
    {
        std::cerr << "usage: " << argv[0] << " destIp [mjpegPort] [syncPort] [dataPort] [camera...]\n";
        return -1;
    }

    std::string ip{argv[1]};
    int mjpegPort = Constants::kDefaultMjpegPort;
    int syncPort = Constants::kDefaultTimePort;
    int dataPort = Constants::kDefaultDataPort;
    std::vector<int> cameras;

    if (argc > 2)
        mjpegPort = atoi(argv[2]);

    if (argc > 3)
        syncPort = atoi(argv[3]);

    if (argc > 4)
        dataPort = atoi(argv[4]);

    for (int i = 5; i < argc; i++)
    {
        int camera = atoi(argv[i]);
        bool exists = false;

        for (int j = 0; j < cameras.size(); j++)
            if (cameras[j] == camera)
            {
                exists = true;
                break;
            }

        if (exists)
            continue;

        cameras.push_back(camera);
    }

    if (cameras.size() == 0)
        cameras.push_back(0);

    RageVision rageVision{ip, mjpegPort, syncPort, dataPort, cameras};
    return rageVision.run();
}
