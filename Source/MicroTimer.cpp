

#include "SDL.h"
#include "MicroTimer.h"

MicroTimer::MicroTimer()
{
}

MicroTimer::~MicroTimer()
{
}

void MicroTimer::Start()
{
	start_ticks = SDL_GetTicks();
	running = true;
}

float MicroTimer::Read() const
{
	float current_time;

	if (running)
	{
		if (paused)
		{
			current_time = (pause_start_ticks - start_ticks) - elapsed_while_paused_ticks;
		}
		else
		{
			current_time = (SDL_GetTicks() - start_ticks) - elapsed_while_paused_ticks;
		}

	}
	else
	{
		current_time = end_time;
	}

	return current_time;
}

void MicroTimer::Resume()
{
	if (paused)
	{
		elapsed_while_paused_ticks += SDL_GetTicks() - pause_start_ticks;
		paused = false;
	}
}

float MicroTimer::Pause()
{
	if (!paused)
	{
		pause_start_ticks = SDL_GetTicks();
		paused = true;
	}
	return pause_start_ticks - start_ticks - elapsed_while_paused_ticks;
}

bool MicroTimer::IsPaused() const
{
	return paused;
}

float MicroTimer::Stop()
{
	end_time = SDL_GetTicks() - start_ticks - elapsed_while_paused_ticks;
	running = false;

	return end_time;
}


