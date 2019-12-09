#include "ComponentTransform.h"
#include "imgui.h"
#include "IconsFontAwesome5.h"

ComponentTransform::ComponentTransform() : position(math::float3::zero), rotation(math::float3::zero), scale(math::float3::zero) {
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

void ComponentTransform::Update() {
	OnEditor();
}

void ComponentTransform::OnEditor() {
	if (ImGui::CollapsingHeader(ICON_FA_RULER_COMBINED" Transform")) {
		ImGui::DragFloat3("Position", &position[0], 0.5F);
		ImGui::DragFloat3("Rotation", &rotation[0], 0.5F);
		ImGui::DragFloat3("Scale", &scale[0], 0.5F);
	}
}
