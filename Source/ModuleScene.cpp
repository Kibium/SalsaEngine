#include "ModuleScene.h"
#include "GameObject.h"
#include "Component.h"
#include "imgui.h"
#include "IconsFontAwesome5.h"
#include "Application.h"
#include "ModuleGUI.h"

ModuleScene::ModuleScene() {
}

ModuleScene::~ModuleScene() {
	for (std::vector<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
		delete *it;
	gameObjects.clear();
}

bool ModuleScene::Init() {
	LOG("Init Module Scene\n");
	bool ret = true;

	GameObject* obj1 = new GameObject("GameObject1");
	obj1->CreateComponent(Type::TRANSFORM);
	obj1->CreateComponent(Type::MESH);
	obj1->CreateComponent(Type::MATERIAL);
	gameObjects.push_back(obj1);

	GameObject* obj1C = new GameObject("GameObject1 Child");
	obj1C->CreateComponent(Type::TRANSFORM);
	obj1C->CreateComponent(Type::MESH);
	obj1C->CreateComponent(Type::MATERIAL);
	obj1->children.push_back(obj1C);

	GameObject* obj1C2 = new GameObject("GameObject1 Child2");
	obj1C2->CreateComponent(Type::TRANSFORM);
	obj1C2->CreateComponent(Type::MESH);
	obj1C2->CreateComponent(Type::MATERIAL);
	obj1->children.push_back(obj1C2);

	GameObject* obj2 = new GameObject("GameObject2");
	obj2->CreateComponent(Type::TRANSFORM);
	obj2->CreateComponent(Type::MESH);
	obj2->CreateComponent(Type::MATERIAL);
	gameObjects.push_back(obj2);

	for (std::vector<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it) {
		if ((*it)->isActive)
			ret = (*it)->Init();
	}

	return ret;
}

update_status ModuleScene::Update() {

	update_status ret = UPDATE_CONTINUE;

	for (std::vector<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it) {
		if ((*it)->isActive)
			ret = (*it)->PreUpdate();
	}

	for (std::vector<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it) {
		if ((*it)->isActive)
			ret = (*it)->Update();
	}

	for (std::vector<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it) {
		if ((*it)->isActive)
			ret = (*it)->PostUpdate();
	}

	return ret;
}

bool ModuleScene::CleanUp() {

	bool ret = true;

	for (std::vector<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it) {
		if ((*it)->isActive)
			ret = (*it)->CleanUp();
	}

	return ret;
}

GameObject* ModuleScene::CreateGameObject() {
	GameObject* gameObject = new GameObject();
	gameObjects.push_back(gameObject);
	return gameObject;
}

void ModuleScene::DrawHierarchy(bool *showHierarchy) {
	if (showHierarchy) {
		if (ImGui::Begin(ICON_FA_CUBES" Hierarchy", showHierarchy)) {
			if (gameObjects.size() > 0) {

				// Loop through all game objects
				for (int i = 0; i < gameObjects.size(); ++i) {

					ImGuiTreeNodeFlags objectFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
					objectFlags |= (selectedObjectIndex == i) ? ImGuiTreeNodeFlags_Selected : NULL;

					// Objects with no childs
					if (gameObjects[i]->children.empty()) {
						ImGui::TreeNodeEx((void*)(intptr_t)&gameObjects[i]->name, objectFlags, "%s", gameObjects[i]->name.c_str());
						if (ImGui::IsItemClicked())
							selectedObjectIndex = i;
						DrawPopup(i, -1);
					}
					// Objects with childs
					else {
						ImGui::TreeNodeEx((void*)(intptr_t)&gameObjects[i]->name, objectFlags, "%s", gameObjects[i]->name.c_str());
						//if (true) {
						//	DrawPopup(i, -1);
						//	for (int j = 0; j < gameObjects[i]->children.size(); ++j) {
						//		ImGui::TreeNodeEx((void*)(intptr_t)&gameObjects[i]->children[i]->name, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, "%s", gameObjects[i]->children[j]->name.c_str());
						//		DrawPopup(i, j);
						//	}
						//	ImGui::TreePop();
						//}
						if (ImGui::IsItemClicked())
							selectedObjectIndex = i;
					}
				}
			}
		}
		ImGui::End();
	}
}

void ModuleScene::DrawInspector(bool *showInspector) {
	if (showInspector) {
		if (ImGui::Begin(ICON_FA_INFO_CIRCLE" Inspector", showInspector)) {
			if (gameObjects.size() > 0 && selectedObjectIndex != -1) {
				gameObjects[selectedObjectIndex]->DrawComponents();
			}
		}
		ImGui::End();
	}
}

void ModuleScene::DrawPopup(int parentID, int childID) {
	if (ImGui::IsItemClicked(1)) 
		ImGui::OpenPopup("my_select_popup");
	
	if (ImGui::BeginPopup("my_select_popup")) { //TODO popupmodal can be useful
		ImGui::TextDisabled("Copy");
		ImGui::TextDisabled("Paste");
		ImGui::Separator();
		ImGui::TextDisabled("Rename");
		if (ImGui::MenuItem("Duplicate")) {
			GameObject* temp = new GameObject();
			temp = gameObjects[selectedObjectIndex+1];
			gameObjects.push_back(temp);
		}
		if (ImGui::MenuItem("Delete")) {
			gameObjects.erase(gameObjects.begin() + parentID);
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Create Empty")) {
			GameObject* temp = new GameObject();
			gameObjects.push_back(temp);
		}
		if (ImGui::BeginMenu("3D Object")) {
			ImGui::TextDisabled("Cube");
			ImGui::TextDisabled("Sphere");
			ImGui::TextDisabled("Cylinder");
			ImGui::TextDisabled("Torus");
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}
}
