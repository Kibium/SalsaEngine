#include "Component.h"
#include "GameObject.h"

Component::Component() {
}

Component::~Component() {
}

void Component::Enable() {
	active = true;
}

void Component::Disable() {
	active = false;
}
