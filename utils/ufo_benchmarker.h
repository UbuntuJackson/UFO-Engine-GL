#ifndef UFO_BENCHMARKER_H
#define UFO_BENCHMARKER_H
#include <chrono>

namespace ufo{

class BenchMarker{
public:
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    BenchMarker(){
        start_time = std::chrono::steady_clock::now();
    }
    float Stop(){
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start_time).count();
    }
};

}

#endif
