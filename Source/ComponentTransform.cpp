#include "ComponentTransform.h"

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
}

void ComponentTransform::OnEditor() {
}
