#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"
#include "math.h"
#include "quaternion.h"
#include "matrix.h"

struct Camera {
	Vector3 position;
	Vector3 rotation;
	f32 aspect_ratio;
	f32 fov_radians = 90.0 / 360.0 * Math::TAU;

	Quaternion GetOrientation() {
		return Quaternion::FromEuler(rotation.x, rotation.y, rotation.z);
	}

	Matrix4 GenerateVP(f32 near, f32 far) {
		Matrix4 projection = Matrix4::Perspective(fov_radians, aspect_ratio, near, far);
		Matrix4 view = GetOrientation().Conjugate().ToMatrix() * Matrix4::Translate(-position);
		return projection * view;
	}

	void Translate(Vector3 v) {
		position = position + GetOrientation().Rotate(v);
	}
};

#endif // CAMERA_H
