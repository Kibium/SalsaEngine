#include "ModuleScene.h"
#include "GameObject.h"
#include "Component.h"
#include "imgui.h"
#include "IconsFontAwesome5.h"
#include "Application.h"
#include "ModuleGUI.h"
#include <algorithm>
#include "optick/optick.h"
#include "ComponentTransform.h"
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

	root = new GameObject();
	root->name = "RootNode";

	gameCamera = new GameObject();
	gameCamera->name = "MainCamera";
	gameCamera->DeleteComponent(Type::TRANSFORM);
	gameCamera->CreateComponent(Type::TRANSFORM);
	gameCamera->CreateComponent(Type::CAMERA);
	gameCamera->parent = root;
	root->children.push_back(gameCamera);

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

	gameCamera->camera->frustum.pos = gameCamera->transform->position;
	gameCamera->camera->frustum.up = gameCamera->transform->rotationQuat * float3::unitY;
	gameCamera->camera->frustum.front = gameCamera->transform->rotationQuat * float3::unitZ;
	gameCamera->camera->CalculateMatrixes();

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
	root->children.push_back(gameObject);
	return gameObject;
}

GameObject* ModuleScene::CreateEmptyGameObject() {
	GameObject* gameObject = new GameObject();
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

void ModuleScene::DeleteGameObjectFlag(GameObject *gameObject) {
	for (int i = 0; i < root->children.size(); ++i) {
		if (gameObject == root->children[i]) {
			//root->children.erase(root->children.begin() + i);
			gameObject->deleteFlag = true;
			break;
		}
	}
}

void ModuleScene::DeleteAll() {
	root->children.clear();
}

void ModuleScene::SortGameObjects(std::vector<GameObject*>& objects) {
	std::sort(objects.begin(), objects.end(), [](const auto& lhs, const auto& rhs) { return lhs->name < rhs->name; });
}

void ModuleScene::DrawGameObjects(const std::vector<GameObject*>& objects) {
	if (objects.size() > 0) {

		for (int i = 0; i < objects.size(); ++i) {

			if (!objects[i]->deleteFlag) {
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
							//dragged->components[0]->position += dragged->parent->components[0]->position;
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
					DrawGameObjects(objects[i]->children);
					ImGui::TreePop();
				}
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
		if (ImGui::MenuItem("Create Empty")) {
			GameObject* newObject = CreateGameObject();
		}
		//if (ImGui::MenuItem("Duplicate")) {
		//	GameObject* duplicate = CreateEmptyGameObject();
		//	duplicate->name = gameObject->name;
		//	duplicate->parent = gameObject->parent;
		//	duplicate->transform = gameObject->transform;
		//	duplicate->model = gameObject->model;
		//	duplicate->mesh = gameObject->mesh;
		//	duplicate->material = gameObject->material;
		//	duplicate->children = gameObject->children;
		//	if (gameObject->parent == root) {
		//		root->children.push_back(duplicate);
		//		//SortGameObjects(root->children);
		//	}
		//	else {
		//		gameObject->parent->children.push_back(duplicate);
		//		//SortGameObjects(duplicate->parent->children);
		//	}
		//	selected = duplicate;
		//}
		if (ImGui::MenuItem("Delete")) {
			gameObject->parent == root ? DeleteGameObjectFlag(gameObject) : gameObject->parent->DeleteChildFlag(gameObject);
			selected = nullptr;
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
