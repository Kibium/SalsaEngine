#include "ModuleTriangle.h"
#include "ModuleShader.h"
#include "ModuleTexture.h"
#include "Application.h"
#include <il.h>
#include <ilu.h>
#include <ilut.h>
#include "ModuleCamera.h"


ModuleTriangle::ModuleTriangle()
{
}


ModuleTriangle::~ModuleTriangle()
{
}

bool ModuleTriangle::Init() {

	model = float4x4::FromTRS(
		float3(0.0f, 0.0f, 0.0f),
		float3x3::identity,
		float3(1.0f, 1.0f, 1.0f)
	);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f    // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);

	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// 4. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // uv
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, App->texture->Load("Lenna.png"));

	return true;
}

update_status ModuleTriangle::Update()
{
	

	glUseProgram(App->shader->def_program);
	glUniformMatrix4fv(
		glGetUniformLocation(App->shader->def_program, "model"),
		1,
		GL_TRUE,
		&model[0][0]
	);
	glUniformMatrix4fv(
		glGetUniformLocation(App->shader->def_program, "view"),
		1,
		GL_TRUE,
		&App->camera->view[0][0]
	);
	glUniformMatrix4fv(
		glGetUniformLocation(App->shader->def_program, "proj"),
		1,
		GL_TRUE,
		&App->camera->proj[0][0]
	);
	glBindVertexArray(VAO); 
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	return UPDATE_CONTINUE;
}

bool ModuleTriangle::CleanUp()
{
	glBindVertexArray(VAO);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glBindVertexArray(0);

	glDeleteBuffers(1, &VBO);



	return true;
}

float4x4 ModuleTriangle::LookAt(float3 target, float3 eye, float3 up) {

	float4x4 matrix;

	math::float3 f(target - eye); f.Normalize();
	math::float3 s(f.Cross(up)); s.Normalize();
	math::float3 u(s.Cross(f));
	matrix[0][0] = s.x; matrix[0][1] = s.y; matrix[0][2] = s.z;
	matrix[1][0] = u.x; matrix[1][1] = u.y; matrix[1][2] = u.z;
	matrix[2][0] = -f.x; matrix[2][1] = -f.y; matrix[2][2] = -f.z;
	matrix[0][3] = -s.Dot(eye); matrix[1][3] = -u.Dot(eye); matrix[2][3] = f.Dot(eye);
	matrix[3][0] = 0.0f; matrix[3][1] = 0.0f; matrix[3][2] = 0.0f; matrix[3][3] = 1.0f;

	return matrix;
}
