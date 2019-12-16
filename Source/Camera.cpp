#include "Globals.h"
#include "Camera.h"


Camera::Camera() : front(.0f, .0f, -1.0f), right(1.0f, .0f, .0f), up(.0f, 1.0f, .0), position(0.0f, 0.0f, 0.0f)
{
	right = front.Cross(up);
	up = right.Cross(front);
}


void Camera::LookAt(const math::float3& eye, const math::float3& at, const math::float3& _up)
{
	front = (at-eye).Normalized(); 
	up    = (_up).Normalized(); 
	right = front.Cross(up).Normalized();
	up    = right.Cross(front).Normalized();

	position = eye;
}

void Camera::SetTransform(const math::float3& f, const math::float3& r, const math::float3& u, const math::float3& pos)
{
	front    = f;
	right    = r;
	up       = u;
	position = position;
}

void Camera::SetPosition(const math::float3& pos)
{
	position = pos;
}

void Camera::SetRotation(const math::Quat& quat)
{
	front = quat.Transform(math::float3(0.0f, 0.0f, -1.0f));
	right = quat.Transform(math::float3(1.0f, 0.0f, 0.0f));
	up    = quat.Transform(math::float3(0.0f, 1.0f, 0.0f));
}

void Camera::Advance(float amount)
{
	position += front*amount;
}

void Camera::Strafe(float amount)
{
	position += right*amount;
}

void Camera::Lift(float amount)
{
	position += up*amount;
}

void Camera::RotateYaw(float angle)
{
    math::Quat rotation(up, angle);

	front = rotation.Transform(front);
	right = rotation.Transform(right);
	up    = rotation.Transform(up);
}

void Camera::RotatePitch(float angle)
{
    math::Quat rotation(right, angle);

	front = rotation.Transform(front);
	right = rotation.Transform(right);
	up    = rotation.Transform(up);
}

void Camera::RotateRoll(float angle)
{
    math::Quat rotation(front, angle);

	front = rotation.Transform(front);
	right = rotation.Transform(right);
	up    = rotation.Transform(up);
}

math::float4x4 Camera::GetTransform() const
{
    math::float4x4 out;

    out.SetCol(0, math::float4(right, 0.0f));
    out.SetCol(1, math::float4(up, 0.0f));
    out.SetCol(2, math::float4(-front, 0.0f));
    out.SetCol(3, math::float4(position, 1.0f));

    return out;
}

math::float4x4 Camera::GetViewMatrix() const
{
    math::float4x4 out = GetTransform();
    out.InverseOrthonormal();

    return out;
}

void Camera::SetPerspective(float fovY, float aspect, float _near, float _far)
{
	float fW, fH;

	fH = tan(fovY*0.5f) * _near;
	fW = fH * aspect;

	SetFrustum(-fW, fW, -fH, fH, _near, _far);
}

void Camera::SetOrtho(float left, float right, float bottom, float top, float _near, float _far)
{
	float a = 2.0f / (right - left);
	float b = 2.0f / (top - bottom);
	float c = -2.0f / (_far - _near);

	float tx = -(right + left) / (right - left);
	float ty = - (top + bottom)/(top - bottom);
	float tz = - (_far + _near)/(_far - _near);

    projection.SetCol(0, math::float4(a, 0.0f, 0.0f, 0.0f));
    projection.SetCol(1, math::float4(0.0f , b, 0.0f, 0.0f));
    projection.SetCol(2, math::float4(0.0f , 0.0f, c, 0.0f));
    projection.SetCol(3, math::float4(tx, ty, tz, 1.0f));
}

void Camera::SetFrustum(float left, float right, float bottom, float top, float _near, float _far)
{
	float A = (right + left) / (right - left);
	float B = (top + bottom) / (top - bottom);
	float C = -(_far + _near) / (_far - _near);
	float D = -2.0f*_far*_near / (_far - _near);

    projection.SetCol(0, math::float4(2.0f*_near/(right-left), 0.0f, 0.0f, 0.0f));
    projection.SetCol(1, math::float4(0.0f, 2.0f*_near/(top-bottom), 0.0f, 0.0f));
    projection.SetCol(2, math::float4(A, B, C, -1.0f));
    projection.SetCol(3, math::float4(0.0f, 0.0f, D, 0.0f));
}

