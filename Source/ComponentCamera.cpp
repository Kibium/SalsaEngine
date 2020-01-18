#include "ComponentCamera.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleModelLoader.h"
#include "ModuleRender.h"
#include "Geometry/AABB.h"
#include "Math/float4x4.h"
#include "ModuleShader.h"
#include "Geometry/AABB.h"
#include "debugdraw.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "Model.h"
#include "Mesh.h"
#include <glew.h>
#include "SDL.h"
#include "optick/optick.h"
#include "ComponentTransform.h"
#include "Mesh.h"
#include "ModuleMSTimer.h"


ComponentCamera::ComponentCamera()
{
	Init();
}

ComponentCamera::~ComponentCamera()
{
}

ComponentCamera::ComponentCamera(const ComponentCamera & cam)
{
}

void ComponentCamera::OnEditor()
{
}
bool ComponentCamera::Init() {
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
	auxView = view;
	return true;
}


update_status ComponentCamera::Update() {
	OPTICK_CATEGORY("UpdateCamera", Optick::Category::Camera);
	glUniform3f(glGetUniformLocation(App->shader->def_program, "viewPos"), frustum.pos.x, frustum.pos.y, frustum.pos.z);
	//Move the camera deppending on the fps
	cameraSpeed = CAMERA_SPEED / App->globalTimer->dt;

	return UPDATE_CONTINUE;
}

bool ComponentCamera::CompareDistances(GameObject* g1, GameObject* g2) {
	return g1->distanceFromCamera < g2->distanceFromCamera;
}

void ComponentCamera::SortByDistance() {

	GameObject* temp;
	for (int i = 0; i < objectsHit.size(); ++i) {
		if (i < objectsHit.size() - 1) {
			for (int j = i + 1; j < objectsHit.size(); ++j) {
				if (CompareDistances(objectsHit[i], objectsHit[j])) {
					temp = objectsHit[j];
					objectsHit[j] = objectsHit[i];
					objectsHit[i] = temp;
				}
			}
		}

	}


	for (int i = 0; i < objectsHit.size(); ++i) {

		//LOG("Object %d: %f units\n", i, objectsHit[i]->distanceFromCamera);
	}


}

//Seems to work properly
bool ComponentCamera::PickingAABBHit() {
	bool intersec = false;
	objectsHit.clear();

	picking = frustum.UnProjectLineSegment(App->input->mousepos.x, App->input->mousepos.y);
	
	//0. Check for AABB intersections
	for (int i = 0; i < App->scene->root->children.size(); ++i) {
		intersec = false;
		intersec = picking.Intersects(App->scene->root->children[i]->model->modelBox);
		if (intersec) {

			//1. If there is intersecion with AABB, add objects hit by the ray in a vector
			objectsHit.push_back(App->scene->root->children[i]);

			//2.1 Get GO's position
			float3 modelpos = float3(App->scene->root->children[i]->transform->position.x, App->scene->root->children[i]->transform->position.y, App->scene->root->children[i]->transform->position.z);

			//2.2 Calculate distance camera - gameObject
			App->scene->root->children[i]->distanceFromCamera = App->scene->camera->frustum.pos.Distance(modelpos);

			LOG("AABB %d hit!\n", i);
		}
	}

	//2.3 Sort them - or try it
	//SortByDistance();
	return intersec;
}

bool ComponentCamera::PickingTriangleHit() {

	bool intersec = false;

	//For every game object
	for (int i = 0; i < objectsHit.size(); ++i) {

		//3. Convert ray to object space
		picking.Transform(objectsHit[i]->transform->worldMatrix);

		//For every mesh in every game object
		//for (int j = 0; j < objectsHit[i]->model->meshes.size(); ++j) {

			//For every triangle in every mesh
			for (int x = 0; x < objectsHit[i]->model->triangles.size(); ++x) {

				//Check if any triangles were hit for every game object, in order
				intersec = objectsHit[i]->model->triangles[x].Intersects(picking, &distance, &hit_point);

				//If there is a hit, mark it as selected on the hierarchy
				if (intersec) {

					LOG(" + Triangle hit\n");
					App->scene->selected = objectsHit[i];
					return intersec;
					break;
					//Enable gizmos and manipulate it from there

				}
			}
			
		//}
		

	}


	return intersec;

}


bool ComponentCamera::CleanUp() {
	return true;
}

void ComponentCamera::SetFOV(float verticalFOV) {
	frustum.verticalFov = verticalFOV;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspectRatio);
	CalculateMatrixes();
}
void ComponentCamera::SetSpeeding(const bool speed) {
	speeding = speed;
}

void ComponentCamera::SetSpeed(float Speed) {
	cameraSpeed = Speed;
}
void ComponentCamera::SetRotationSpeed(float Speed) {
	rotationSpeed = Speed;

}
void ComponentCamera::SetAspectRatio(float aspectRatio) {
	this->aspectRatio = aspectRatio;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * this->aspectRatio);
	CalculateMatrixes();
}

void ComponentCamera::SetOrbit(const bool isOrbit)
{
	orbit = isOrbit;
}

void ComponentCamera::CalculateMatrixes() {
	proj = frustum.ProjectionMatrix();
	view = frustum.ViewMatrix();
}

math::float4x4 ComponentCamera::LookAt(math::float3 eye, math::float3 target, math::float3 up) const {
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

void ComponentCamera::LookAt(const float3 target)
{
	float3 direction = (target - frustum.pos).Normalized();
	float3x3 rotationMatrix = float3x3::LookAt(frustum.front, direction, frustum.up, float3::unitY);
	frustum.front = rotationMatrix.Transform(frustum.front).Normalized();
	frustum.up = rotationMatrix.Transform(frustum.up).Normalized();
}

void ComponentCamera::MoveUp()
{
	frustum.pos.y += cameraSpeed;
	CalculateMatrixes();
}

void ComponentCamera::MoveDown()
{
	frustum.pos.y -= cameraSpeed;
	CalculateMatrixes();
}

void ComponentCamera::MoveFoward()
{
	frustum.pos += frustum.front.ScaledToLength(cameraSpeed);
	CalculateMatrixes();
}

void ComponentCamera::MoveBackward()
{
	frustum.pos -= frustum.front.ScaledToLength(cameraSpeed);
	CalculateMatrixes();
}

void ComponentCamera::MoveLeft()
{
	frustum.pos -= frustum.WorldRight().ScaledToLength(cameraSpeed);
	CalculateMatrixes();
}

void ComponentCamera::MoveRight()
{
	frustum.pos += frustum.WorldRight().ScaledToLength(cameraSpeed);
	CalculateMatrixes();
}

void ComponentCamera::Rotate(const float xpos, const float ypos)
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

void ComponentCamera::Orbit(const float xpos, float ypos)
{
	if (App->scene->selected != nullptr) {
		if (orbit) {
			float3 center = (App->scene->selected->model->modelBox.maxPoint + App->scene->selected->model->modelBox.minPoint) / 2;

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



}

void ComponentCamera::Focus()
{
	if (App->scene->selected != nullptr) {
		if (App->scene->selected->isCamera) {
			//TODO focus when camera selected
		}
		else {
			float3 size = App->scene->selected->model->modelBox.maxPoint - App->scene->selected->model->modelBox.minPoint;
			float3 center = (App->scene->selected->model->modelBox.maxPoint + App->scene->selected->model->modelBox.minPoint) / 2;

			float3 direction = (center - frustum.pos).Normalized();
			float3x3 rotationMatrix = float3x3::LookAt(frustum.front, direction, frustum.up, float3::unitY);
			frustum.front = rotationMatrix * frustum.front;
			frustum.up = rotationMatrix * frustum.up;

			frustum.farPlaneDistance = 1000 * (size.Length() / 2);
			frustum.pos = center - frustum.front * SIZE_FACTOR * (size.Length() / 2);
			frustum.pos.y = (size.Length() / 4);
			CalculateMatrixes();
		}
	}

}

void ComponentCamera::DrawFrustum()
{
	math::float4x4 inverseMatrix = proj * view;
	math::float4x4 inverted = inverseMatrix.Inverted();
	dd::frustum(inverted, float3(0, 1, 1));

}

in_out_frustum ComponentCamera::ContainsAABOX(const AABB& refBox) {

	float3 vCorner[8];
	int iTotalIn = 0;
	refBox.GetCornerPoints(vCorner); // get the corners of the box into the vCorner array
// test all 8 corners against the 6 sides
// if all points are behind 1 specific plane, we are out
// if we are in with all points, then we are fully in
	Plane m_planes[6];
	frustum.GetPlanes(m_planes);
	for (int p = 0; p < 6; ++p) {
		int iInCount = 8;
		int iPtIn = 1;
		for (int i = 0; i < 8; ++i) {
			// test this point against the planes
			if (m_planes[p].IsOnPositiveSide(vCorner[i])) {
				iPtIn = 0;
				--iInCount;
			}
		}
		// were all the points outside of plane p?
		if (iInCount == 0)
			return(ABB_OUT);
		// check if they were all on the right side of the plane
		iTotalIn += iPtIn;
	}
	// so if iTotalIn is 6, then all are inside the view
	if (iTotalIn == 6)
		return(ABB_IN);
	// we must be partly in then otherwise
	return(INTERSECT);

}
