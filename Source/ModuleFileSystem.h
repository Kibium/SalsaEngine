#pragma once
#include "Module.h"
#include "Globals.h"



class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem();
	~ModuleFileSystem();
	bool Init();
	bool Exists(const char*);
	bool CleanUp();



};

