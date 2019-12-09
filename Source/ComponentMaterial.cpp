#include "ComponentMaterial.h"
#include "imgui.h"
#include "IconsFontAwesome5.h"

ComponentMaterial::ComponentMaterial() {
}

ComponentMaterial::ComponentMaterial(const ComponentMaterial & material) {
}

ComponentMaterial::~ComponentMaterial() {
}

void ComponentMaterial::Update() {
}

void ComponentMaterial::OnEditor() {
	if (ImGui::CollapsingHeader(ICON_FA_PALETTE" Material")) {
		ImGui::Text("Mesh");
		ImGui::Text("Mesh");
		ImGui::Text("Mesh");
	}
}
