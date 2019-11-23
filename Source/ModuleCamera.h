#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib.h"

class ModuleCamera : public Module
{
public:
	ModuleCamera();
	 ~ModuleCamera();
	 bool Init();
	 update_status PreUpdate();
	 update_status Update();
	 update_status PostUpdate();
	 bool CleanUp();

	 void SetFOV(float);
	 void SetSpeed(float Speed);
	 void SetAspectRatio(float);

	 void CalculateMatrixes();
	 math::float4x4 LookAt(math::float3, math::float3, math::float3) const;
	 void LookAt(float3);

	 void MoveUp();
	 void MoveDown();
	 void MoveFoward();
	 void MoveBackward();
	 void MoveLeft();
	 void MoveRight();
	 void Rotate(const float, const float );
	 void Orbit(const float, const float);
	 void Focus();

	 Frustum frustum;
	 float aspectRatio;
	 math::float4x4 model;
	 math::float4x4 view;
	 math::float4x4 proj;
	 bool orbit;

private:

	float cameraSpeed = 0.1f;
	float rotationSpeed = 0.02f;


};

