#ifndef __COMPONENTTRANSFORM_H__
#define __COMPONENTTRANSFORM_H__

#include "Component.h"
#include "MathGeoLib.h"

class ComponentTransform : public Component {
public:
	ComponentTransform();
	~ComponentTransform();

	update_status Update() override;
	void RotToQuat();
	void UpdateMatrix();
	void SetNewMatrix(const float4x4 & newGlobal);
	void UpdateAABBBox();
	void OnEditor() override;

public:
	float3 lastPosition = float3::zero;
	float3 position = float3::zero;
	float3 rotationFloat = float3::zero;
	float3 scale = float3(1.0f, 1.0f, 1.0f);
	Quat rotationQuat = Quat::identity;

	float4x4 localMatrix = float4x4::identity;
	float4x4 worldMatrix = float4x4::identity;

	bool updateOnce = false;
	bool updateOnce2 = false;

	// local and world transform and dirty flag to know if needs to be recalculated
};

#endif // __COMPONENTTRANSFORM_H__