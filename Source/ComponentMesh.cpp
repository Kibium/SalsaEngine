#include "ComponentMesh.h"
#include "imgui.h"
#include "IconsFontAwesome5.h"

ComponentMesh::ComponentMesh() {
}

ComponentMesh::ComponentMesh(const ComponentMesh &mesh) {
}

ComponentMesh::~ComponentMesh() {
}

void ComponentMesh::Update() {
}

void ComponentMesh::OnEditor() {
	if (ImGui::CollapsingHeader(ICON_FA_CUBE " Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("Active", &active);
	}
	ImGui::Separator();
}
