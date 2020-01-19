#ifndef __MODULEGUI_H__
#define __MODULEGUI_H__

#include "Module.h"
#include "Globals.h"
#include "SDL.h"
#include <vector>
#include <time.h>
#include <tchar.h>

#define OFFSET_HIERARCHYW 0.155f
#define OFFSET_HIERARCHYH 0.685f
#define OFFSET_NAVBAR 0.0227

#define OFFSET_CONSOLEW 0.73f
#define OFFSET_CONSOLEH 0.3f
#define OFFSET_CONSOLE_UP 0.7f

#define OFFSET_SCENE_POSX 0.155f
#define OFFSET_SCENE_POSY 0.034f
#define OFFSET_SCENEW 0.5757f
#define OFFSET_SCENEH 0.648f

#define OFFSET_INSPECTORW 0.27f
#define OFFSET_INSPECTORH 0.977f
#define OFFSET_INSPECTOR_POSX 0.73f

#define OFFSET_BUTTON_POSX 0.155f
#define OFFSET_BUTTONW 0.5757f
#define OFFSET_BUTTONH 0.037f


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
	void ShowConsole();
	void ProjectView();
	void MainMenu();
	void MainWindow();
	void Clear() { logBuffer.clear(); LineOffsets.clear(); }
  
public:
	void Game();
	void Scene();
	void GameObjecInfo();
	void ShowHelp();
	void ShowAbout();
	void ShowDefWindow();
	void showExplorer();
	void oldinspector();
	void ShowTimeButtons();//Play Pause Stop 
	ImVec2 GetScenePos();
	bool isScene = false;
	bool isInspector = false;
	void HelpMarker(const char* desc);
	const char* GetInputFile();
	float GetSceneWidth();
	float GetSceneHeight();
	ImVec2 explorerPos;
	float explorerWidth = 0;
	float explorerHeight = 0;
	char* GetInputFileJson();
	ImVec2 cursorScene;

private:
	char szFileName[MAX_PATH] = _T("");
	bool showAboutWindow = false;
	bool showHelpWindow = false;
	bool showAppWindow = false;
	bool showScene = true;

	bool showGame = true;
	bool showHierarchy = true;

	bool showInspector = true;
	std::vector<float> frames;
	int max_h = 0;
	int min_h = 0;
	int max_w = 0;
	int min_w = 0;
	float sceneWidth = 0;
	float sceneHeight = 0;
	float sceneWidthGame = 0;
	float sceneHeightGame = 0;
	bool aspectFixed = false;
	ImVec2 vMin;
	ImVec2 vMax;
	ImVec2 scenePos;

	//Color picker variables
	ImGuiColorEditFlags misc_flags;

};
#endif // __MODULEMODELLOADER_H__
