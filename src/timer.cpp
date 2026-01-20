#include <chrono>
#include "timer.h"
namespace ufo{

Timer::Timer(){
    start_time = std::chrono::steady_clock::now();
    is_started = false;
};
void Timer::Start(float _number_of_seconds_to_count){
    number_of_seconds_to_count = _number_of_seconds_to_count;
    start_time = std::chrono::steady_clock::now();
    is_started = true;
}
void Timer::Stop(){
    is_started = false;
}
float Timer::GetTimeLeft(){
    return number_of_seconds_to_count - std::chrono::duration_cast<std::chrono::milliseconds>((std::chrono::steady_clock::now() - start_time)).count();
}
void Timer::FastForward(float _delta_time, float _multply_elapsed_time){
    if(!is_started) return;
    float reduced_time = GetTimeLeft() - _delta_time * _multply_elapsed_time * 1000.0f;
    Start(reduced_time);
}
void Timer::Pause(){
    if(!is_paused) time_left_when_paused = GetTimeLeft();
    is_paused = true;
}
void Timer::Resume(){
    //Guard against starting the timer over continuously.
    if(is_paused) Start(time_left_when_paused);
    is_paused = false;
}
}
