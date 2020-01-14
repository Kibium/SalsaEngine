#ifndef __COMPONENTCAMERA_H__
#define __COMPONENTCAMERA_H__

#include "Component.h"
#include "Globals.h"
#include "MathGeoLib.h"
#include "GameObject.h"

#define SIZE_FACTOR 3
#define CAMERA_SPEED 0.1f
#define ROTATION_SPEED 0.02f
#define MAXIMUM_PITCH 89.f
#define MINIMUM_PITCH -89.f

enum in_out_frustum
{
	ABB_OUT = 0,
	ABB_IN,
	INTERSECT
};


class ComponentCamera : public Component
{
public:
	ComponentCamera();
    ~ComponentCamera();
	ComponentCamera(const ComponentCamera &cam);


	update_status Update() override;
	void OnEditor() override;
	bool Init();
	bool CleanUp();

	void SetFOV(float);
	void SetSpeeding(bool);
	void SetSpeed(float);
	void SetRotationSpeed(float);
	void SetAspectRatio(float);
	void SetOrbit(bool);


	bool GetSpeeding() { return speeding; };
	bool GetOrbit() { return orbit; };
	float GetSpeed() { return cameraSpeed; };
	float GetRotationSpeed() { return rotationSpeed; };

	void CalculateMatrixes();
	math::float4x4 LookAt(math::float3, math::float3, math::float3) const;
	void LookAt(float3);

	void MoveUp();
	void MoveDown();
	void MoveFoward();
	void MoveBackward();
	void MoveLeft();
	void MoveRight();
	void Rotate(const float, const float);
	void Orbit(const float, const float);
	void Focus();
	void DrawFrustum();
	bool PickingAABBHit();
	bool PickingTriangleHit();
	in_out_frustum ContainsAABOX(const AABB & refBox);

	Frustum frustum;
	float aspectRatio;
	math::float4x4 model;
	math::float4x4 view;
	math::float4x4 proj;
	math::float4x4 auxView;
	float cameraSpeed = CAMERA_SPEED;
	float rotationSpeed = ROTATION_SPEED;
private:
	bool CompareDistances(GameObject* g1, GameObject* g2);
	void SortByDistance();
	float lastX = 0;
	float lastY = 0;
	bool orbit = false;
	bool speeding = false;
	LineSegment picking;
	vector<GameObject*> objectsHit, sorted;

	float3 hit_point;
	float distance;


};
#endif // __COMPONENTCAMERA_H__

