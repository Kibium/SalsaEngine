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
#include "Geometry/AABB.h"


ComponentTransform::ComponentTransform() {
	type = Type::TRANSFORM;
	if (myGo != nullptr) {
		position = float3(myGo->model->modelPosition.x, myGo->model->modelPosition.y, myGo->model->modelPosition.z);
		rotationFloat = float3(myGo->model->modelRotation.x, myGo->model->modelRotation.y, myGo->model->modelRotation.z);
		RotToQuat();
		scale = float3(myGo->model->modelScale.x, myGo->model->modelScale.y, myGo->model->modelScale.z);
	}
}

ComponentTransform::~ComponentTransform() {
}

update_status ComponentTransform::Update() {
	return UPDATE_CONTINUE;
}

void ComponentTransform::RotToQuat() {

	float3 auxRot = rotationFloat;
	auxRot.x = DegToRad(auxRot.x);
	auxRot.y = DegToRad(auxRot.y);
	auxRot.z = DegToRad(auxRot.z);
	rotationQuat = rotationQuat.FromEulerXYZ(auxRot.x, auxRot.y, auxRot.z);
  
}

void ComponentTransform::QuatToFloat() {

	rotationFloat = rotationQuat.ToEulerXYZ();
	rotationFloat.x = RadToDeg(rotationFloat.x);
	rotationFloat.y = RadToDeg(rotationFloat.y);
	rotationFloat.z = RadToDeg(rotationFloat.z);

}

void ComponentTransform::UpdateMatrix()
{
	worldMatrix = worldMatrix * localMatrix.Inverted();
	localMatrix = float4x4::FromTRS(position, rotationQuat, scale);
	worldMatrix = worldMatrix * localMatrix;
}

void ComponentTransform::SetNewMatrix(const float4x4 &newGlobal)
{
	worldMatrix = newGlobal*localMatrix;
	worldMatrix.Decompose(position, rotationQuat, scale);
	QuatToFloat();
	UpdateAABBBox(App->scene->selected);

void ComponentTransform::UpdateAABBBox(GameObject* go)
{
	if (go->model != nullptr) {
		AABB auxBox;
		auxBox.SetNegativeInfinity();
		auxBox.Enclose(go->model->boundingBox);
		auxBox.TransformAsAABB(worldMatrix);
		go->model->modelBox = auxBox;
	}
}

void ComponentTransform::OnEditor() {
	if (ImGui::CollapsingHeader(ICON_FA_RULER_COMBINED" Transform", &canBeDeleted, ImGuiTreeNodeFlags_DefaultOpen)) {
		Component::OnEditor();
		if (ImGui::Checkbox("Active##ComponentTransform", &active)) {
			active ? Enable() : Disable();
		}

		if (ImGui::DragFloat3("Position", &App->scene->selected->transform->position[0], 0.1f)) {
			App->scene->selected->transform->UpdateMatrix();
			UpdateAABBBox(App->scene->selected);
		}

		if (ImGui::DragFloat3("Rotation", &App->scene->selected->transform->rotationFloat[0], 0.1f)) {
			LOG("Rotating");
			App->scene->selected->transform->RotToQuat();
			App->scene->selected->transform->UpdateMatrix();
			UpdateAABBBox(App->scene->selected);
		}
    
		if (ImGui::DragFloat3("Scale", &App->scene->selected->transform->scale[0], 0.5F, -9999.F, 9999.F, "%.1f")) {
			App->scene->selected->transform->UpdateMatrix();
			UpdateAABBBox(App->scene->selected);
    }
	
	ImGui::Separator();
}
