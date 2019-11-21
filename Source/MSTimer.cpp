#include "MSTimer.h"
#include "SDL.h"



MSTimer::MSTimer()
{
}


MSTimer::~MSTimer()
{
}

void MSTimer::Start()
{
	run = true;
	startingTime = SDL_GetTicks();
	currentTime = 0;

}

unsigned int MSTimer::Read()
{
	currentTime = SDL_GetTicks()- startingTime;
	return currentTime;
}

unsigned int MSTimer::Stop()
{
	if (run) {
		run = false;
		currentTime = Read();
		return currentTime;
	}
	else {
		return currentTime;
	}

}
