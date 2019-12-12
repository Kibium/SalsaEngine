#include "ModuleScene.h"
#include "GameObject.h"
#include "Component.h"
#include "imgui.h"
#include "IconsFontAwesome5.h"
#include "Application.h"
#include "ModuleGUI.h"
#include <algorithm>

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

	root = new GameObject("Root");

	GameObject* obj1 = new GameObject("Pepito");
	obj1->CreateComponent(Type::TRANSFORM);
	obj1->CreateComponent(Type::MESH);
	obj1->CreateComponent(Type::MATERIAL);
	gameObjects.push_back(obj1);
	obj1->parent = root;

	GameObject* obj1C = new GameObject("Pepa");
	obj1C->CreateComponent(Type::TRANSFORM);
	obj1C->CreateComponent(Type::MESH);
	obj1C->CreateComponent(Type::MATERIAL);
	obj1->children.push_back(obj1C);
	obj1C->parent = obj1;

	GameObject* obj1C2 = new GameObject("Jorge");
	obj1C2->CreateComponent(Type::TRANSFORM);
	obj1C2->CreateComponent(Type::MESH);
	obj1C2->CreateComponent(Type::MATERIAL);
	obj1->children.push_back(obj1C2);
	obj1C2->parent = obj1;

	GameObject* obj2 = new GameObject("Marta");
	obj2->CreateComponent(Type::TRANSFORM);
	obj2->CreateComponent(Type::MESH);
	obj2->CreateComponent(Type::MATERIAL);
	gameObjects.push_back(obj2);
	obj2->parent = root;

	SortGameObjects(gameObjects);
	SortGameObjects(obj1->children);

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
	gameObject->parent = root;
	gameObjects.push_back(gameObject);
	return gameObject;
}

void ModuleScene::DeleteGameObject(GameObject *gameObject) {
	for (int i = 0; i < gameObjects.size(); ++i) {
		if (gameObject == gameObjects[i]) {
			gameObjects.erase(gameObjects.begin() + i);
			break;
		}
	}
}

void ModuleScene::DrawGameObject(unsigned index, const std::vector<GameObject*>& objects) {
	for (int i = 0; i < objects.size(); ++i, ++index) {

		// Tree node flags processing
		ImGuiTreeNodeFlags objectFlags = ImGuiTreeNodeFlags_None;
		objectFlags = (objects[i]->children.empty()) ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_DefaultOpen;

		if (selectedObject == objects[i])
			objectFlags |= ImGuiTreeNodeFlags_Selected;

		// Process each gameobject
		if (ImGui::TreeNodeEx(objects[i]->name.c_str(), objectFlags)) {

			if (ImGui::IsItemClicked())
				selectedObject = objects[i];

			// Popup options
			DrawPopup(objects[i]);

			// Change hierarchy structure depending on dragged object
			if (ImGui::BeginDragDropTarget()) {
				if (ImGui::AcceptDragDropPayload("GameObject")) {
					draggedObject->parent == root ? DeleteGameObject(draggedObject) : draggedObject->parent->DeleteChild(draggedObject);
					draggedObject->parent = objects[i];
					objects[i]->children.push_back(draggedObject);
					SortGameObjects(objects[i]->children);
					draggedObject = nullptr;
				}
				ImGui::EndDragDropTarget();
			}

			// Save dragged object
			if (ImGui::BeginDragDropSource()) {
				draggedObject = objects[i];
				ImGui::SetDragDropPayload("GameObject", NULL, NULL);
				ImGui::Text("Move to...");
				ImGui::EndDragDropSource();
			}

			// Process each gameobject's childs
			DrawGameObject(index, objects[i]->children);
			ImGui::TreePop();
		}
	}
}

void ModuleScene::DrawHierarchy(bool *showHierarchy) {
	if (showHierarchy && ImGui::Begin(ICON_FA_CUBES" Hierarchy", showHierarchy)) {
		if (gameObjects.size() > 0 && (ImGui::TreeNode("Root"))) {
			DrawGameObject(0, gameObjects);
			ImGui::TreePop();
		}
	}
	ImGui::End();
}

void ModuleScene::DrawInspector(bool *showInspector) {
	if (showInspector && ImGui::Begin(ICON_FA_INFO_CIRCLE" Inspector", showInspector)) {
		if (gameObjects.size() > 0 && selectedObject != nullptr) {
			selectedObject->DrawComponents();
		}
	}
	ImGui::End();
}

void ModuleScene::DrawPopup(GameObject *gameObject) {
	if (ImGui::IsItemClicked(1)) {
		ImGui::OpenPopup("GameObject Popup");
		selectedObject = gameObject;
	}

	if (ImGui::BeginPopup("GameObject Popup")) { //TODO popupmodal can be useful
		ImGui::TextDisabled("Copy");
		ImGui::TextDisabled("Paste");
		ImGui::Separator();
		ImGui::TextDisabled("Rename");
		if (ImGui::MenuItem("Duplicate")) {
			//GameObject* temp = new GameObject();
			//temp = selectedObject;
			//gameObjects.push_back(temp);
		}

		static bool canDelete = false;
		if (ImGui::MenuItem("Delete")) {
			ImGui::OpenPopup("Delete?");
			LOG("peppe");
			canDelete = true;
		}

		if (ImGui::BeginPopupModal("Delete?", &canDelete, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
			ImGui::Separator();

			//static int dummy_i = 0;
			//ImGui::Combo("Combo", &dummy_i, "Delete\0Delete harder\0");

			static bool dont_ask_me_next_time = false;
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
			ImGui::PopStyleVar();

			if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); gameObject->parent == root ? DeleteGameObject(gameObject) : gameObject->parent->DeleteChild(gameObject); }
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}

		ImGui::Separator();
		if (ImGui::MenuItem("Create Empty")) {
			CreateGameObject();
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

void ModuleScene::SortGameObjects(std::vector<GameObject*>& objects) {
	std::sort(objects.begin(), objects.end(), [](const auto& lhs, const auto& rhs) { return lhs->name < rhs->name; });
}
