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
	 void SetAspectRatio(float);
	 void SetPos(math::float3);
	 void CalculateMatrixes();

	 math::float4x4 LookAt(math::float3 eye, math::float3 target, math::float3 up);

	 void MoveUp();
	 void MoveDown();
	 void MoveFoward();
	 void MoveBackward();
	 void MoveLeft();
	 void MoveRight();
	 void OrbitX(const float angle);
	 void OrbitY(const float angle);

	 float aspectRatio;
	 float cameraSpeed = 0.005f;
	 Frustum frustum;

	 math::float4x4 model;
	 math::float4x4 view;
	 math::float4x4 proj;

};

