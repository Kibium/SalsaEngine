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

	root = new GameObject("RootNode");

	GameObject* obj1 = new GameObject("Pepito");
	obj1->parent = root;
	gameObjects.push_back(obj1);

	GameObject* obj1C = new GameObject("Pepa");
	obj1C->parent = obj1;
	obj1->children.push_back(obj1C);

	GameObject* obj1C2 = new GameObject("Jorge");
	obj1C2->parent = obj1;
	obj1->children.push_back(obj1C2);

	GameObject* obj2 = new GameObject("Marta");
	obj2->parent = root;
	gameObjects.push_back(obj2);

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
	if (objects.size() > 0) {

		for (int i = 0; i < objects.size(); ++i, ++index) {

			// Tree node flags processing
			ImGuiTreeNodeFlags objectFlags = ImGuiTreeNodeFlags_SpanFullWidth;
			objectFlags |= (objects[i]->children.empty()) ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_DefaultOpen;

			if (selectedObject == objects[i])
				objectFlags |= ImGuiTreeNodeFlags_Selected;

			// Process each gameobject
			std::string objectName = (objects[i]->isActive) ? objects[i]->name : objects[i]->name + " (Inactive) ";
			if (ImGui::TreeNodeEx(objectName.c_str(), objectFlags)) {

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
}

void ModuleScene::DrawHierarchy(bool *showHierarchy) {
	if (showHierarchy && ImGui::Begin(ICON_FA_CUBES" Hierarchy", showHierarchy)) {
		if (gameObjects.size() > 0 && (ImGui::TreeNode(root->name.c_str()))) {
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
	if (ImGui::BeginPopup("GameObject Popup")) { 
		ImGui::TextDisabled("Copy");
		ImGui::TextDisabled("Paste");
		ImGui::Separator();
		ImGui::TextDisabled("Rename");
		ImGui::TextDisabled("Duplicate");
		if (ImGui::MenuItem("Delete")) {
			gameObject->parent == root ? DeleteGameObject(gameObject) : gameObject->parent->DeleteChild(gameObject);
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
