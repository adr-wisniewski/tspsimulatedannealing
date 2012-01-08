#include "Timer.h"

#ifdef WIN32
#include <windows.h>
#else
#include <time.h>
#endif

Timer Timer::Instance;	

Timer::Timer() {
#ifdef WIN32
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&frequency));
#endif
}

void Timer::Start() {
#ifdef WIN32
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&referenceTime));
#else
	struct timespec ts;
	::clock_gettime(CLOCK_MONOTONIC, &ts);
	startTime = ts.tv_sec + static_cast<double>(ts.tv_nsec) / (1000 * 1000 * 1000);
#endif
}

double Timer::ElapsedSeconds() {
#ifdef WIN32
	__int64 current;	
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&current));
	current = current - referenceTime;
	return ((double)current /(double)frequency);
#else
	struct timespec ts;
	::clock_gettime(CLOCK_MONOTONIC, &ts);
	double currentTime = ts.tv_sec + static_cast<double>(ts.tv_nsec) / (1000 * 1000 * 1000);
	return currentTime - startTime;
#endif
}