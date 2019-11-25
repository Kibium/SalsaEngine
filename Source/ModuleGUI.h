#ifndef __MODULEGUI_H__
#define __MODULEGUI_H__

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
	void Scene();
	void GameObjecInfo();
	void ShowHelp();
	void ShowAbout();
	void ShowDefWindow();
	bool isScene;
	bool isInspector;
private:
	bool showAboutWindow;
	bool showHelpWindow;
	bool showAppWindow;
	bool showScene;
	bool showInspector;
	std::vector<float> frames;
	int max_h;
	int min_h;
	int max_w;
	int min_w;
	float sceneWidth;
	float sceneHeight;
	bool aspectFixed;

};
#endif // __MODULEMODELLOADER_H__
