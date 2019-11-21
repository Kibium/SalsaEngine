#include "Globals.h"
#include "Application.h"
#include "ModuleTime.h"
#include "ModuleWindow.h"
#include "MicroTimer.h"

#include "imgui.h"
#include "SDL.h"


ModuleTime::ModuleTime()
{
}

ModuleTime::~ModuleTime()
{
	delete realTime;
	delete gameTime;
}

// Called before render is available
bool ModuleTime::Init()
{
	LOG("Initializing Timers\n");
	realTime = new MicroTimer();
	gameTime = new MicroTimer();

	realTime->Start();
	gameTime->Start();

	return true;
}

update_status ModuleTime::PreUpdate()
{
	frame_start_time = gameTime->Read();
	real_frame_start_time = realTime->Read();
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleTime::Update()
{
	return UPDATE_CONTINUE;
}

update_status ModuleTime::PostUpdate()
{
	++frame_count;

	deltaTime = (gameTime->Read() - frame_start_time) * timeScale;
	realTimeDT = realTime->Read() - real_frame_start_time;

	if (fpsLimit)
	{
		float remaining_frame_time = 1000.f / maxFPS - realTimeDT;
		if (remaining_frame_time > 0)
		{
			SDL_Delay(remaining_frame_time);
		}
		deltaTime = (gameTime->Read() - frame_start_time) * timeScale;
		realTimeDT = realTime->Read() - real_frame_start_time;
	}


	time += deltaTime;
	startingTime += realTimeDT;


	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleTime::CleanUp()
{
	delete realTime;
	delete gameTime;

	return true;
}

void ModuleTime::Play()
{
	gameTime->Resume();
}

void ModuleTime::Pause()
{
	if (gameTime->IsPaused())
	{
		gameTime->Resume();
	}
	else {
		gameTime->Pause();
	}
}

