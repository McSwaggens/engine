#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

struct Matrix2 {
	Vector2 x;
	Vector2 y;

	Matrix2(
		Vector2 x,
		Vector2 y
	) : x(x), y(y) { }

	// static const Matrix2 one = Matrix2(
	// 	Vector2(1, 0),
	// 	Vector2(0, 1)
	// );
};

struct Matrix3 {
	f32 m[3*3];

	// static const Matrix3 one = {
	// 	Vector3(1, 0, 0),
	// 	Vector3(0, 1, 0),
	// 	Vector3(0, 0, 1),
	// };
};

struct Matrix4 {
	Vector4 x;
	Vector4 y;
	Vector4 z;
	Vector4 w;

	explicit Matrix4(
		Vector4 x,
		Vector4 y,
		Vector4 z,
		Vector4 w
	) : x(x), y(y), z(z), w(w) { }

	explicit Matrix4(
		f32 xx, f32 xy, f32 xz, f32 xw,
		f32 yx, f32 yy, f32 yz, f32 yw,
		f32 zx, f32 zy, f32 zz, f32 zw,
		f32 wx, f32 wy, f32 wz, f32 ww
	) : Matrix4(
		Vector4(xx, xy, xz, xw),
		Vector4(yx, yy, yz, yw),
		Vector4(zx, zy, zz, zw),
		Vector4(wx, wy, wz, ww)
	) { }

	static inline Matrix4 One() {
		return Matrix4(
			Vector4(1, 0, 0, 0),
			Vector4(0, 1, 0, 0),
			Vector4(0, 0, 1, 0),
			Vector4(0, 0, 0, 1)
		);
	};

	Vector4 operator*(Vector4 v) {
		return Vector4(
			Dot(v, x),
			Dot(v, y),
			Dot(v, z),
			Dot(v, w)
		);
	}
};

#endif // MATRIX_H
