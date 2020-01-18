#ifndef __SKYBOX_H__
#define __SKYBOX_H__
#include <vector>
#include "Globals.h"
#include <il.h>
#include <ilu.h>
#include <ilut.h>
typedef unsigned int GLuint;
class Skybox
{
public:
	Skybox();
	 ~Skybox();
	GLuint loadCubemap(std::vector<std::string> &);
	void Draw();
	GLuint cubemapTexture;
	std::vector<int> textureWidth;
	std::vector<int> textureHeight;
	//std::vector<GLuint> textures;
	std::vector<ILubyte*> data;


private:
	unsigned int skyboxVAO, skyboxVBO;
	std::vector<std::string> faces;


};

#endif // __SKYBOX_H__