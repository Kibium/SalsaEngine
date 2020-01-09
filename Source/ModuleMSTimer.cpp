#include "ModuleMSTimer.h"
#include "SDL.h"

void ModuleMSTimer::Start() {

	lastTimeGot = SDL_GetTicks();
	time = 0;
	isCounting = true;
}

float ModuleMSTimer::InitRead() {
	time = SDL_GetTicks();
	lastTimeGot += time;
	return time;
}

float ModuleMSTimer::Read() {

	return time;
}

float ModuleMSTimer::Stop() {

	isCounting = false;
	return time;
}

void ModuleMSTimer::Pause() {
	paused = true;
}

void ModuleMSTimer::Resume() {

}

update_status ModuleMSTimer::Update() {

	//for test purposes, 
	if (isCounting && !isGlobal)
		time = SDL_GetTicks() - lastTimeGot;

	if (isGlobal) {
		frameDelay = 1000 / FPS;
		dt = frameDelay/1000;
	}

	return UPDATE_CONTINUE;
}