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
	void DeleteGameObject(GameObject *gameObject);

	void DrawGameObject(unsigned index, const std::vector<GameObject*>& objects);
	void DrawHierarchy(bool *showHierarchy);
	void DrawInspector(bool *showInspector);
	void DrawPopup(GameObject *gameObject);

	void SortGameObjects(std::vector<GameObject*> &objects);

public:
	std::vector<GameObject*> gameObjects;
	GameObject* root = nullptr;
	GameObject* draggedObject = nullptr;
	GameObject* selectedObject = nullptr;
};

#endif // __MODULESCENE_H__
