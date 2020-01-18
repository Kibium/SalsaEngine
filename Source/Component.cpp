#include "Component.h"
#include "GameObject.h"
#include "pcg/pcg_basic.h"

Component::Component() : position(math::float3::zero), rotation(math::float3::zero), scale(math::float3::one) {
	UUID = pcg32_random();
}

Component::~Component() {
}

void Component::Enable() {
	active = true;
}

update_status Component::Update() {
	return UPDATE_CONTINUE;
}

void Component::Disable() {
	active = false;
}

void Component::OnEditor() {
	if (!canBeDeleted) {
		LOG("Deleted component!\n");
		myGo->DeleteComponent(type);
		canBeDeleted = true;
	}
	//ImGui::Text("UUID: %d", UUID);
}
