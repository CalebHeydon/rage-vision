/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>

#include "RageVision.hxx"

int main(int argc, char **argv)
{
    std::cout << "RageVision v" << RageVision::kVersion << "\n";
    std::cout << "Copyright (c) 2022, Caleb Heydon\n";
    std::cout << "All rights reserved.\n\n";

    if (argc < 1)
    {
        std::cerr << "usage: RageVision destIp [mjpegPort] [cameraId...]\n";
        return -1;
    }
    else if (argc < 2)
    {
        std::cerr << "usage: " << argv[0] << " destIp [mjpegPort] [cameraId...]\n";
        return -1;
    }

    std::string ip{argv[1]};
    int mjpegPort = 5800;
    std::vector<int> cameras;

    if (argc > 2)
        mjpegPort = atoi(argv[2]);

    for (int i = 3; i < argc; i++)
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
