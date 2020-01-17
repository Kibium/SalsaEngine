#ifndef __MODULERENDER_H__
#define __MODULERENDER_H__
#include "Module.h"
#include "ModuleCamera.h"
#include "Globals.h"
#include "ImGuizmo/ImGuizmo.h"
#include <map>


struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;
class GameObject;
class ComponentCamera;

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
	void DrawAABB(GameObject * go);
	void SetAxis();
	void SetWireframe(const bool wireframe);

	void DrawLine(float3 so);

	void MousePicking(float2 mouse);

	LineSegment CreatingRay(float2 mouse);

	void GetAABBHits(LineSegment ray, std::map<float, GameObject*>& gos);

	void DrawGuizmo();

public:
	void* context = nullptr;
	GLuint frameTex;
	GLuint sceneTex;
	ComponentCamera* GameCamera = nullptr;
	ImGuizmo::OPERATION guizmoOP = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE guizmoMode = ImGuizmo::WORLD;

private:
	GLuint FBO;
	GLuint RBO;
	GLuint gameFBO;
	GLuint gameRBO;
};
#endif // __MODULERENDER_H__