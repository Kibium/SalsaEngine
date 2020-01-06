#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleTriangle.h"
#include "ModuleShader.h"
#include "ModuleGUI.h"
#include "ModuleTexture.h"
#include "ModuleCamera.h"
#include "ModuleModelLoader.h"
#include "ModuleDebugDraw.h"
#include "ModuleMSTimer.h"
#include <windows.h>

using namespace std;

Application::Application()
{
	modules.push_back(mstimer = new ModuleMSTimer());
	modules.push_back(globalTimer = new ModuleMSTimer());

	modules.push_back(window = new ModuleWindow());
	modules.push_back(gui = new ModuleGUI());
	modules.push_back(renderer = new ModuleRender());
	modules.push_back(input = new ModuleInput());
	modules.push_back(shader = new ModuleShader());
	modules.push_back(texture = new ModuleTexture());
	modules.push_back(camera = new ModuleCamera());
	modules.push_back(debugdraw = new ModuleDebugDraw());
	modules.push_back(model = new ModuleModelLoader());
}

Application::~Application()
{
	for(list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }
}

bool Application::Init()
{
	globalTimer->isGlobal = true;
	globalTimer->Start();
	mstimer->Start();
	bool ret = true;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();

	LOG("Time elapsed for initialization: %0.1f ms \n", mstimer->InitRead());
	mstimer->Stop();
	return ret;
}

update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;

	globalTimer->frameStart = SDL_GetTicks();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();

	globalTimer->frameTime = SDL_GetTicks() - globalTimer->frameStart;

	if (globalTimer->frameDelay > globalTimer->frameTime)
		SDL_Delay(globalTimer->frameDelay - globalTimer->frameTime);

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for(list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}
