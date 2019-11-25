#ifndef __MODULERENDER_H__
#define __MODULERENDER_H__
#include "Module.h"
#include "Globals.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	void DrawScene(float width, float height);
	update_status PostUpdate();
	bool CleanUp();
	void WindowResized(unsigned width, unsigned height);
	void DrawGrid();
	void SetWireframe(const bool wireframe);

public:
	void* context;
	GLuint frameTex;
private:
	GLuint FBO;
	GLuint RBO;
};
#endif // __MODULERENDER_H__