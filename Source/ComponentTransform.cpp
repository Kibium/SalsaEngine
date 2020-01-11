#include "ComponentTransform.h"
#include "imgui.h"
#include "IconsFontAwesome5.h"
#include "GameObject.h"

ComponentTransform::ComponentTransform() : position(math::float3::zero), rotation(math::float3::zero), scale(math::float3::one) {
	type = Type::TRANSFORM;
	if (myGo != nullptr) {
		position = math::float3(myGo->model->modelPosition.x, myGo->model->modelPosition.y, myGo->model->modelPosition.z);
		rotation = math::float3(myGo->model->modelRotation.x, myGo->model->modelRotation.y, myGo->model->modelRotation.z);
		scale = math::float3(myGo->model->modelScale.x, myGo->model->modelScale.y, myGo->model->modelScale.z);
	}
}

ComponentTransform::ComponentTransform(math::float3 &translation, math::float3 &rotation, math::float3 &scale) : position(position), rotation(rotation), scale(scale) {
}

ComponentTransform::ComponentTransform(const ComponentTransform &transform) : position(transform.position), rotation(transform.rotation), scale(transform.scale) {
}

ComponentTransform::~ComponentTransform() {
}

ComponentTransform ComponentTransform::operator+(const ComponentTransform &transform) const {
	return ComponentTransform(position + transform.position, rotation + transform.rotation, scale + transform.scale);
}

ComponentTransform ComponentTransform::operator-(const ComponentTransform &transform) const {
	return ComponentTransform(position - transform.position, rotation - transform.rotation, scale - transform.scale);
}

update_status ComponentTransform::Update() {
	return UPDATE_CONTINUE;
}

void ComponentTransform::OnEditor() {
	if (ImGui::CollapsingHeader(ICON_FA_RULER_COMBINED" Transform", &canBeDeleted, ImGuiTreeNodeFlags_DefaultOpen)) {
		Component::OnEditor();
		if (ImGui::Checkbox("Active##ComponentTransform", &active)) {
			active ? Enable() : Disable();
		}
		if (myGo != nullptr && myGo->model != nullptr) {
			ImGui::DragFloat3("Position", &myGo->model->modelPosition[0], 0.5F, -9999.F, 9999.F, "%.1f");
			ImGui::DragFloat3("Rotation", &myGo->model->modelRotation[0], 0.5F, -9999.F, 9999.F, "%.1f");
			ImGui::DragFloat3("Scale", &myGo->model->modelScale[0], 0.5F, -9999.F, 9999.F, "%.1f");
		}
		else {
			ImGui::DragFloat3("Position", &position[0], 0.5F, -9999.F, 9999.F, "%.1f");
			ImGui::DragFloat3("Rotation", &rotation[0], 0.5F, -9999.F, 9999.F, "%.1f");
			ImGui::DragFloat3("Scale", &scale[0], 0.5F, -9999.F, 9999.F, "%.1f");
		}
	}
	ImGui::Separator();
}
