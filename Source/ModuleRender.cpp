#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleShader.h"
#include "ModuleCamera.h"
#include "ModuleGUI.h"
#include "ModuleModelLoader.h"
#include "debugdraw.h"
#include "ModuleDebugDraw.h"
#include "Skybox.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "SDL.h"
#include "MathGeoLib.h"
#include "optick/optick.h"
#include "ModuleScene.h"
#include "ComponentCamera.h"
#include "AABBTree.h"
#include "Geometry/LineSegment.h"


ModuleRender::ModuleRender()
{
}

// Destructor
ModuleRender::~ModuleRender()
{
}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Init Render\n");
	LOG("Creating Renderer context\n");
	context = SDL_GL_CreateContext(App->window->window);
	GLenum err = glewInit();

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_BLEND);
	glGenFramebuffers(1, &FBO);
	GameCamera = new ComponentCamera();
	GameCamera->Init();
	GameCamera->frustum.pos = math::float3(-29.f, 12.60f, -34.78f);
	GameCamera->frustum.up = math::float3(0.192f, 0.960f, 0.205f);
	GameCamera->frustum.front = math::float3(0.655f, -0.281f, 0.701f);
	//GameCamera->model = math::float4x4::FromTRS(GameCamera->frustum.pos, math::float3x3::RotateY(math::pi / 4.0f), math::float3(1.0f, 1.0f, 1.0f));
	GameCamera->CalculateMatrixes();

	return true;
}

update_status ModuleRender::PreUpdate()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	OPTICK_CATEGORY("UpdateRender", Optick::Category::Rendering);
	
	//DrawScene();
	return UPDATE_CONTINUE;
}
void ModuleRender::DrawGame(unsigned width, unsigned height)
{
	if (gameFBO != 0)
	{
		//Generate FrameBuffer if necessary
		glDeleteFramebuffers(1, &gameFBO);
	}
	if (sceneTex != 0)
	{
		glDeleteTextures(1, &sceneTex);
	}
	glGenTextures(1, &sceneTex);

	if (gameRBO != 0)
	{
		glDeleteRenderbuffers(1, &gameRBO);
	}
	glGenFramebuffers(1, &gameFBO);
	glGenRenderbuffers(1, &gameRBO);


	glBindRenderbuffer(GL_RENDERBUFFER, gameRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, sceneTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, gameFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gameRBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("OPENGL::ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, gameFBO);

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glUseProgram(App->shader->test_program);
	
	glUniformMatrix4fv(glGetUniformLocation(App->shader->test_program, "view"), 1, GL_TRUE, &App->scene->gameCamera->camera->view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->shader->test_program, "proj"), 1, GL_TRUE, &App->scene->gameCamera->camera->proj[0][0]);
	//App->scene->camera->DrawFrustum();

	for (auto gameObject : App->scene->root->children) {
		if (gameObject->model != nullptr) {
			DrawAABB(gameObject);
			App->scene->DrawTree();
			if (App->scene->gameCamera->camera->ContainsAABOX(gameObject->model->modelBox)) {
				glUniformMatrix4fv(glGetUniformLocation(App->shader->test_program, "model"), 1, GL_TRUE, &gameObject->transform->worldMatrix[0][0]);
				gameObject->model->Draw();
			}
		}

	}
	DrawGrid();
//	if(App->scene->camera->ContainsAABOX(App->model->modelBox)!= 0)
		//App->model->Draw();
	//PINTAR AQUI DRAWDEBUG
	glUseProgram(0);
	App->skybox->Draw();
	App->debugdraw->Draw(App->scene->gameCamera->camera, gameFBO, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void ModuleRender::DrawScene(const float width, const float height) {
	if (FBO == 0)
	{
		//Generate FrameBuffer if necessary
		glCreateFramebuffers(1, &FBO);
	}
	if (frameTex != 0)
	{
		glDeleteTextures(1, &frameTex);
	}
	glGenTextures(1, &frameTex);

	if (RBO != 0)
	{
		glDeleteRenderbuffers(1, &RBO);
	}
	glGenRenderbuffers(1, &RBO);


	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, frameTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameTex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("OPENGL::ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

/* TONI USED THIS TO RENDER FIGURES AND THE GRID
	//glUseProgram(App->shader->grid_program);
	DrawGrid();

	glUseProgram(App->shader->phong_program);

	for (int i = 0; i < App->model->figures.size(); ++i) {
		const ModuleModelLoader::Figure& f = App->model->figures[i];
		App->model->RenderMesh(f, App->model->materials[f.material], f.transform, App->camera->view, App->camera->proj);
	}
*/

	glUseProgram(App->shader->def_program);

	glUniformMatrix4fv(glGetUniformLocation(App->shader->def_program, "view"), 1, GL_TRUE, &App->scene->camera->view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->shader->def_program, "proj"), 1, GL_TRUE, &App->scene->camera->proj[0][0]);
	App->scene->gameCamera->camera->DrawFrustum();
	for (auto gameObject : App->scene->root->children) {
		glUniformMatrix4fv(glGetUniformLocation(App->shader->def_program, "model"), 1, GL_TRUE, &gameObject->transform->worldMatrix[0][0]);
		if (gameObject->model != nullptr) {
			gameObject->model->Draw();
			DrawAABB(gameObject);
			App->scene->DrawTree();
		}
			
			
	}
	glUseProgram(0);
	App->skybox->Draw();
	DrawGrid();
	App->debugdraw->Draw(App->scene->camera, FBO, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}


update_status ModuleRender::PostUpdate()
{
	
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");
	SDL_GL_DeleteContext(context);
	glDeleteTextures(1, &frameTex);
	glDeleteFramebuffers(1, &FBO);
	glDeleteRenderbuffers(1, &RBO);
	delete GameCamera;
	//Destroy window

	return true;
}

void ModuleRender::WindowResized(unsigned width, unsigned height)
{
	glViewport(0, 0, width, height);
}
void ModuleRender::DrawGrid() {

	// Lines white
	dd::xzSquareGrid(-100.0f, 100.0f, 0.0f, 4.0f, math::float3(0.0f, 0.0f, 0.0f));

}
void ModuleRender::DrawAABB(GameObject* go) {

	dd::aabb(go->model->modelBox.minPoint, go->model->modelBox.maxPoint, math::float3(0.0f, 0.0f, 0.0f));
}

void ModuleRender::SetWireframe(const bool wireframe) {
	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
}

void ModuleRender::MousePicking(float2 mouse) {

	std::map<float, GameObject*> hits;
	LineSegment ray = CreatingRay(mouse);
	GetAABBHits(ray, hits);
	float minDistance = App->scene->camera->frustum.farPlaneDistance*20;
	if (hits.size() > 0) {
		GameObject* aux = nullptr;
		for (std::map<float, GameObject*>::const_iterator it = hits.begin(); it != hits.end(); ++it) {
			GameObject* go = it->second;
			if (go->model != nullptr) {
				LineSegment localRay(ray);
				localRay.Transform(go->transform->worldMatrix.Inverted());
				Triangle triangle;
				//for (int j = 0; j < go->model->meshes.size(); j++) {
					for (int i = 0; i < go->model->indices.size()-2; i++) {
						triangle.a = go->model->vertices[go->model->indices[i]].Position;
						triangle.b = go->model->vertices[go->model->indices[i+1]].Position;
						triangle.c = go->model->vertices[go->model->indices[i+2]].Position;
						
						float distance;
						bool hit = triangle.Intersects(localRay, &distance);
						LOG("HIT GAME OBJECT: %s \n", hit ? "true" : "false");
						if (hit) {
							if (distance < minDistance) {
								aux = go;
								minDistance = distance;
							}
						}
					}
				//}
			}
		}
		App->scene->selected = aux;
	}	

}

LineSegment ModuleRender::CreatingRay(float2 mouse) {
	
	ImVec2 pos = App->gui->GetScenePos();
	float sceneHeight = App->gui->GetSceneHeight();
	float sceneWidth = App->gui->GetSceneWidth();
	float normalized_x = -1 + 2 * ((mouse.x - pos.x) / (sceneWidth)); //-(1.0f - (float(mouse.x) * 2.0f) / sceneWidth);
	float normalized_y = -1 + 2 * ((mouse.y - pos.y) / (sceneHeight)); //1.0f - (float(mouse.y) * 2.0f) / sceneHeight;
	float2 normalizedPos = float2((mouse.x - (pos.x + (sceneWidth / 2))) / (sceneWidth / 2), ((pos.y + (sceneHeight / 2)) - mouse.y) / (sceneWidth / 2));
	LineSegment picking = App->scene->camera->frustum.UnProjectLineSegment(normalizedPos.x, normalizedPos.y);
	
	return picking;

}
 void ModuleRender::GetAABBHits(LineSegment ray, std::map<float, GameObject*>& gos) {
	 for (auto gameObject : App->scene->root->children) {
		 if (gameObject->model != nullptr) {
			 bool hit = ray.Intersects(gameObject->model->modelBox);
			 LOG("HIT AABB: %s \n", hit ? "true" : "false");
			 if (hit) {
				 float distance = App->scene->camera->frustum.pos.Distance(gameObject->model->modelBox);
				 gos[distance] = gameObject;
			 }
		 }
	 }
}

void ModuleRender::DrawGuizmo() {

	ImVec2 pos = App->gui->GetScenePos();
	float sceneHeight = App->gui->GetSceneHeight();
	float sceneWidth = App->gui->GetSceneWidth();
	ImGuizmo::SetRect((float)pos.x, (float)pos.y, sceneWidth, sceneHeight);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::Enable(true);


	if (App->scene->selected != nullptr) {

		float4x4 newModel = App->scene->selected->transform->worldMatrix.Transposed();
		float4x4 newView = App->scene->camera->view.Transposed();
		float4x4 newProj = App->scene->camera->proj.Transposed();
		ImGuizmo::Manipulate(newView.ptr(), newProj.ptr(), guizmoOP, guizmoMode, newModel.ptr());

		if (ImGuizmo::IsUsing()) {
			App->scene->selected->transform->SetNewMatrix(newModel.Transposed());
		}
	}

}