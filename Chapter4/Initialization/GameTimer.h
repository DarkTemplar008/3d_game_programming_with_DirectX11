#pragma once
#include <cstdint>

class GameTimer
{
public:
	GameTimer();
	~GameTimer();

	void Start();
	void Stop();

	void Tick();

	double Delta() const;
	double Total() const;

private:
	double			secs_pre_count_ = 0;
	std::uint64_t	start_time_		= 0;
	bool			started_		= false;
	std::uint64_t	prev_time_		= 0;
	double			delta_			= 0.0;
	double			total_			= 0.0;
};

