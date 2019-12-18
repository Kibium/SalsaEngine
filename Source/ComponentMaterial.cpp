#include "ComponentMaterial.h"
#include "imgui.h"
#include "IconsFontAwesome5.h"
#include "GameObject.h"

ComponentMaterial::ComponentMaterial() {
	type = Type::MATERIAL;
}

ComponentMaterial::ComponentMaterial(const ComponentMaterial &material) {
}

ComponentMaterial::~ComponentMaterial() {
}

update_status ComponentMaterial::Update() {
	return UPDATE_CONTINUE;
}

void ComponentMaterial::OnEditor() {
	if (ImGui::CollapsingHeader(ICON_FA_PALETTE" Material", &canBeDeleted, ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::Checkbox("Active##ComponentMaterial", &active)) {
			active ? Enable() : Disable();
		}
	}
	ImGui::Separator();
	Component::OnEditor();
}
