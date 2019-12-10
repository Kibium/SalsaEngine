#include "ModuleScene.h"
#include "GameObject.h"
#include "Component.h"

ModuleScene::ModuleScene() {
}

ModuleScene::~ModuleScene() {
	for (std::vector<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
		delete *it;
	gameObjects.clear();
}

bool ModuleScene::Init() {
	LOG("Init Module Scene\n");
	GameObject* obj1 = new GameObject();
	obj1->name = std::string("Obj1");
	obj1->selected = false;
	obj1->CreateComponent(Type::TRANSFORM);
	obj1->CreateComponent(Type::MESH);
	obj1->CreateComponent(Type::MATERIAL);
	gameObjects.push_back(obj1);

	GameObject* obj1C = new GameObject();
	obj1C->name = std::string("Obj1 Child");
	obj1C->selected = false;
	obj1C->CreateComponent(Type::TRANSFORM);
	obj1C->CreateComponent(Type::MESH);
	obj1C->CreateComponent(Type::MATERIAL);
	obj1->children.push_back(obj1C);

	GameObject* obj1C2 = new GameObject();
	obj1C2->name = std::string("Obj1222 Child");
	obj1C2->selected = false;
	obj1C2->CreateComponent(Type::TRANSFORM);
	obj1C2->CreateComponent(Type::MESH);
	obj1C2->CreateComponent(Type::MATERIAL);
	obj1->children.push_back(obj1C2);

	GameObject* obj2 = new GameObject();
	obj2->name = std::string("Obj2");
	obj2->selected = false;
	obj2->CreateComponent(Type::TRANSFORM);
	obj2->components[0]->ID = 34;
	obj2->CreateComponent(Type::MESH);
	obj2->CreateComponent(Type::MATERIAL);
	gameObjects.push_back(obj2);
	return true;
}

update_status ModuleScene::PreUpdate() {
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update() {
	//for (std::vector<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it) {
		//if ((*it)->active)
		//	(*it)->Update();
	//}
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
	gameObjects.push_back(gameObject);
	return gameObject;
}

void ModuleScene::DrawHierarchy() {
	if (gameObjects.size() > 0) {

		// Loop through all game objects
		for (int i = 0; i < gameObjects.size(); ++i) {

			// Objects with no childs
			if (gameObjects[i]->children.empty()) {
				ImGui::TreeNodeEx((void*)(intptr_t)&gameObjects[i]->name, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, "%s", gameObjects[i]->name.c_str());
				if (ImGui::IsItemClicked())
					nodeClicked = i;

				DrawPopup(i, -1);
			}
			// Objects with childs
			else {
				if (ImGui::TreeNodeEx((void*)(intptr_t)&gameObjects[i]->name, ImGuiTreeNodeFlags_DefaultOpen, "%s", gameObjects[i]->name.c_str())) {
					DrawPopup(i, -1);
					for (int j = 0; j < gameObjects[i]->children.size(); ++j) {
						ImGui::TreeNodeEx((void*)(intptr_t)&gameObjects[i]->children[i]->name, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, "%s", gameObjects[i]->children[j]->name.c_str());
						DrawPopup(i, j);
					}
					ImGui::TreePop();
				}
				if (ImGui::IsItemClicked())
					nodeClicked = i;
			}
		}
	}
}

void ModuleScene::DrawPopup(int parentID, int childID) {
	if (ImGui::IsItemClicked(1)) {
		ImGui::OpenPopup("my_select_popup");
	}
	if (ImGui::BeginPopup("my_select_popup")) { //popupmodal
		if (ImGui::MenuItem("Copy")) {
			//TODO
		}
		if (ImGui::MenuItem("Paste")) {
			//TODO
		}
		ImGui::Separator();
		if (ImGui::BeginMenu("Rename")) {
			char name[140] = "";
			ImGui::InputTextWithHint("", "Enter name here", &name[0], 140);
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Duplicate")) {
			GameObject* temp = gameObjects[parentID];
			gameObjects.push_back(temp);
		}
		if (ImGui::MenuItem("Delete")) {
			gameObjects.erase(gameObjects.begin() + parentID);
		}
		ImGui::Separator();
		ImGui::MenuItem("Create Empty");
		if (ImGui::BeginMenu("3D Object")) {
			if (ImGui::MenuItem("Cube")) {
				//TODO
			}
			if (ImGui::MenuItem("Sphere")) {
				//TODO
			}
			if (ImGui::MenuItem("Cylinder")) {
				//TODO
			}
			if (ImGui::MenuItem("Torus")) {
				//TODO
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}
}
