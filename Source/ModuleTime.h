#pragma once
#include "Module.h"
#include "Globals.h"

class MicroTimer;

class ModuleTime : public Module
{
public:
	ModuleTime();
	~ModuleTime();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	void Play();
	void Pause();

	void ShowTimeControls();
	void ShowTimeOptions();

public:
	float frame_count = 0;

	float timeScale = 1.f;

	float deltaTime = 0.f;
	float realTimeDT = 0.f;

	float time = 0.f;
	float startingTime = 0.f;
	int maxFPS = 60;
	bool fpsLimit = false;

private:
	MicroTimer *realTime = nullptr;
	MicroTimer *gameTime = nullptr;

	float frame_start_time = 0.f;
	float real_frame_start_time = 0.f;

	
	
};
