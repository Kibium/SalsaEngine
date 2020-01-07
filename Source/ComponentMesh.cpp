#include "ComponentMesh.h"
#include "imgui.h"
#include "IconsFontAwesome5.h"
#include "GameObject.h"
#include "ModuleRender.h"
#include "Application.h"

ComponentMesh::ComponentMesh() {
	type = Type::MESH;
}

ComponentMesh::ComponentMesh(const ComponentMesh &mesh) {
}

ComponentMesh::~ComponentMesh() {
}

update_status ComponentMesh::Update() {
	return UPDATE_CONTINUE;
}

void ComponentMesh::OnEditor() {
	if (ImGui::CollapsingHeader(ICON_FA_CUBE " Mesh", &canBeDeleted, ImGuiTreeNodeFlags_DefaultOpen)) {
		Component::OnEditor();
		if (ImGui::Checkbox("Active##ComponentMesh", &active)) {
			active ? Enable() : Disable();
		}
		if (myGo != nullptr) {
			ImGui::InputInt("Mesh", &myGo->model->nmeshes, 0, 0, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputInt("Triangles", &myGo->model->npolys, 0, 0, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputInt("Vertex", &myGo->model->nvertex, 0, 0, ImGuiInputTextFlags_ReadOnly);
			static bool wireframe = false;
			ImGui::Checkbox("Wireframe", &wireframe);
			App->renderer->SetWireframe(wireframe);
		}
	}
	ImGui::Separator();
}
