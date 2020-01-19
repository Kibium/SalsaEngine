#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "MathGeoLib.h"

class Camera
{
    math::float3    front;
    math::float3    right;
    math::float3    up;
    math::float3    position;
    math::float4x4  projection;
public:

	Camera();

	void LookAt(const math::float3& eye, const math::float3& at, const math::float3& up);
	void SetTransform(const math::float3& f, const math::float3& r, const math::float3& u, const math::float3& pos);

	void SetPosition(const math::float3& pos);
	void SetRotation(const math::Quat& quat);

	void Advance(float amount);
	void Strafe(float amount);
	void Lift(float amount);

	void RotateYaw(float angle);
	void RotatePitch(float angle);
	void RotateRoll(float angle);

	const math::float3& GetFront() const { return front; }
	const math::float3& GetRight() const { return right; }
	const math::float3& GetUp() const { return up; }
	const math::float3& GetPosition() const { return position; }

	void SetPerspective(float fovY, float aspect, float near, float far);
	void SetOrtho(float left, float right, float bottom, float top, float near, float far);
	void SetFrustum(float left, float right, float bottom, float top, float near, float far);

    math::float4x4        GetTransform () const;
	math::float4x4        GetViewMatrix() const;
	const math::float4x4& GetProjMatrix() const { return projection; }
};

#endif /* _CAMERA_H_ */

