#ifndef PORR__TIMER_H__INCLUDED
#define PORR__TIMER_H__INCLUDED

class Timer {
public:
	static Timer Instance;	

	Timer();

	void Start();
	double ElapsedSeconds();
protected:

#ifdef WIN32
	__int64 frequency;
	__int64 referenceTime;
#else
	double startTime;
#endif
};

#endif