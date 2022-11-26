/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#include "TimeServer.hxx"

#include <mutex>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <cstdlib>
#include <chrono>
#include <string>

#include "RageVision.hxx"

TimeServer::TimeServer(int port, long *time, std::mutex *mutex)
{
    mPort = port;
    mTime = time;
    mMutex = mutex;
    mRunning = true;
}

void TimeServer::run()
{
    signal(SIGPIPE, SIG_IGN);

    mServerFd = socket(AF_INET, SOCK_STREAM, 0);
    if (mServerFd == -1)
    {
        std::cerr << "Unable to open tcp socket\n";
        std::exit(-1);
    }

    int opt = 1;
    setsockopt(mServerFd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(mPort);
    serverAddress.sin_addr.s_addr = inet_addr("0.0.0.0");

    if (bind(mServerFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cerr << "Unable to bind to port\n";
        close(mServerFd);
        std::exit(-1);
    }

    if (listen(mServerFd, RageVision::kMjpegConnectionBacklog) == -1)
    {
        std::cerr << "Unable to listen\n";
        close(mServerFd);
        std::exit(-1);
    }

    std::thread thread{[this]
                       {
                           while (mRunning)
                           {
                               struct sockaddr_in clientAddress;
                               socklen_t addressSize = sizeof(clientAddress);
                               int clientFd = accept(mServerFd, (struct sockaddr *)&clientAddress, &addressSize);
                               if (clientFd == -1)
                                   continue;

                               char *buffer = new char[RageVision::kTimeBufferSize];
                               if (!buffer)
                               {
                                   std::cerr << "Out of memory\n";
                                   close(clientFd);
                                   return;
                               }
                               memset(buffer, 0, RageVision::kTimeBufferSize);

                               long time = -1;
                               int bytesRead = 0;
                               while (bytesRead < RageVision::kTimeBufferSize - 1 && buffer[bytesRead == 0 ? 0 : bytesRead - 1] != '\n')
                               {
                                   int tmp = read(clientFd, buffer + bytesRead, RageVision::kTimeBufferSize - 1 - bytesRead);
                                   if (time == -1)
                                       time = std::chrono::high_resolution_clock::now().time_since_epoch().count();

                                   if (tmp < 0)
                                   {
                                       delete[] buffer;
                                       close(clientFd);
                                       return;
                                   }
                                   bytesRead += tmp;

                                   if (tmp < 1)
                                       break;
                               }

                               std::string request{buffer};
                               delete[] buffer;

                               double offset = atof(request.c_str());

                               mMutex->lock();
                               *mTime = time - offset * 1e9;
                               mMutex->unlock();

                               close(clientFd);
                           }
                       }};
    thread.detach();
}

void TimeServer::stop()
{
    mRunning = false;
}
