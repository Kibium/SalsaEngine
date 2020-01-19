#ifndef __MODULESHADER_H__
#define __MODULESHADER_H__

#include "Application.h"
#include "Module.h"
#include <string>

class ModuleShader : public Module
{
public:
	ModuleShader();
	~ModuleShader();
	bool Init();
	void CreateShader(GLuint & program, const char * vertexFile, const char * fragmentFile);
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void checkCompileErrors(GLuint& shader, std::string type);
	GLuint def_program, phong_program, grid_program, blinn_program, gouraud_program, flat_program, lines_program;

	//GLuint defVS, defFS, phVS, phFS, flatVS, flatFS, blinnVS, blinnFS, gVS, gFS, linesVS, linesFS;
	GLuint test_program;
	GLuint skybox_program;

};
#endif // __MODULESHADER_H__


