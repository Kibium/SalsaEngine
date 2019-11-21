#pragma once
#include "Module.h"
#include <string>

class ModuleShader : public Module
{
public:
	ModuleShader();
	~ModuleShader();
	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	const char* readShader(const char * path);
	void checkCompileErrors(unsigned int shader, std::string type);
	GLuint def_program;
};


