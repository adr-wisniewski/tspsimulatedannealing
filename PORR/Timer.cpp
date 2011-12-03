#include "Timer.h"

Timer Timer::Instance;	

Timer::Timer() {
#ifdef WIN32
	QueryPerformanceFrequency( &frequency );
#endif
}

void Timer::Start() {
#ifdef WIN32
	QueryPerformanceCounter( &referenceTime );
#endif
}

double Timer::ElapsedSeconds() {
#ifdef WIN32
	LARGE_INTEGER current;	
	QueryPerformanceCounter( &current );
	current.QuadPart = current.QuadPart - referenceTime.QuadPart;
	 return ((double)current.QuadPart /(double)frequency.QuadPart) ;
#endif
}