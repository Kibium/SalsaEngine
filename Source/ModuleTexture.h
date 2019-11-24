#ifndef __MODULETEXTURE_H__
#define __MODULETEXTURE_H__

#include "Module.h"

typedef unsigned int GLuint;
typedef unsigned int ILenum;
struct ILinfo;

class ModuleTexture : public Module {
public:
	ModuleTexture() {}
	~ModuleTexture() {}

	bool Init();
	bool CleanUp();

public:
	GLuint Load(char *filename);
	bool loaded;

};

#endif // __MODULETEXTURE_H__

