#include "ModuleTexture.h"
#include "Application.h"
#include "ModuleModelLoader.h"
#include "Globals.h"
#include <glew.h>
#include <il.h>
#include <ilu.h>
#include <ilut.h>
#include <string>


bool ModuleTexture::Init() {
	LOG("Init Module Textures\n");
	ilInit();
	iluInit();
	ilutInit();
	loaded = false;
	return true;
}

bool ModuleTexture::CleanUp() {
	return true;
}

GLuint ModuleTexture::Load(const char *filename) {
	GLuint texture;
	ILuint image;
	ilGenImages(1, &image);

	ilBindImage(image);
	ilLoadImage(filename);

	ILenum error;
	error = ilGetError();
	if (error == IL_COULD_NOT_OPEN_FILE)
	{
		loaded = false;
	}
	else {
		loaded = true;
	}
	ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
	ILinfo ImageInfo;
	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
	{
		iluFlipImage();
	}
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	int width = ilGetInteger(IL_IMAGE_WIDTH);
	int height = ilGetInteger(IL_IMAGE_HEIGHT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, ilGetData());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	App->model->textureWidth.push_back(width);
	App->model->textureHeight.push_back(height);

	return texture;
}

