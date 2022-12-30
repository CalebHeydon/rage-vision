/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#include "UdpSender.hxx"

#include <sys/socket.h>
#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <cstring>
#include <arpa/inet.h>

UdpSender::UdpSender(int port)
{
    mPort = port;

    mSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (mSocket == -1)
    {
        std::cerr << "Unable to open udp socket\n";
        std::exit(-1);
    }

    if (int flags = fcntl(mSocket, F_GETFL, 0) == -1 || fcntl(mSocket, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        std::cerr << "Unable to make udp socket non-blocking\n";
        std::exit(-1);
    }
}

UdpSender::~UdpSender()
{
    close(mSocket);
}

void UdpSender::send(std::string address, double timestamp, int id, double tx, double ty, double tz, double qw, double qx, double qy, double qz, double processingLatency)
{
    std::stringstream stringStream;
    stringStream.precision(17);
    stringStream << timestamp << ":";
    stringStream << id << ":";
    stringStream << tx << ":" << ty << ":" << tz << ":";
    stringStream << qw << ":" << qx << ":" << qy << ":" << qz << ":";
    stringStream << processingLatency;
    std::string data = stringStream.str();

    struct sockaddr_in addressInfo;
    memset(&addressInfo, 0, sizeof(addressInfo));

    addressInfo.sin_family = AF_INET;
    addressInfo.sin_port = htons(mPort);
    addressInfo.sin_addr.s_addr = inet_addr(address.c_str());

    if (sendto(mSocket, data.c_str(), data.size() + 1, 0, (struct sockaddr *)&addressInfo, sizeof(addressInfo)) == -1)
        std::cerr << "Unable to send udp packet\n";
}
