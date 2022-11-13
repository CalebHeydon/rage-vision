/*
Copyright (c) 2022, Caleb Heydon
All rights reserved.
*/

#include "MjpegServer.hxx"

#include <sys/socket.h>
#include <iostream>
#include <cstdlib>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <thread>
#include <memory>
#include <string>
#include <cstddef>
#include <cstdlib>

#include "RageVision.hxx"

MjpegServer::MjpegServer(int port)
{
    mPort = port;
    mRunning = true;
}

void MjpegServer::run()
{
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

                               std::thread clientThread{[this, clientFd]
                                                        {
                                                            char *buffer = new char[RageVision::kHttpBufferSize];
                                                            if (!buffer)
                                                            {
                                                                std::cerr << "Out of memory\n";
                                                                close(clientFd);
                                                                return;
                                                            }
                                                            memset(buffer, 0, RageVision::kHttpBufferSize);

                                                            int bytesRead = 0;
                                                            while (bytesRead < RageVision::kHttpBufferSize - 1 && buffer[bytesRead == 0 ? 0 : bytesRead - 1] != '\n')
                                                            {
                                                                int tmp = read(clientFd, buffer + bytesRead, RageVision::kHttpBufferSize - 1 - bytesRead);

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

                                                            std::shared_ptr<std::string> request = std::make_shared<std::string>(buffer);
                                                            delete[] buffer;

                                                            size_t i = request->find("\r\n");
                                                            if (i == -1)
                                                            {
                                                                close(clientFd);
                                                                return;
                                                            }
                                                            std::string urlLine = request->substr(0, i);
                                                            if (urlLine.find("GET ") != 0)
                                                            {
                                                                close(clientFd);
                                                                return;
                                                            }

                                                            i = urlLine.find("/") + 1;
                                                            if (i == -1)
                                                            {
                                                                close(clientFd);
                                                                return;
                                                            }
                                                            size_t j;
                                                            for (j = i; urlLine[j] >= '0' && urlLine[j] <= '9' && j < urlLine.size(); j++)
                                                                ;
                                                            std::string cameraString = urlLine.substr(i, j - i);
                                                            int camera = std::atoi(cameraString.c_str());

                                                            std::string response{"HTTP/1.1 200 OK\r\nServer: RageVision\r\nContent-Type: multipart/x-mixed-replace; boundary=--BoundaryString\r\n\r\n"};
                                                            const char *responseBuffer = response.c_str();

                                                            int bytesWritten = 0;
                                                            while (bytesWritten < response.size())
                                                            {
                                                                int tmp = write(clientFd, responseBuffer - bytesWritten, response.size() - bytesWritten);

                                                                if (tmp < 1)
                                                                {
                                                                    close(clientFd);
                                                                    return;
                                                                }
                                                                bytesWritten += tmp;
                                                            }

                                                            close(clientFd);
                                                        }};
                               clientThread.detach();
                           }
                       }};
    thread.detach();
}

void MjpegServer::stop()
{
    mRunning = false;
}
