#pragma once

#include <string>

class Constants
{
public:
    static const std::string kVersion;
    static const int kHttpBufferSize = 4096;
    static const int kDefaultMjpegPort = 5800;
    static const int kMjpegConnectionBacklog = 128;
    static const int kTimeConnectionBacklog = 128;
    static const int kDefaultTimePort = 5801;
    static const int kDefaultDataPort = 5800;
    static const int kMaxHamming = 0;
    static constexpr double kMaxError = 0.001;
    static constexpr double kTagSize = 0.1524;
    static const int kTagThreads = 2;
    static constexpr float kTagDecimate = 2.0f;
    static const int kTimeBufferSize = 2048;
    static const int kDefaultFrameWidth = 640;
    static const int kDefaultFrameHeight = 480;
};
