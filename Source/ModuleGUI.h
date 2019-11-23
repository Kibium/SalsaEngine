#pragma once
#include "Module.h"
#include "Globals.h"
#include "SDL.h"
#include <vector>
#include <time.h>

class ModuleGUI :public Module
{
public:
	ModuleGUI();
	~ModuleGUI();
	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void EventManager(SDL_Event);
	void ShowConsole(const char* title, bool* p_opened = NULL);
	void MainMenu();
	void Clear() { logBuffer.clear(); LineOffsets.clear(); }
public:
	void Menu();
	void Scene();
	void GameObjecInfo();
	void ShowHelp();
	void ShowDefWindow();
	int max_h;
	int min_h;
	int max_w;
	int min_w;
	std::vector<float> frames;
	bool showHelpWindow;
	bool showAppWindow;
	bool showScene;
	bool isScene;

};
