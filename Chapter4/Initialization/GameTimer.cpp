#include "pch.h"
#include "GameTimer.h"

GameTimer::GameTimer()
{
	LARGE_INTEGER freq = { 0 };
	QueryPerformanceFrequency(&freq);
	secs_pre_count_ = 1.0 / freq.QuadPart;
}

GameTimer::~GameTimer()
{
}

void GameTimer::Start()
{
	if (started_)
	{
		return;
	}
	started_ = true;

	LARGE_INTEGER cur = { 0 };
	QueryPerformanceCounter(&cur);
	start_time_ = cur.QuadPart;
	prev_time_	= start_time_;
}

void GameTimer::Stop()
{
	if (!started_)
	{
		return;
	}
	started_ = false;

	LARGE_INTEGER cur = { 0 };
	QueryPerformanceCounter(&cur);

	delta_ = (cur.QuadPart - prev_time_)*secs_pre_count_;
	total_ += delta_;
}

void GameTimer::Tick()
{
	if (!started_)
	{
		return;
	}

	LARGE_INTEGER cur = { 0 };
	QueryPerformanceCounter(&cur);

	delta_ = (cur.QuadPart - prev_time_) * secs_pre_count_;
	total_ += delta_;

	prev_time_ = cur.QuadPart;
}

double GameTimer::Delta() const
{
	return delta_;
}

double GameTimer::Total() const
{
	return total_;
}
