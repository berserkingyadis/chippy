#include "timer.h"

Timer::Timer(){ reset(); }

void Timer::reset(){
    m_timestamp = std::chrono::high_resolution_clock::now();
}

long Timer::framedeltaMicroseconds(){
    auto diff = std::chrono::high_resolution_clock::now() - m_timestamp;
    long microSeconds = std::chrono::duration_cast<std::chrono::microseconds>(diff).count();
    reset();
    return microSeconds;
}
