/*
Copyright (c) 2023, Caleb Heydon
All rights reserved.
*/

#pragma once

#include <string>

class UdpSender
{
private:
    int mPort;
    int mSocket;

public:
    UdpSender(int port);
    ~UdpSender();

    void send(std::string address, int camera, double timestamp, int id, double tx, double ty, double tz, double qw, double qx, double qy, double qz, double processingLatency);
};
