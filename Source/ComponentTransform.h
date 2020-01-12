#ifndef __COMPONENTTRANSFORM_H__
#define __COMPONENTTRANSFORM_H__

#include "Component.h"
#include "MathGeoLib.h"

class ComponentTransform : public Component {
public:
	ComponentTransform();
	ComponentTransform(math::float3 &, math::float3 &, math::float3 &);
	ComponentTransform(const ComponentTransform &);
	~ComponentTransform();

	ComponentTransform operator+(const ComponentTransform &) const;
	ComponentTransform operator-(const ComponentTransform &) const;

	update_status Update() override;
	void UpdateMatrix();
	void SetWorldMatrix(const float4x4 &);
	void OnEditor() override;

public:
	float3 position = float3(0.0f, 0.0f, 0.0f);
	float3 rotation = float3(0.0f, 0.0f, 0.0f);
	float3 scale = float3(1.0f, 1.0f, 1.0f);
	float4x4 localMatrix = float4x4::identity;
	float4x4 worldMatrix = float4x4::identity;

	// local and world transform and dirty flag to know if needs to be recalculated
};

#endif // __COMPONENTTRANSFORM_H__