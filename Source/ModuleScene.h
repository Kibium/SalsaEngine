#ifndef __MODULESCENE_H__
#define __MODULESCENE_H__

#include "Module.h"
#include "Globals.h"
#include <vector>

class GameObject;

class ModuleScene : public Module {
public:
	ModuleScene();
	~ModuleScene();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

public:
	GameObject* CreateGameObject();
	void DrawHierarchy();
	void DrawPopup(int parentID, int childID);

public:
	std::vector<GameObject*> gameObjects;
	GameObject* root = nullptr;
	int nodeClicked = -1;

	bool rename = false;
};

#endif // __MODULESCENE_H__

//TODO modulescene to castdistance of raycast, tell me the first triangle that I hit (mouse click)