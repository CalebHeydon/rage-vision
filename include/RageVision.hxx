/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#pragma once

#include <string>
#include <vector>

class RageVision
{
public:
    static const std::string kVersion;
    static const int kHttpBufferSize = 4096;
    static const int kDefaultMjpegPort = 5800;
    static const int kMjpegConnectionBacklog = 128;
    static const int kDefaultSyncPort = 5801;

    RageVision(std::string ip, int mjpegPort, std::vector<int> cameras);

    int run();
};
