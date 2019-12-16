#include "ComponentMesh.h"
#include "imgui.h"
#include "IconsFontAwesome5.h"
#include "GameObject.h"

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
	if (ImGui::CollapsingHeader(ICON_FA_CUBE " Mesh", &canDelete, ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::Checkbox("Active", &canDelete)) {
			active ? Enable() : Disable();
		}
	}
	ImGui::Separator();
	Component::OnEditor();
}
