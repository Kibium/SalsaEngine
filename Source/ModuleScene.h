#ifndef __MODULESCENE_H__
#define __MODULESCENE_H__

#include "Module.h"
#include "Globals.h"
#include <vector>

class GameObject;
class ComponentCamera;

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
	void SortGameObjects(std::vector<GameObject*> &objects);

	void DrawGameObjects(const std::vector<GameObject*>& objects);
	void DrawHierarchy(bool *showHierarchy);
	void DrawInspector(bool *show);
	void DrawPopup(GameObject *gameObject);

	void SaveScene();
	void LoadScene(const char *fileName);

public:
	GameObject* root = nullptr;
	GameObject* dragged = nullptr;
	GameObject* selected = nullptr;
	ComponentCamera* camera = nullptr;
	std::vector<GameObject*> gameObjects;
};

#endif // __MODULESCENE_H__
