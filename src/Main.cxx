/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>

#include "Calibration.hxx"
#include "RageVision.hxx"

int main(int argc, char **argv)
{
    std::cout << "RageVision v" << RageVision::kVersion << "\n";
    std::cout << "Copyright (c) 2022, Caleb Heydon\n";
    std::cout << "All rights reserved.\n\n";

    if (argc < 1)
    {
        std::cerr << "usage: RageVision destIp [mjpegPort] [syncPort] [cameraId...]\n";
        std::cerr << "       Calibrate [mjpegPort]\n";
        return -1;
    }

    std::string name{argv[0]};

    if (name.find("Calibrate") != -1)
    {
        int mjpegPort = 5800;

        if (argc > 1)
            mjpegPort = atoi(argv[1]);

        Calibration calibration{mjpegPort};
        return calibration.run();
    }

    if (argc < 2)
    {
        std::cerr << "usage: " << argv[0] << " destIp [mjpegPort] [syncPort] [cameraId...]\n";
        return -1;
    }

    std::string ip{argv[1]};
    int mjpegPort = 5800;
    int syncPort = 5801;
    std::vector<int> cameras;

    if (argc > 2)
        mjpegPort = atoi(argv[2]);

    if (argc > 3)
        syncPort = atoi(argv[3]);

    for (int i = 4; i < argc; i++)
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

    RageVision rageVision{ip, mjpegPort, cameras};
    return rageVision.run();
}
