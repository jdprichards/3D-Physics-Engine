//========================================
// Timer Class - Works like a stopwatch
//========================================

#include "Timer.h"

//==============
// Constructor
//==============

Timer::Timer()
{
	// Reset and start the timer
	Reset();
	mRunning = true;
}

//=================
// Timer Control
//=================

// Start the timer running
void Timer::Start()
{
	if (!mRunning)
	{
		auto now = mClock.now();
		mStart += now - mStop;
		mLap += now - mStop;
		mRunning = true;
	}
}

// Stop the timer running
void Timer::Stop()
{
	if (mRunning)
	{
		mStop = mClock.now();
		mRunning = false;
	}
}

// Reset the timer to zero
void Timer::Reset()
{
	mStart = mLap = mStop = mClock.now();
}

//==========
// Timing
//==========

// Get time passed (seconds) since the timer started or was last reset
float Timer::GetTime()
{
	Clock::time_point lastRunningTime = mRunning ? mClock.now() : mStop;
	std::chrono::duration<float> timePassed = lastRunningTime - mStart;
	return timePassed.count();
}


// Get time passed (seconds) since last call to this function 
// If this is the first call, then time since timer was started or last reset is returned
float Timer::GetLapTime()
{
	Clock::time_point lastRunningTime = mRunning ? mClock.now() : mStop;
	std::chrono::duration<float> timePassed = lastRunningTime - mLap;
	mLap = lastRunningTime;
	return timePassed.count();
}