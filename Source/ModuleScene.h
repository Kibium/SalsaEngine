#ifndef __MODULESCENE_H__
#define __MODULESCENE_H__

#include "Module.h"
#include "Globals.h"
#include <vector>

class AABBTree;
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
	GameObject* CreateEmptyGameObject();
	GameObject* CreateGameObject(uint32_t UID, uint32_t ParentUID, const std::string &name, bool Active, bool Static, const char *modelFile = nullptr);
	void DeleteGameObject(GameObject *gameObject);
	void DeleteGameObjectFlag(GameObject *gameObject);
	void DeleteAll();
	void SortGameObjects(std::vector<GameObject*> &objects);

	void DrawGameObjects(const std::vector<GameObject*>& objects);
	void DrawHierarchy(bool *showHierarchy);
	void DrawInspector(bool *show);
	void DrawPopup(GameObject *gameObject);
	void DrawTree();

	void SaveScene();
	void LoadScene(const char *fileName);

public:
	ComponentCamera* camera = nullptr;
	GameObject* root = nullptr;
	GameObject* dragged = nullptr;
	GameObject* selected = nullptr;
	GameObject* gameCamera = nullptr;
	AABBTree* abbTree = nullptr;
	std::vector<GameObject*> gameObjects;

};

#endif // __MODULESCENE_H__
