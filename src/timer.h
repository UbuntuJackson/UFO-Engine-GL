#ifndef TIMER
#define TIMER
#include <chrono>
namespace ufo{

class Timer{
public:
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    float number_of_seconds_to_count = 0.0f;
    float time_left_when_paused = 0.0f;
    bool is_started = false;
    bool is_paused = false;
    Timer();
    void Start(float _number_of_seconds_to_count);
    void Stop();
    float GetTimeLeft();
    void FastForward(float _delta_time, float _multply_elapsed_time);
    void Pause();
    void Resume();
};
}

#endif
