#include "ModuleCamera.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleModelLoader.h"
#include "Geometry/AABB.h"

#include <glew.h>
#include "SDL.h"

ModuleCamera::ModuleCamera() {
}

ModuleCamera::~ModuleCamera() {
}

bool ModuleCamera::Init() {
	LOG("Init Camera\n");
	int width, height;
	SDL_GetWindowSize(App->window->window, &width, &height);
	aspectRatio = width / height;

	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = float3::unitX;
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 1000.0f;
	frustum.verticalFov = math::pi / 4.0f;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspectRatio);
	model = math::float4x4::FromTRS(frustum.pos, math::float3x3::RotateY(math::pi / 4.0f), math::float3(1.0f, 1.0f, 1.0f));
	CalculateMatrixes();

	return true;
}

update_status ModuleCamera::PreUpdate() {
	return UPDATE_CONTINUE;
}

update_status ModuleCamera::Update() {
	return UPDATE_CONTINUE;
}

update_status ModuleCamera::PostUpdate() {
	return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp() {
	return true;
}

void ModuleCamera::SetFOV(float verticalFOV) {
	frustum.verticalFov = verticalFOV;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspectRatio);
	CalculateMatrixes();
}
void ModuleCamera::SetSpeeding(const bool speed) {
	speeding = speed;
}

void ModuleCamera::SetSpeed(float Speed) {
	cameraSpeed = Speed;
}
void ModuleCamera::SetRotationSpeed(float Speed) {
	rotationSpeed = Speed;

}
void ModuleCamera::SetAspectRatio(float aspectRatio) {
	this->aspectRatio = aspectRatio;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * this->aspectRatio);
	CalculateMatrixes();
}

void ModuleCamera::SetOrbit(const bool isOrbit)
{
	orbit = isOrbit;
}

void ModuleCamera::CalculateMatrixes() {
	proj = frustum.ProjectionMatrix();
	view = frustum.ViewMatrix();
}

math::float4x4 ModuleCamera::LookAt(math::float3 eye, math::float3 target, math::float3 up) const {
	math::float4x4 matrix;
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

void ModuleCamera::LookAt(const float3 target)
{
	float3 direction = (target - frustum.pos).Normalized();
	float3x3 rotationMatrix = float3x3::LookAt(frustum.front, direction, frustum.up, float3::unitY);
	frustum.front = rotationMatrix.Transform(frustum.front).Normalized();
	frustum.up = rotationMatrix.Transform(frustum.up).Normalized();
}

void ModuleCamera::MoveUp()
{
	frustum.pos.y += cameraSpeed;
	CalculateMatrixes();
}

void ModuleCamera::MoveDown()
{
	frustum.pos.y -= cameraSpeed;
	CalculateMatrixes();
}

void ModuleCamera::MoveFoward()
{
	frustum.pos += frustum.front.ScaledToLength(cameraSpeed);
	CalculateMatrixes();
}

void ModuleCamera::MoveBackward()
{
	frustum.pos -= frustum.front.ScaledToLength(cameraSpeed);
	CalculateMatrixes();
}

void ModuleCamera::MoveLeft()
{
	frustum.pos -= frustum.WorldRight().ScaledToLength(cameraSpeed);
	CalculateMatrixes();
}

void ModuleCamera::MoveRight()
{
	frustum.pos += frustum.WorldRight().ScaledToLength(cameraSpeed);
	CalculateMatrixes();
}

void ModuleCamera::Rotate(const float xpos, const float ypos)
{

	if (xpos != 0.0f)
	{
		float3x3 rotationY = float3x3::RotateY(xpos * rotationSpeed);
		frustum.front = rotationY.Transform(frustum.front).Normalized();
		frustum.up = rotationY.Transform(frustum.up).Normalized();
	}

	if (ypos != 0.0f)
	{
		
		float3x3 rotationX = float3x3::RotateAxisAngle(frustum.WorldRight(), ypos * rotationSpeed);
		frustum.up = rotationX.Transform(frustum.up).Normalized();
		frustum.front = rotationX.Transform(frustum.front).Normalized();
	}
	CalculateMatrixes();
}

void ModuleCamera::Orbit(const float xpos, float ypos)
{

	if (orbit) {
		float3 center = (App->model->modelBox.maxPoint + App->model->modelBox.minPoint) / 2;
		
		if (xpos != 0.0f)
		{
			float3x3 orbitMatrix = float3x3::RotateY(xpos * rotationSpeed);
			frustum.pos = orbitMatrix.Transform(frustum.pos - center) + center;
		}
		if (lastY + ypos < MINIMUM_PITCH) {
			ypos = 0.0f;
			lastY = -89;
		}

		if (lastY + ypos > MAXIMUM_PITCH) {
			ypos = 0.0f;
			lastY = 89;
		}
		if (ypos != 0.0f)
		{
			lastY += ypos;
			float3x3 orbitMatrix = float3x3::RotateAxisAngle(frustum.WorldRight(), ypos * rotationSpeed);
			frustum.pos = orbitMatrix.Transform(frustum.pos - center) + center;
		}

		LookAt(center);
		CalculateMatrixes();
	}

	
}

void ModuleCamera::Focus()
{
	float3 size = App->model->modelBox.maxPoint - App->model->modelBox.minPoint;
	float3 center = (App->model->modelBox.maxPoint + App->model->modelBox.minPoint) / 2;

	float3 direction = (center - frustum.pos).Normalized();
	float3x3 rotationMatrix = float3x3::LookAt(frustum.front, direction, frustum.up, float3::unitY);
	frustum.front = rotationMatrix * frustum.front;
	frustum.up = rotationMatrix * frustum.up;

	frustum.farPlaneDistance = 1000 * (size.Length() / 2);
	frustum.pos = center - frustum.front * SIZE_FACTOR * (size.Length() / 2);
	frustum.pos.y =  (size.Length() / 4);
	CalculateMatrixes();
}
