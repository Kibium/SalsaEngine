#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleGUI.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleTexture.h"
#include "ModuleModelLoader.h"
#include "SDL.h"
#include <assert.h>
#include <shlwapi.h>
#include <iostream>
#pragma comment(lib,"shlwapi.lib")

ModuleInput::ModuleInput()
{}

ModuleInput::~ModuleInput()
{}

bool ModuleInput::Init()
{
	LOG("Init SDL input event system\n");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

update_status ModuleInput::Update()
{
	SDL_PumpEvents();
	SDL_Event sdlEvent;

	while (SDL_PollEvent(&sdlEvent) != 0)
	{
		App->gui->EventManager(sdlEvent);
		keyboard = SDL_GetKeyboardState(NULL);
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			return UPDATE_STOP;
			break;

		case SDL_WINDOWEVENT:
			if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED || sdlEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				App->renderer->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2);
			break;
		case SDL_MOUSEWHEEL:
			if (sdlEvent.wheel.y > 0)
				App->camera->MoveFoward();
			
			else if (sdlEvent.wheel.y < 0)
				App->camera->MoveBackward();

			break;
		case SDL_KEYDOWN:

			if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_LALT) {
				App->camera->orbit = true;
			}
			break;
		case SDL_KEYUP:

			if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_LALT) {
				App->camera->orbit = false;
			}
			break;

		case SDL_MOUSEMOTION:
			if (sdlEvent.motion.state && SDL_BUTTON_RMASK && App->gui->isScene)
				App->camera->Orbit(sdlEvent.motion.xrel, -sdlEvent.motion.yrel);
			break;
					
		case SDL_DROPFILE:
			char* newFile = sdlEvent.drop.file;
			DroppedFile(newFile);
			SDL_free(newFile);
			break;
		
		}
	}

	if (keyboard[SDL_SCANCODE_LEFT])
		App->camera->Rotate(0.01, 0);

	if (keyboard[SDL_SCANCODE_RIGHT])
		App->camera->Rotate(-0.01, 0);

	if (keyboard[SDL_SCANCODE_UP])
		App->camera->Rotate(0, 0.01);

	if (keyboard[SDL_SCANCODE_DOWN])
		App->camera->Rotate(0, -0.01);

	if (keyboard[SDL_SCANCODE_Q])
		App->camera->MoveUp();

	if (keyboard[SDL_SCANCODE_E])
		App->camera->MoveDown();

	if (keyboard[SDL_SCANCODE_W])
		App->camera->MoveFoward();

	if (keyboard[SDL_SCANCODE_S])
		App->camera->MoveBackward();

	if (keyboard[SDL_SCANCODE_A])
		App->camera->MoveLeft();

	if (keyboard[SDL_SCANCODE_D])
		App->camera->MoveRight();

	if (keyboard[SDL_SCANCODE_F])
		App->camera->Focus();

	return UPDATE_CONTINUE;
}

bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem\n");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

void ModuleInput::DroppedFile(char* file)
{
	if (file == NULL) {
		LOG("ERROR:: DROPPED FILE NOT VALID OR MISSING\n ");
		return;
	}
	char* extension = PathFindExtensionA(file);
	if (strcmp(".obj", extension) == 0) {
		App->model->SwitchModel(file);
	}
	else if (strcmp(".fbx", extension) == 0) {
		App->model->SwitchModel(file);
	}
	else if (extension == ".png" || extension == ".dds") {
		App->texture->Load(file);
	}
	else if (strcmp(".png", extension) == 0) {
		App->model->SwitchTexture(file);
	}
	else if (strcmp(".dds", extension) == 0) {
		App->model->SwitchTexture(file);
	}
	else if (strcmp(".tga", extension) == 0) {
		App->model->SwitchTexture(file);
	}
	else {
		LOG("ERROR:: FILE FORMAT '%s' NOT ACCEPTED\n ", extension);
	}
	
}