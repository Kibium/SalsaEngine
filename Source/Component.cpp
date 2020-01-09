#include "Component.h"
#include "GameObject.h"

Component::Component() {
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
}
