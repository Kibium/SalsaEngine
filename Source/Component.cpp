#include "Component.h"
#include "GameObject.h"

Component::Component() {
}

Component::~Component() {
}

void Component::Enable() {
	active = true;
}

void Component::Update() {
	if (active) {
		// Call Update method on child class
	}
}

void Component::Disable() {
	active = false;
}
