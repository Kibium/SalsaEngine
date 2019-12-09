#ifndef __COMPONENTTRANSFORM_H__
#define __COMPONENTTRANSFORM_H__

#include "Component.h"
#include "MathGeoLib.h"

class ComponentTransform : public Component {
public:
	ComponentTransform();
	ComponentTransform(math::float3 &position, math::float3 &rotation, math::float3 &scale);
	ComponentTransform(const ComponentTransform &transform);
	~ComponentTransform();

	ComponentTransform operator+(const ComponentTransform &transform) const;
	ComponentTransform operator-(const ComponentTransform &transform) const;

	void Update() override;
	void OnEditor() override;

public:
	math::float3 position;
	math::float3 rotation;
	math::float3 scale;

	// local and world transform and dirty flag to know if needs to be recalculated
};

#endif // __COMPONENTTRANSFORM_H__