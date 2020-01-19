#ifndef __COMPONENTTRANSFORM_H__
#define __COMPONENTTRANSFORM_H__

#include "Component.h"
#include "MathGeoLib.h"
#define MINIMUM_SCALE 0.001f

class ComponentTransform : public Component {
public:
	ComponentTransform();
	ComponentTransform(const float3 position, const float3 rotation, const float3 scale);
	~ComponentTransform();

	update_status Update() override;
	void RotToQuat();
	void QuatToFloat();
	void UpdateMatrix();
	void SetWorldMatrix();
	void SetNewMatrixLocal(const float4x4 & newLocal);
	void CheckScale();
	void UpdateAABBBox();
	//void UpdateAABBBox(GameObject * go);
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

};

#endif // __COMPONENTTRANSFORM_H__