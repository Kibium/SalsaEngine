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
	void DrawGame(unsigned width, unsigned height);
	void DrawScene(float width, float height);
	update_status PostUpdate();
	bool CleanUp();
	void WindowResized(unsigned width, unsigned height);
	void DrawGrid();
	void SetAxis();
	void SetWireframe(const bool wireframe);

public:
	void* context = nullptr;
	GLuint frameTex;
	GLuint sceneTex;
	ModuleCamera* GameCamera;
private:
	GLuint FBO;
	GLuint RBO;
	GLuint gameFBO;
	GLuint gameRBO;
};
#endif // __MODULERENDER_H__