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

	void DrawGameObjects(const std::vector<GameObject*>& objects);
	void DrawHierarchy(bool *showHierarchy);
	void DrawInspector(bool *showInspector);
	void DrawPopup(GameObject *gameObject);

	void SortGameObjects(std::vector<GameObject*> &objects);
	ComponentCamera* camera = nullptr;

public:
	GameObject* root = nullptr;
	GameObject* dragged = nullptr;
	GameObject* selected = nullptr;

};

#endif // __MODULESCENE_H__
