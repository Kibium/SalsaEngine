#ifndef __MODULEMSTIMER_H__
#define __MODULEMSTIMER_H__

#include "Module.h"
#include "Globals.h"

class ModuleMSTimer : public Module
{
public:
	ModuleMSTimer() {};
	~ModuleMSTimer() {};
	
	bool isGlobal = false;

	//when true, the delta time is added to the 'time' variable
	bool isCounting = false;
	bool paused = false;

	float time = 0;
	float FPS = 60;
	float dt = 0;

	int frameStart;
	float frameTime;
	float frameDelay;

	void Start();
	float Read();
	void Pause();
	void Resume();
	float Stop();

	update_status Update();
	float InitRead();

	float lastTimeGot = 0;

	/*bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();*/
};

#endif  // __MODULEMSTIMER_H__
