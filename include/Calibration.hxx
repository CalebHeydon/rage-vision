/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#pragma once

class Calibration
{
public:
    Calibration(int camera, int mjpegPort);

    int run();
};
