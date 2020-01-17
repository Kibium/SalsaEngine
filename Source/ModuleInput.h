#ifndef __MODULEINPUT_H__
#define __MODULEINPUT_H__
#include "Module.h"
#include "Globals.h"
#include <iostream>
#include <map>
#include <string>
#include <iterator>



typedef unsigned __int8 Uint8;

class ModuleInput : public Module
{
public:
	
	ModuleInput();
	~ModuleInput();

	bool Init();
	update_status Update();
	bool CleanUp();
	void DroppedFile(const char *)const;
	float2 mousepos;
	float4 ray_clip, ray_eye;
	float3 ray_world;
	float4 temp;
	int mouseX, mouseY;
	
private:
	
	float pickX, pickY;
	std::map<std::string, int> assimpMap;
	std::map<std::string, int> devilMap;
	const Uint8 *keyboard = NULL;

	
};
#endif // __MODULEINPUT_H__