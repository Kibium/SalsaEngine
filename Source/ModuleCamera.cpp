#include "ModuleCamera.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleTime.h"

#include <glew.h>
#include "SDL.h"

ModuleCamera::ModuleCamera() {
}

ModuleCamera::~ModuleCamera() {
}

bool ModuleCamera::Init() {
	LOG("Initializing Camera\n");
	int width, height;
	SDL_GetWindowSize(App->window->window, &width, &height);
	aspectRatio = width / height;
	lastX = 0;
	lastY = 0;
	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.pos = float3::unitX;
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 1.f;
	frustum.farPlaneDistance = 100.0f;
	frustum.verticalFov = math::pi / 4.0f;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspectRatio);
	
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

void ModuleCamera::SetAspectRatio(float aspectRatio) {
	this->aspectRatio = aspectRatio;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * this->aspectRatio);
	CalculateMatrixes();
}


void ModuleCamera::SetPos(math::float3 pos) {
	frustum.Translate(pos);
	CalculateMatrixes();
}


void ModuleCamera::CalculateMatrixes() {
	model = math::float4x4::FromTRS(frustum.pos, math::float3x3::RotateY(math::pi / 4.0f), math::float3(1.0f, 1.0f, 1.0f));
	view = LookAt(math::float3(0.0f, 1.f, 4.0f), math::float3(0.0f, 0.0f, 0.0f), math::float3(0.0f, 1.0f, 0.0f));
	proj = frustum.ProjectionMatrix();
}

math::float4x4 ModuleCamera::LookAt(math::float3 eye, math::float3 target, math::float3 up) {
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

void ModuleCamera::MoveUp()
{
	const float distance = App->time->realTimeDT * cameraSpeed;
	float3 new_pos = frustum.pos;
	new_pos.y = new_pos.y + distance;
	frustum.pos = new_pos;
	CalculateMatrixes();
}
void ModuleCamera::MoveDown()
{
	const float distance = App->time->realTimeDT * cameraSpeed;
	float3 new_pos = frustum.pos;
	new_pos.y = new_pos.y - distance;
	frustum.pos = new_pos;
	CalculateMatrixes();
}
void ModuleCamera::MoveFoward()
{
	const float distance = App->time->realTimeDT * cameraSpeed;
	frustum.pos -= frustum.front.ScaledToLength(distance);

	CalculateMatrixes();
}

void ModuleCamera::MoveBackward()
{
	const float distance = App->time->realTimeDT * cameraSpeed;
	frustum.pos += frustum.front.ScaledToLength(distance);

	CalculateMatrixes();
}

void ModuleCamera::MoveLeft()
{
	const float distance = App->time->realTimeDT * cameraSpeed;
	frustum.pos += frustum.WorldRight().ScaledToLength(distance);

	CalculateMatrixes();
}

void ModuleCamera::MoveRight()
{
	const float distance = App->time->realTimeDT * cameraSpeed;
	frustum.pos -= frustum.WorldRight().ScaledToLength(distance);

	CalculateMatrixes();
}

void ModuleCamera::Rotate(float xpos, float ypos) {

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	/*float3x3 rotationMatrix = float3x3::RotateY(pitch);
	frustum.up = frustum.up * rotationMatrix;
	frustum.front = frustum.front * rotationMatrix;
	CalculateMatrixes();
	rotationMatrix = float3x3::RotateX(yaw);
	frustum.up = frustum.up * rotationMatrix;
	frustum.front = frustum.front* rotationMatrix;
	CalculateMatrixes();*/
	/*frustum.front = float3x3::RotateY(yaw).Transform(frustum.front).Normalized();
	frustum.up = float3x3::RotateY(yaw).Transform(frustum.up).Normalized();
	view = LookAt(frustum.pos, frustum.pos + frustum.front, frustum.up);

	frustum.front = float3x3::RotateAxisAngle(frustum.WorldRight(), pitch).Transform(frustum.front).Normalized();
	frustum.up = float3x3::RotateAxisAngle(frustum.WorldRight(), pitch).Transform(frustum.up).Normalized();
	view = LookAt(frustum.pos, frustum.pos + frustum.front, frustum.up);*/

/*	float3 front;
	front.x = cos(((double)yaw*3.14) / 180) *cos(((double)pitch*3.14) / 180);
	front.y = sin(((double)pitch*3.14) / 180);
	front.z = sin(((double)yaw*3.14) / 180) *cos(((double)pitch*3.14) / 180);

    front.Normalize();

	frustum.front.x = front.x;
	frustum.front.y = front.y;
	frustum.front.z = front.z;
	
	CalculateMatrixes();*/
	
}
void ModuleCamera::OrbitX(const float angle)
{
	const float distance = App->time->realTimeDT * cameraSpeed * angle;
	float3x3 rotateMatrix = float3x3::RotateY(distance);
	frustum.pos = frustum.pos * rotateMatrix;
	float3 target = (float3::zero - frustum.pos).Normalized();
	rotateMatrix = float3x3::LookAt(frustum.front, target, frustum.up, float3::unitY);
	frustum.front = frustum.front * rotateMatrix;
	frustum.up = frustum.up * rotateMatrix;

	CalculateMatrixes();
}
void ModuleCamera::OrbitY(const float angle)
{
	const float distance = App->time->realTimeDT * cameraSpeed * angle;
	
	CalculateMatrixes();
}