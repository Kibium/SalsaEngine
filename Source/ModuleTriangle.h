#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib.h"


class ModuleTriangle :public Module
{
public:
	ModuleTriangle();
	virtual ~ModuleTriangle();
	bool Init();
	update_status Update();
	bool CleanUp();
	float4x4 LookAt(float3, float3, float3);
	

private:
	GLuint texture;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	float4x4 model;
};

