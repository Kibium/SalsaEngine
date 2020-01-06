#ifndef __MODULEGUI_H__
#define __MODULEGUI_H__

#include "Module.h"
#include "Globals.h"
#include "SDL.h"
#include <vector>
#include <time.h>
#include <tchar.h>


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
	void ShowTimeButtons();//Play Pause Stop 
	bool isScene = false;
	bool isInspector = false;
private:
	char* GetInputFile();
	char szFileName[MAX_PATH] = _T("");
	bool showAboutWindow = false;
	bool showHelpWindow = false;
	bool showAppWindow = true;
	bool showScene = true;
	bool showInspector = true;
	std::vector<float> frames;
	int max_h = 0;
	int min_h = 0;
	int max_w = 0;
	int min_w = 0;
	float sceneWidth = 0;
	float sceneHeight = 0;
	bool aspectFixed = false;

	void UpdateMaterial(unsigned int& textureID);

	//Color picker variables
	ImGuiColorEditFlags misc_flags;


};
#endif // __MODULEMODELLOADER_H__
