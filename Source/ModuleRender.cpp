#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleShader.h"
#include "ModuleCamera.h"
#include "ModuleModelLoader.h"
#include "debugdraw.h"
#include "ModuleDebugDraw.h"
#include "Skybox.h"

#include "SDL.h"
#include "MathGeoLib.h"
#include "optick/optick.h"
#include "ModuleScene.h"
#include "ComponentCamera.h"

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
	glUniformMatrix4fv(glGetUniformLocation(App->shader->test_program, "model"), 1, GL_TRUE, &App->scene->camera->model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->shader->test_program, "view"), 1, GL_TRUE, &GameCamera->view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->shader->test_program, "proj"), 1, GL_TRUE, &GameCamera->proj[0][0]);
	App->scene->camera->DrawFrustum();
	//dd::axisTriad(App->camera->view.Inverted(),5,8);
	DrawGrid();
//	if(App->scene->camera->ContainsAABOX(App->model->modelBox)!= 0)
		//App->model->Draw();
	//PINTAR AQUI DRAWDEBUG
	glUseProgram(0);
	App->skybox->Draw();
	App->debugdraw->Draw(GameCamera, gameFBO, width, height);
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

	glUniformMatrix4fv(glGetUniformLocation(App->shader->def_program, "model"), 1, GL_TRUE, &App->scene->camera->model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->shader->def_program, "view"), 1, GL_TRUE, &App->scene->camera->view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->shader->def_program, "proj"), 1, GL_TRUE, &App->scene->camera->proj[0][0]);
	DrawGrid();
	App->model->Draw();
	glUseProgram(0);
	App->skybox->Draw();
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
	//
	for (int i = 0; i < App->model->models.size(); ++i) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		App->model->models[i]->RenderAABB();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glLineWidth(1.0F);
	float d = 200.0F;
	glColor4f(1.F, 1.F, 1.F, 1.F);
	glBegin(GL_LINES);
	for (float i = -d; i <= d; i += 1.0F) {
		glVertex3f(i, 0.0F, -d);
		glVertex3f(i, 0.0F, d);
		glVertex3f(-d, 0.0F, i);
		glVertex3f(d, 0.0F, i);
	}
	glEnd();

	glLineWidth(2.0F);
	glBegin(GL_LINES);

	// Red X
	glColor4f(1.0F, 0.0F, 0.0F, 1.0F);
	glVertex3f(0.0F, 0.0F, 0.0F);
	glVertex3f(1.0F, 0.0F, 0.0F);
	glVertex3f(1.0F, 0.1F, 0.0F);
	glVertex3f(1.1F, -0.1F, 0.0F);
	glVertex3f(1.1F, 0.1F, 0.0F);
	glVertex3f(1.0F, -0.1F, 0.0F);

	// Green Y
	glColor4f(0.0F, 1.0F, 0.0F, 1.0F);
	glVertex3f(0.0F, 0.0F, 0.0F);
	glVertex3f(0.0F, 1.0F, 0.0F);
	glVertex3f(-0.05F, 1.25F, 0.0F);
	glVertex3f(0.0F, 1.15F, 0.0F);
	glVertex3f(0.05F, 1.25F, 0.0F);
	glVertex3f(0.0F, 1.15F, 0.0F);
	glVertex3f(0.0F, 1.15F, 0.0F);
	glVertex3f(0.0F, 1.05F, 0.0F);

	// Blue Z
	glColor4f(0.0F, 0.0F, 1.0F, 1.0F);
	glVertex3f(0.0F, 0.0F, 0.0F);
	glVertex3f(0.0F, 0.0F, 1.0F);
	glVertex3f(-0.05F, 0.1F, 1.05F);
	glVertex3f(0.05F, 0.1F, 1.05F);
	glVertex3f(0.05F, 0.1F, 1.05F);
	glVertex3f(-0.05F, -0.1F, 1.05F);
	glVertex3f(-0.05F, -0.1F, 1.05F);
	glVertex3f(0.05F, -0.1F, 1.05F);

	glEnd();
	glLineWidth(1.0F);

	glUseProgram(App->shader->def_program);

}
void ModuleRender::SetAxis() {
	glLineWidth(2.0F);
	glBegin(GL_LINES);

	// Red X
	glColor4f(1.0F, 0.0F, 0.0F, 1.0F);
	glVertex3f(0.0F, 0.0F, 0.0F);
	glVertex3f(1.0F, 0.0F, 0.0F);
	glVertex3f(1.0F, 0.1F, 0.0F);
	glVertex3f(1.1F, -0.1F, 0.0F);
	glVertex3f(1.1F, 0.1F, 0.0F);
	glVertex3f(1.0F, -0.1F, 0.0F);

	// Green Y
	glColor4f(0.0F, 1.0F, 0.0F, 1.0F);
	glVertex3f(0.0F, 0.0F, 0.0F);
	glVertex3f(0.0F, 1.0F, 0.0F);
	glVertex3f(-0.05F, 1.25F, 0.0F);
	glVertex3f(0.0F, 1.15F, 0.0F);
	glVertex3f(0.05F, 1.25F, 0.0F);
	glVertex3f(0.0F, 1.15F, 0.0F);
	glVertex3f(0.0F, 1.15F, 0.0F);
	glVertex3f(0.0F, 1.05F, 0.0F);

	// Blue Z
	glColor4f(0.0F, 0.0F, 1.0F, 1.0F);
	glVertex3f(0.0F, 0.0F, 0.0F);
	glVertex3f(0.0F, 0.0F, 1.0F);
	glVertex3f(-0.05F, 0.1F, 1.05F);
	glVertex3f(0.05F, 0.1F, 1.05F);
	glVertex3f(0.05F, 0.1F, 1.05F);
	glVertex3f(-0.05F, -0.1F, 1.05F);
	glVertex3f(-0.05F, -0.1F, 1.05F);
	glVertex3f(0.05F, -0.1F, 1.05F);

	glEnd();
	glLineWidth(1.0F);
}

void ModuleRender::SetWireframe(const bool wireframe) {
	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
}