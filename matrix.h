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
};

struct Matrix3 {
	f32 m[3*3];
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

	Matrix4 operator*(Matrix4 b) {
		return Matrix4(
			Vector4(x.x*b.x.x + y.x*b.x.y + z.x*b.x.z + w.x*b.x.w,
			        x.y*b.x.x + y.y*b.x.y + z.y*b.x.z + w.y*b.x.w,
			        x.z*b.x.x + y.z*b.x.y + z.z*b.x.z + w.z*b.x.w,
			        x.w*b.x.x + y.w*b.x.y + z.w*b.x.z + w.w*b.x.w),
			Vector4(x.x*b.y.x + y.x*b.y.y + z.x*b.y.z + w.x*b.y.w,
			        x.y*b.y.x + y.y*b.y.y + z.y*b.y.z + w.y*b.y.w,
			        x.z*b.y.x + y.z*b.y.y + z.z*b.y.z + w.z*b.y.w,
			        x.w*b.y.x + y.w*b.y.y + z.w*b.y.z + w.w*b.y.w),
			Vector4(x.x*b.z.x + y.x*b.z.y + z.x*b.z.z + w.x*b.z.w,
			        x.y*b.z.x + y.y*b.z.y + z.y*b.z.z + w.y*b.z.w,
			        x.z*b.z.x + y.z*b.z.y + z.z*b.z.z + w.z*b.z.w,
			        x.w*b.z.x + y.w*b.z.y + z.w*b.z.z + w.w*b.z.w),
			Vector4(x.x*b.w.x + y.x*b.w.y + z.x*b.w.z + w.x*b.w.w,
			        x.y*b.w.x + y.y*b.w.y + z.y*b.w.z + w.y*b.w.w,
			        x.z*b.w.x + y.z*b.w.y + z.z*b.w.z + w.z*b.w.w,
			        x.w*b.w.x + y.w*b.w.y + z.w*b.w.z + w.w*b.w.w)
		);
	}

	static Matrix4 Perspective(f32 fov_y, f32 aspect, f32 near, f32 far) {
		f32 f = 1.0f / Tan(fov_y / 2.0f);
		return Matrix4(
			f/aspect, 0,  0,                       0,
			0,       -f,  0,                       0,
			0,        0,  far/(near-far),         -1,
			0,        0,  (near*far)/(near-far),   0
		);
	}

	static Matrix4 LookAt(Vector3 eye, Vector3 target, Vector3 up) {
		Vector3 f = (target - eye).Normal();
		Vector3 r = Cross(f, up).Normal();
		Vector3 u = Cross(r, f);
		return Matrix4(
			 r.x,  u.x, -f.x, 0,
			 r.y,  u.y, -f.y, 0,
			 r.z,  u.z, -f.z, 0,
			-Dot(r,eye), -Dot(u,eye), Dot(f,eye), 1
		);
	}

	static Matrix4 RotateX(f32 a) {
		f32 c = Cos(a);
		f32 s = Sin(a);
		return Matrix4(
			1,  0, 0, 0,
			0,  c, s, 0,
			0, -s, c, 0,
			0,  0, 0, 1
		);
	}

	static Matrix4 RotateY(f32 a) {
		f32 c = Cos(a);
		f32 s = Sin(a);
		return Matrix4(
			 c, 0, -s, 0,
			 0, 1,  0, 0,
			 s, 0,  c, 0,
			 0, 0,  0, 1
		);
	}

	static Matrix4 RotateZ(f32 a) {
		f32 c = Cos(a), s = Sin(a);
		return Matrix4(
			 c, s, 0, 0,
			-s, c, 0, 0,
			 0, 0, 1, 0,
			 0, 0, 0, 1
		);
	}
};

#endif // MATRIX_H
