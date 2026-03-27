//========================================
// Timer Class - Works like a stopwatch
//========================================

#ifndef _TIMER_H_INCLUDED_
#define _TIMER_H_INCLUDED_

#include <chrono>

class Timer
{
public:
	//==============
	// Constructor
	//==============

	Timer();

	//=================
	// Timer Control
	//=================

	// Start the timer running
	void Start();

	// Stop the timer running
	void Stop();

	// Reset the timer to zero
	void Reset();

	//==========
	// Timing
	//==========

	// Get time passed (seconds) since timer started or was last reset
	float GetTime();

	// Get time passed (seconds) since last call to this function 
	// If this is the first call, then time since timer was started or last reset is returned
	float GetLapTime();

private:

	//=====================
	// Private Member Data
	//=====================

	// Is timer running
	bool mRunning;

	using Clock = std::chrono::steady_clock;
	Clock mClock;
	Clock::time_point mStart;
	Clock::time_point mLap;
	Clock::time_point mStop;
};

#endif // !_TIMER_H_INCLUDED_
