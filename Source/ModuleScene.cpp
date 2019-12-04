#include "ModuleScene.h"
#include "GameObject.h"

ModuleScene::ModuleScene() {
}

ModuleScene::~ModuleScene() {
	for each(GameObject* obj in game_objects)
		delete obj;
	game_objects.clear();
}

bool ModuleScene::Init() {
	LOG("Init Module Scene\n");
	return true;
}

update_status ModuleScene::PreUpdate() {
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update() {
	for each (GameObject* obj in game_objects) {
		if (obj->active)
			obj->Update();
	}
	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate() {
	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp() {
	return true;
}

GameObject* ModuleScene::CreateGameObject() {
	GameObject* gameObject = new GameObject();
	game_objects.push_back(gameObject);
	return gameObject;
}
