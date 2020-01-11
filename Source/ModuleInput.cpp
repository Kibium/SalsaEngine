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

#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentCamera.h"

#include "optick/optick.h"

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
	//ASSIMP SUPPORTED FORMATS
	//COMMON INTERCHANGE FORMATS
	assimpMap[".fbx"] = 1;
	assimpMap[".obj"] = 2;
	assimpMap[".dae"] = 3;
	assimpMap[".gtlf"] = 4;
	assimpMap[".glb"] = 5;
	assimpMap[".blend"] = 6;
	assimpMap[".3ds"] = 7;
	assimpMap[".ase"] = 8;
	assimpMap[".ifc"] = 9;
	assimpMap[".xgl"] = 10;
	assimpMap[".zgl"] = 11;
	assimpMap[".ply"] = 12;
	assimpMap[".dxf"] = 13;
	assimpMap[".lwo"] = 14;
	assimpMap[".lws"] = 15;
	assimpMap[".lxo"] = 16;
	assimpMap[".stl"] = 17;
	assimpMap[".x"] = 18;
	assimpMap[".ac"] = 19;
	assimpMap[".ms3d"] = 20;
	assimpMap[".cob"] = 21;
	assimpMap[".scn"] = 22;
	//ASSIMP MOTION CAPTURE FORMATS
	assimpMap[".bvh"] = 23;
	assimpMap[".csm"] = 24;
	//GRAPHICS ENGINE FORMATS
	assimpMap[".xml"] = 25;
	assimpMap[".irrmesh"] = 26;
	assimpMap[".irr"] = 27;
	//GAME FILE FORMATS
	assimpMap[".mdl"] = 28;
	assimpMap[".mdl2"] = 29;
	assimpMap[".md3"] = 30;
	assimpMap[".pk3"] = 31;
	assimpMap[".mdc"] = 32;
	assimpMap[".md5"] = 33;
	assimpMap[".smd"] = 34;
	assimpMap[".vta"] = 35;
	assimpMap[".ogex"] = 36;
	assimpMap[".3d"] = 37;
	//OTHER FILE FORMATS
	assimpMap[".b3d"] = 38;
	assimpMap[".q3d"] = 39;
	assimpMap[".q3s"] = 40;
	assimpMap[".nff"] = 41;
	assimpMap[".off"] = 42;
	assimpMap[".raw"] = 43;
	assimpMap[".ter"] = 44;
	assimpMap[".hmp"] = 45;
	assimpMap[".ndo"] = 46;
	//DEVIL SUPPORTED FORMATS
	devilMap[".bmp"] = 47;
	devilMap[".cut"] = 48;
	devilMap[".dds"] = 49;
	devilMap[".doom"] = 50;
	devilMap[".exr"] = 51;
	devilMap[".hdr"] = 52;
	devilMap[".gif"] = 53;
	devilMap[".ico"] = 54;
	devilMap[".jp2"] = 55;
	devilMap[".jpg"] = 56;
	devilMap[".lbm"] = 57;
	devilMap[".mng"] = 58;
	devilMap[".pal"] = 59;
	devilMap[".pbm"] = 60;
	devilMap[".pcd"] = 61;
	devilMap[".pcx"] = 62;
	devilMap[".pgm"] = 63;
	devilMap[".pic"] = 64;
	devilMap[".png"] = 65;
	devilMap[".ppm"] = 66;
	devilMap[".psd"] = 67;
	devilMap[".psp"] = 68;
	devilMap[".sgi"] = 69;
	devilMap[".tga"] = 70;
	devilMap[".tif"] = 71;
	
	return ret;
}

update_status ModuleInput::Update()
{
	OPTICK_CATEGORY("UpdateInput", Optick::Category::Input);

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
			if (sdlEvent.wheel.y > 0 && App->gui->isScene)
				App->scene->camera->MoveFoward();
			
			else if (sdlEvent.wheel.y < 0 && App->gui->isScene)
				App->scene->camera->MoveBackward();

			break;
		case SDL_KEYDOWN:

			if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_LALT) {
				App->scene->camera->SetOrbit(true);
			}
			if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_LSHIFT) {

				if (!App->scene->camera->GetSpeeding()) {
					App->scene->camera->SetSpeeding(true);
					App->scene->camera->SetSpeed(App->scene->camera->cameraSpeed * 2);
					App->scene->camera->SetRotationSpeed(App->scene->camera->rotationSpeed * 2);

				}
			}
			break;
		case SDL_KEYUP:

			if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_LALT) {
				App->scene->camera->SetOrbit(false);
			}
			if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_LSHIFT) {

				App->scene->camera->SetSpeed(CAMERA_SPEED);
				App->scene->camera->SetRotationSpeed(ROTATION_SPEED);
				App->scene->camera->SetSpeeding(false);
				

			}
			break;

		case SDL_MOUSEMOTION:
			mouseX = sdlEvent.motion.x;
			mouseY = sdlEvent.motion.y;
			if (sdlEvent.motion.state & SDL_BUTTON_RMASK && App->gui->isScene)
				if(App->scene->camera->GetOrbit())
					App->scene->camera->Orbit(sdlEvent.motion.xrel, -sdlEvent.motion.yrel);
				else
					App->scene->camera->Rotate(sdlEvent.motion.xrel, -sdlEvent.motion.yrel);
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
				LOG("%d\n", App->scene->camera->PickingHit());
			}
			break;

					
		case SDL_DROPFILE:
			char* newFile = sdlEvent.drop.file;
			DroppedFile(newFile);
			SDL_free(newFile);
			break;
		
		}
	}

	if (keyboard[SDL_SCANCODE_LEFT] && App->gui->isScene)
		App->scene->camera->Rotate(0.5, 0);

	if (keyboard[SDL_SCANCODE_RIGHT] && App->gui->isScene)
		App->scene->camera->Rotate(-0.5, 0);

	if (keyboard[SDL_SCANCODE_UP] && App->gui->isScene)
		App->scene->camera->Rotate(0, 0.5);

	if (keyboard[SDL_SCANCODE_DOWN] && App->gui->isScene)
		App->scene->camera->Rotate(0, -0.5);

	if (keyboard[SDL_SCANCODE_Q] && App->gui->isScene)
		App->scene->camera->MoveUp();

	if (keyboard[SDL_SCANCODE_E] && App->gui->isScene)
		App->scene->camera->MoveDown();

	if (keyboard[SDL_SCANCODE_W] && App->gui->isScene)
		App->scene->camera->MoveFoward();

	if (keyboard[SDL_SCANCODE_S] && App->gui->isScene)
		App->scene->camera->MoveBackward();

	if (keyboard[SDL_SCANCODE_A] && App->gui->isScene)
		App->scene->camera->MoveLeft();

	if (keyboard[SDL_SCANCODE_D] && App->gui->isScene)
		App->scene->camera->MoveRight();

	if (keyboard[SDL_SCANCODE_F] && App->gui->isScene)
		App->scene->camera->Focus();

	return UPDATE_CONTINUE;
}

bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem\n");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

void ModuleInput::DroppedFile(const char* file) const
{
	//If the house is BakerHouse.fbx, returns BakerHouse
	App->model->model_name = App->model->GetFilename(file);
	App->model->load_once = false;
	

	if (file == NULL) {
		LOG("ERROR:: DROPPED FILE NOT VALID OR MISSING\n ");
		return;
	}
	char* extension = PathFindExtensionA(file);
	if (assimpMap.find(extension) != assimpMap.end()) {

		LOG("MODEL FILE FORMAT '%s' ACCEPTED\n ", extension);
		// Process file and create empty gameobject
		App->model->AddModel(file);
		auto obj = App->scene->CreateGameObject();
		obj->name = App->model->GetModel(file)->name;

		// Process components
		obj->model = App->model->GetModel(file);
		obj->DeleteComponent(Type::TRANSFORM);
		obj->CreateComponent(Type::TRANSFORM);
		obj->CreateComponent(Type::MESH);
		obj->CreateComponent(Type::MATERIAL);
		App->scene->root->children.push_back(obj);
		App->scene->selected = obj;

	}
	else if (devilMap.find(extension) != devilMap.end()) {
		LOG("TEXTURE FILE FORMAT '%s' ACCEPTED\n ", extension);
		//App->model->SwitchTexture(file);
	}
	else {
		LOG("ERROR:: FILE FORMAT '%s' NOT ACCEPTED\n ", extension);
	}
	
}
