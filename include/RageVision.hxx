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

    RageVision(std::string ip, int mjpegPort, std::vector<int> cameras);

    int run();
};
