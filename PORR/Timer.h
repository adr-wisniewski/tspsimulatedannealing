#ifndef PORR__TIMER_H__INCLUDED
#define PORR__TIMER_H__INCLUDED

#ifdef WIN32
#include <windows.h>
#endif

class Timer {
public:
	static Timer Instance;	

	Timer();

	void Start();
	double ElapsedSeconds();
protected:

#ifdef WIN32
	LARGE_INTEGER frequency;
	LARGE_INTEGER referenceTime;
#endif
};

#endif