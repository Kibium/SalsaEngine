#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleGUI.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleModelLoader.h"
#include "SDL.h"
#include <assert.h>

ModuleInput::ModuleInput()
{}

// Destructor
ModuleInput::~ModuleInput()
{}

// Called before render is available
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

// Called every draw update
update_status ModuleInput::Update()
{
	SDL_PumpEvents();
	SDL_Event sdlEvent;

	while (SDL_PollEvent(&sdlEvent) != 0)
	{
		// Esc button is pressed
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
			if (sdlEvent.wheel.y > 0) // scroll up
			{
				// Put code for handling "scroll up" here!
				App->camera->MoveFoward();
			}
			else if (sdlEvent.wheel.y < 0) // scroll down
			{
				// Put code for handling "scroll down" here!
				App->camera->MoveBackward();

			}
			break;
		case SDL_MOUSEMOTION:
			if (sdlEvent.motion.state && SDL_BUTTON_RMASK) {
				if (math::Abs(sdlEvent.motion.xrel) > 0 || math::Abs(sdlEvent.motion.yrel) > 0 && SDL_BUTTON_RMASK) {
					if (keyboard[SDL_SCANCODE_J]) {
						App->camera->Rotate(sdlEvent.motion.xrel, sdlEvent.motion.yrel);
					}
					/*else {
						if (sdlEvent.motion.x > 0)
							App->camera->MoveRight();
						else if (sdlEvent.motion.x < 0)
							App->camera->MoveLeft();
						else if (sdlEvent.motion.y > 0)
							App->camera->MoveUp();
						else if (sdlEvent.motion.y < 0)
							App->camera->MoveDown();
					}*/
				}
			}

			break;
					
		case SDL_DROPFILE:
			char* newFile = sdlEvent.drop.file;
			assert(newFile != NULL);
			App->model->SwitchModel(newFile);
			SDL_free(newFile);
			break;
		
		}
	}
	
	if (keyboard[SDL_SCANCODE_Q])
	{
		App->camera->MoveUp();
	}

	if (keyboard[SDL_SCANCODE_E])
	{
		App->camera->MoveDown();
	}

	if (keyboard[SDL_SCANCODE_W])
	{
		App->camera->MoveFoward();
	}

	if (keyboard[SDL_SCANCODE_S])
	{
		App->camera->MoveBackward();
	}

	if (keyboard[SDL_SCANCODE_A])
	{
		App->camera->MoveLeft();
	}

	if (keyboard[SDL_SCANCODE_D])
	{
		App->camera->MoveRight();
	}
	/*if (keyboard[SDL_SCANCODE_J])
	{
		App->camera->OrbitX(-2);
	}*/
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem\n");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}
