#pragma once

#include<list>
#include "Globals.h"
#include "Module.h"

class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleRenderExercise;
class ModuleTriangle;
class ModuleGUI;
class ModuleShader;
class ModuleTexture;
class ModuleCamera;
class ModuleModelLoader;
class ModuleTime;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

public:
	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleTriangle* triangle = nullptr;
	ModuleGUI* gui = nullptr;
	ModuleShader* shader = nullptr;
	ModuleTexture* texture = nullptr;
	ModuleCamera* camera = nullptr;
	ModuleModelLoader* model = nullptr;
	ModuleTime* time = nullptr;

private:

	std::list<Module*> modules;

};

extern Application* App;
