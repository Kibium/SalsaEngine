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
	update_status Update();
	bool CleanUp();

public:
	GameObject* CreateGameObject();
	void DrawHierarchy(bool *showHierarchy);
	void DrawInspector(bool *showInspector);
	void DrawPopup(int parentID, int childID);

public:
	std::vector<GameObject*> gameObjects;
	GameObject* root = nullptr;
	GameObject* selectedObject = nullptr;
	int selectedObjectIndex = -1;
};

#endif // __MODULESCENE_H__
