#ifndef QUATERNION_H
#define QUATERNION_H

#include "matrix.h"

struct Quaternion {
	f32 i;
	f32 j;
	f32 k;
	f32 r;

	explicit Quaternion(f32 r, f32 i, f32 j, f32 k) : r(r), i(i), j(j), k(k) { }
	explicit Quaternion() : Quaternion(1, 0, 0, 0) { }

	static Quaternion CreateRotation(Vector3 axis, f32 angle) {
		f32 h = angle * 0.5f;
		Vector3 n = axis.Normal();
		return Quaternion(
			Cos(h),
			Sin(h) * n.x,
			Sin(h) * n.y,
			Sin(h) * n.z
		);
	}

	f32 Dot(Quaternion q) { return r*q.r + i*q.i + j*q.j + k*q.k; }
	f32 Length() { return Sqrt(r*r + i*i + j*j + k*k); }
	Quaternion Normal() { f32 len = Length(); return Quaternion(r/len, i/len, j/len, k/len); }
	Quaternion Conjugate() { return Quaternion(r, -i, -j, -k); }
	Quaternion Inverse() { f32 len2 = r*r + i*i + j*j + k*k; return Quaternion(r/len2, -i/len2, -j/len2, -k/len2); }

	Quaternion operator +(Quaternion q) { return Quaternion(r + q.r, i + q.i, j + q.j, k + q.k); }
	Quaternion operator -(Quaternion q) { return Quaternion(r - q.r, i - q.i, j - q.j, k - q.k); }
	Quaternion operator -() { return Quaternion(-r, -i, -j, -k); }

	Quaternion operator *(f32 s) { return Quaternion(r * s, i * s, j * s, k * s); }
	Quaternion operator /(f32 s) { return Quaternion(r / s, i / s, j / s, k / s); }

	Quaternion operator *(Quaternion q) {
		return Quaternion(
			r*q.r - i*q.i - j*q.j - k*q.k,
			r*q.i + i*q.r + j*q.k - k*q.j,
			r*q.j - i*q.k + j*q.r + k*q.i,
			r*q.k + i*q.j - j*q.i + k*q.r
		);
	}

	Vector3 Rotate(Vector3 v) {
		Quaternion result = *this * Quaternion(0, v.x, v.y, v.z) * Conjugate();
		return { result.i, result.j, result.k };
	}

	Vector3 operator *(Vector3 v) { return Rotate(v); }
	Vector4 operator *(Vector4 v) { Vector3 rotated = Rotate(Vector3(v.x, v.y, v.z)); return Vector4(rotated.x, rotated.y, rotated.z, v.w); }

	Matrix4 ToMatrix() {
		f32 ii = i * i, jj = j * j, kk = k * k;
		f32 ij = i * j, ik = i * k, jk = j * k;
		f32 ri = r * i, rj = r * j, rk = r * k;
		return Matrix4(
			1 - 2*(jj + kk),     2*(ij + rk),     2*(ik - rj), 0,
			    2*(ij - rk), 1 - 2*(ii + kk),     2*(jk + ri), 0,
			    2*(ik + rj),     2*(jk - ri), 1 - 2*(ii + jj), 0,
			              0,               0,               0, 1
		);
	}
};

static f32 Dot(Quaternion a, Quaternion b) { return a.Dot(b); }

static Quaternion Slerp(Quaternion a, Quaternion b, f32 t) {
	f32 dot = a.Dot(b);
	if (dot < 0) {
		b = -b;
		dot = -dot;
	}

	if (dot > 0.9995f)
		return (a + (b - a) * t).Normal();

	f32 theta = ACos(dot);
	f32 sin_theta = Sin(theta);
	f32 wa = Sin((1 - t) * theta) / sin_theta;
	f32 wb = Sin(t * theta) / sin_theta;
	return a * wa + b * wb;
}

#endif // QUATERNION_H
