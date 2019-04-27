#include "timer.h"

Timer::Timer() { reset(); }

void Timer::reset() {
	m_timestamp = std::chrono::high_resolution_clock::now();
}

uint32_t Timer::framedeltaMicroseconds() {
	auto diff = std::chrono::high_resolution_clock::now() - m_timestamp;
	uint32_t microSeconds = std::chrono::duration_cast<std::chrono::microseconds>(diff).count();
	reset();
	return microSeconds;
}
