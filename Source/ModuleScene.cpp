#include "ModuleScene.h"
#include "GameObject.h"

ModuleScene::ModuleScene() {
}

ModuleScene::~ModuleScene() {
	for (std::vector<GameObject*>::iterator it = game_objects.begin(); it != game_objects.end(); ++it)
		delete *it;
	game_objects.clear();
}

bool ModuleScene::Init() {
	LOG("Init Module Scene\n");
	GameObject* obj1 = new GameObject();
	obj1->name = std::string("Obj1");
	obj1->selected = false;
	obj1->CreateComponent(Type::TRANSFORM);
	obj1->CreateComponent(Type::MESH);
	obj1->CreateComponent(Type::MATERIAL);
	game_objects.push_back(obj1);
	return true;
}

update_status ModuleScene::PreUpdate() {
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update() {
	for (std::vector<GameObject*>::iterator it = game_objects.begin(); it != game_objects.end(); ++it) {
		//if ((*it)->active)
		//	(*it)->Update();
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
