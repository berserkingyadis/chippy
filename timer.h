#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
public:
	Timer();
	void reset();
	uint32_t framedeltaMicroseconds();
private:
	std::chrono::high_resolution_clock::time_point m_timestamp;
};
#endif
