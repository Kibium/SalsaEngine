#include "ModuleScene.h"
#include "GameObject.h"
#include "Component.h"
#include "imgui.h"
#include "IconsFontAwesome5.h"
#include "Application.h"
#include "ModuleGUI.h"
#include <algorithm>
#include "optick/optick.h"
#include "ComponentCamera.h"
#include "AABBTree.h"
#include "ComponentTransform.h"
#include "JsonConfig.h"

ModuleScene::ModuleScene() {
}

ModuleScene::~ModuleScene() {
	for (std::vector<GameObject*>::iterator it = root->children.begin(); it != root->children.end(); ++it)
		delete *it;
	root->children.clear();
}

bool ModuleScene::Init() {
	LOG("Init Module Scene\n");
	bool ret = true;
  
	camera = new ComponentCamera();
  
	root = new GameObject("RootNode");
  root->name = "RootNode";
  
	abbTree = new AABBTree(5);

	for (std::vector<GameObject*>::iterator it = root->children.begin(); it != root->children.end(); ++it) {
		if ((*it)->isActive)
			ret = (*it)->Init();
	}

	return ret;
}

update_status ModuleScene::Update() {

	OPTICK_CATEGORY("UpdateScene", Optick::Category::Scene);

	update_status ret = UPDATE_CONTINUE;

	for (std::vector<GameObject*>::iterator it = root->children.begin(); it != root->children.end(); ++it) {
		if ((*it)->isActive)
			ret = (*it)->PreUpdate();
	}

	for (std::vector<GameObject*>::iterator it = root->children.begin(); it != root->children.end(); ++it) {
		if ((*it)->isActive)
			ret = (*it)->Update();
	}

	for (std::vector<GameObject*>::iterator it = root->children.begin(); it != root->children.end(); ++it) {
		if ((*it)->isActive)
			ret = (*it)->PostUpdate();
	}

	return ret;
}

bool ModuleScene::CleanUp() {

	bool ret = true;

	for (std::vector<GameObject*>::iterator it = root->children.begin(); it != root->children.end(); ++it) {
		if ((*it)->isActive)
			ret = (*it)->CleanUp();
	}
	delete camera;
	return ret;
}

GameObject* ModuleScene::CreateGameObject() {
	GameObject* gameObject = new GameObject();
	gameObject->parent = root;
	allGo.push_back(gameObject);
	root->children.push_back(gameObject);
	return gameObject;
}

GameObject* ModuleScene::CreateGameObject(uint32_t UID, uint32_t ParentUID, const std::string &name, bool Active, bool Static, const char *modelFile) {
	GameObject* gameObject = new GameObject(UID, ParentUID, name, Active, Static, modelFile);
	gameObject->parent = root;
	root->children.push_back(gameObject);
	return gameObject;
}

void ModuleScene::DeleteGameObject(GameObject *gameObject) {
	for (int i = 0; i < root->children.size(); ++i) {
		if (gameObject == root->children[i]) {
			root->children.erase(root->children.begin() + i);
			break;
		}
	}
}

void ModuleScene::SortGameObjects(std::vector<GameObject*>& objects) {
	std::sort(objects.begin(), objects.end(), [](const auto& lhs, const auto& rhs) { return lhs->name < rhs->name; });
}

void ModuleScene::DrawGameObjects(const std::vector<GameObject*>& objects) {
	if (objects.size() > 0) {

		for (int i = 0; i < objects.size(); ++i) {

			// Tree node flags processing
			ImGuiTreeNodeFlags objectFlags = ImGuiTreeNodeFlags_SpanFullWidth;
			objectFlags |= (objects[i]->children.empty()) ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_DefaultOpen;

			if (selected == objects[i])
				objectFlags |= ImGuiTreeNodeFlags_Selected;

			// Process each gameobject
			std::string objectName = (objects[i]->isActive) ? objects[i]->name : objects[i]->name + " (Inactive) ";
			if (ImGui::TreeNodeEx(objectName.c_str(), objectFlags)) {

				if (ImGui::IsItemClicked())
					selected = objects[i];

				// Popup options
				DrawPopup(objects[i]);

				// Change hierarchy structure depending on dragged object
				if (ImGui::BeginDragDropTarget()) {
					if (ImGui::AcceptDragDropPayload("GameObject")) {
						if (dragged->name < objects[i]->name && objects[i]->parent == root)
							--i;

						dragged->parent == root ? DeleteGameObject(dragged) : dragged->parent->DeleteChild(dragged);
						dragged->parent = objects[i];
						objects[i]->children.push_back(dragged);
						dragged->components[0]->position += dragged->parent->components[0]->position;
						//SortGameObjects(objects[i]->children);
						dragged = nullptr;
					}
					ImGui::EndDragDropTarget();
				}

				// Save dragged object
				if (ImGui::BeginDragDropSource()) {
					dragged = objects[i];
					ImGui::SetDragDropPayload("GameObject", NULL, NULL);
					ImGui::Text("Move to...");
					ImGui::EndDragDropSource();
				}

				// Process each gameobject's childs
				if(objects.size() > 0) //Toni super fix, it crashed when you'd delete the last one
					DrawGameObjects(objects[i]->children);
				ImGui::TreePop();
			}
		}
	}
}

void ModuleScene::DrawHierarchy(bool *showHierarchy) {
	if (showHierarchy && ImGui::Begin(ICON_FA_CUBES" Hierarchy", showHierarchy)) {
		if (root->children.size() > 0 && (ImGui::TreeNode(root->name.c_str()))) {
			DrawGameObjects(root->children);
			ImGui::TreePop();
		}
	}
	ImGui::End();
}

void ModuleScene::DrawInspector(bool *show) {
	if (show && ImGui::Begin(ICON_FA_INFO_CIRCLE" Inspector", show)) {
		if (root->children.size() > 0 && selected != nullptr) {
			selected->DrawComponents();
		}
	}
	ImGui::End();
}

void ModuleScene::DrawPopup(GameObject *gameObject) {
	if (ImGui::IsItemClicked(1)) {
		ImGui::OpenPopup("GameObject Popup");
		selected = gameObject;
	}
	if (ImGui::BeginPopup("GameObject Popup")) {
		ImGui::Separator();
		ImGui::TextDisabled("Copy");
		ImGui::TextDisabled("Paste");
		ImGui::Separator();
		ImGui::TextDisabled("Rename");
		if (ImGui::MenuItem("Duplicate")) {
			GameObject* duplicate = CreateGameObject();
			duplicate->name = gameObject->name;
			duplicate->parent = gameObject->parent;
			duplicate->components = gameObject->components;
			duplicate->children = gameObject->children;
			if (duplicate->parent == root) {
				root->children.push_back(duplicate);
				//SortGameObjects(root->children);
			}
			else {
				duplicate->parent->children.push_back(duplicate);
				//SortGameObjects(duplicate->parent->children);
			}
			selected = duplicate;
		}
		if (ImGui::MenuItem("Delete")) {
			gameObject->parent == root ? DeleteGameObject(gameObject) : gameObject->parent->DeleteChild(gameObject);
			//delete gameObject;
			selected = nullptr;
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Create Empty")) {
			GameObject* newObject = CreateGameObject();
			selected = nullptr;
			if (selected == nullptr) {
				root->children.push_back(newObject);
			}
			else {
				newObject->parent = gameObject;
				gameObject->children.push_back(newObject);
			}
			selected = newObject;
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

void ModuleScene::SaveScene() {
	JsonConfig config;
	for (auto &obj : root->children)
		config.SaveGameObject(*obj);
	config.SaveJson("SceneData.json");
}

void ModuleScene::LoadScene(const char *fileName) {
	JsonConfig config;
	config.LoadJson(fileName);
}

void ModuleScene::DrawTree() {
	AABBTree* aux = nullptr;
	aux = new AABBTree(5);
	for (auto gameObject : root->children) {

		if (gameObject->model != nullptr) 
			aux->insertObject(gameObject);
		
	}

	aux->DrawTree();
	abbTree = aux;
	aux = nullptr;
}
