/*
BSD 2-Clause License

Copyright (c) 2022, Caleb Heydon
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <functional>
#include <thread>
#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <cstdint>
#include <string>
#include <cstddef>
#include <cstdlib>
#include <unistd.h>
#include <sstream>

class RageVision
{
private:
    static const int kBufferSize = 1024;

public:
    RageVision(int port, std::function<void(double timestamp, int id, double tx, double ty, double tz, double qw, double qx, double qy, double qz, double processingLatency)> callback)
    {
        int bufferSize = kBufferSize;

        std::thread thread{[port, bufferSize, callback]
                           {
                               int fd = socket(AF_INET, SOCK_DGRAM, 0);
                               if (fd == -1)
                               {
                                   std::cerr << "Unable to open tcp socket\n";
                                   std::exit(-1);
                               }

                               struct sockaddr_in serverAddress;
                               memset(&serverAddress, 0, sizeof(serverAddress));

                               serverAddress.sin_family = AF_INET;
                               serverAddress.sin_port = htons(port);
                               serverAddress.sin_addr.s_addr = inet_addr("0.0.0.0");

                               if (bind(fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
                               {
                                   std::cerr << "Unable to bind to port\n";
                                   std::exit(-1);
                               }

                               uint8_t *buffer = new uint8_t[bufferSize];
                               if (!buffer)
                               {
                                   std::cerr << "Out of memory\n";
                                   std::exit(-1);
                               }

                               while (true)
                               {
                                   if (recvfrom(fd, buffer, bufferSize, 0, nullptr, nullptr) == -1)
                                       continue;

                                   std::string data{(const char *)buffer};

                                   size_t i = data.find(":");
                                   if (i == -1)
                                       continue;
                                   std::string number = data.substr(0, i);
                                   data = data.substr(i + 1, data.size() - i);
                                   double timestamp = atof(number.c_str());

                                   i = data.find(":");
                                   if (i == -1)
                                       continue;
                                   number = data.substr(0, i);
                                   data = data.substr(i + 1, data.size() - i);
                                   int id = atoi(number.c_str());

                                   i = data.find(":");
                                   if (i == -1)
                                       continue;
                                   number = data.substr(0, i);
                                   data = data.substr(i + 1, data.size() - i);
                                   double tx = atof(number.c_str());

                                   i = data.find(":");
                                   if (i == -1)
                                       continue;
                                   number = data.substr(0, i);
                                   data = data.substr(i + 1, data.size() - i);
                                   double ty = atof(number.c_str());

                                   i = data.find(":");
                                   if (i == -1)
                                       continue;
                                   number = data.substr(0, i);
                                   data = data.substr(i + 1, data.size() - i);
                                   double tz = atof(number.c_str());

                                   i = data.find(":");
                                   if (i == -1)
                                       continue;
                                   number = data.substr(0, i);
                                   data = data.substr(i + 1, data.size() - i);
                                   double qw = atof(number.c_str());

                                   i = data.find(":");
                                   if (i == -1)
                                       continue;
                                   number = data.substr(0, i);
                                   data = data.substr(i + 1, data.size() - i);
                                   double qx = atof(number.c_str());

                                   i = data.find(":");
                                   if (i == -1)
                                       continue;
                                   number = data.substr(0, i);
                                   data = data.substr(i + 1, data.size() - i);
                                   double qy = atof(number.c_str());

                                   i = data.find(":");
                                   if (i == -1)
                                       continue;
                                   number = data.substr(0, i);
                                   data = data.substr(i + 1, data.size() - i);
                                   double qz = atof(number.c_str());

                                   double processingLatency = atof(data.c_str());

                                   callback(timestamp, id, tx, ty, tz, qw, qx, qy, qz, processingLatency);
                               }
                           }};
        thread.detach();
    }

    int sync(std::string ip, double timestamp, int port = 5801)
    {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1)
            return -1;

        struct sockaddr_in serverAddress;
        memset(&serverAddress, 0, sizeof(serverAddress));

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        serverAddress.sin_addr.s_addr = inet_addr(ip.c_str());

        if (connect(fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
        {
            close(fd);
            return -1;
        }

        std::stringstream stringStream;
        stringStream << timestamp << "\n";
        std::string data = stringStream.str();

        if (send(fd, data.c_str(), data.size(), 0) == -1)
        {
            close(fd);
            return -1;
        }

        close(fd);
        return 0;
    }
};
