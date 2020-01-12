#include "ComponentTransform.h"
#include "imgui.h"
#include "IconsFontAwesome5.h"
#include "GameObject.h"
#include "Math/Quat.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/float4.h"
#include "Math/float3x3.h"
#include "ModuleScene.h"
#include "Application.h"


ComponentTransform::ComponentTransform() : position(math::float3::zero), rotation(math::float3::zero), scale(math::float3::one) {
	type = Type::TRANSFORM;
	if (myGo != nullptr) {
		position = float3(myGo->model->modelPosition.x, myGo->model->modelPosition.y, myGo->model->modelPosition.z);
		rotation = float3(myGo->model->modelRotation.x, myGo->model->modelRotation.y, myGo->model->modelRotation.z);
		scale = float3(myGo->model->modelScale.x, myGo->model->modelScale.y, myGo->model->modelScale.z);
	}
}


ComponentTransform::~ComponentTransform() {
}


update_status ComponentTransform::Update() {
	return UPDATE_CONTINUE;
}

void ComponentTransform::UpdateMatrix()
{
	worldMatrix = worldMatrix * localMatrix.Inverted();
	localMatrix = float4x4::FromTRS(position, Quat::identity, scale);
	worldMatrix = worldMatrix * localMatrix;

}

void ComponentTransform::SetWorldMatrix(const float4x4 &newWorld)
{
	worldMatrix = newWorld * localMatrix;
}
void ComponentTransform::UpdateAABBBox(GameObject* go)
{
	if (go->model != nullptr) {
		go->model->modelBox.TransformAsAABB(go->transform->worldMatrix);
	}
}



void ComponentTransform::OnEditor() {
	if (ImGui::CollapsingHeader(ICON_FA_RULER_COMBINED" Transform", &canBeDeleted, ImGuiTreeNodeFlags_DefaultOpen)) {
		Component::OnEditor();
		if (ImGui::Checkbox("Active##ComponentTransform", &active)) {
			active ? Enable() : Disable();
		}

		if (ImGui::DragFloat3("Position", &App->scene->selected->transform->position[0], 0.5F, -9999.F, 9999.F, "%.1f")) {
			LOG("moving");
			App->scene->selected->transform->UpdateMatrix();
			UpdateAABBBox(App->scene->selected);
		}
		//ImGui::DragFloat3("Rotation", &rotation[0], 0.5F, -9999.F, 9999.F, "%.1f");
		if (ImGui::DragFloat3("Scale", &App->scene->selected->transform->scale[0], 0.5F, -9999.F, 9999.F, "%.1f")) {
			App->scene->selected->transform->UpdateMatrix();
			UpdateAABBBox(App->scene->selected);
		}
	}
	ImGui::Separator();
}
