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

public:
	std::vector<GameObject*> game_objects;
	GameObject* root = nullptr;
};

#endif // __MODULESCENE_H__
