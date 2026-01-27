#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include "general.h"
#include "math.h"

struct Vector2 {
	f32 x;
	f32 y;

	Vector2(f32 x, f32 y) : x(x), y(y) { }
	Vector2(f32 f) : Vector2(f, f) { }

	f32 Dot(Vector2 v) { return x*v.x + y*v.y; }
	f32 Length()       { return Sqrt(x*x + y*y); }

	Vector2 operator +(Vector2 v) { return Vector2(x + v.x, y + v.y); }
	Vector2 operator -(Vector2 v) { return Vector2(x - v.x, y - v.y); }
	Vector2 operator *(Vector2 v) { return Vector2(x * v.x, y * v.y); }
	Vector2 operator /(Vector2 v) { return Vector2(x / v.x, y / v.y); }

	Vector2 operator +(f32 f) { return Vector2(x + f, y + f); }
	Vector2 operator -(f32 f) { return Vector2(x - f, y - f); }
	Vector2 operator *(f32 f) { return Vector2(x * f, y * f); }
	Vector2 operator /(f32 f) { return Vector2(x / f, y / f); }

	Vector2 Normal() { return *this / Vector2(Length()); }
	// Vector2 Cross(Vector2 v) { return Normal(); }
};

struct Vector3 {
	f32 x;
	f32 y;
	f32 z;

	Vector3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) { }
	Vector3(Vector2 v, f32 z) : Vector3(v.x, v.y, z) { }
	Vector3(f32 f) : Vector3(f, f, f) { }
	Vector3() : Vector3(0, 0, 0) { }

	explicit operator Vector2() { return Vector2(x, y); }

	f32 Dot(Vector3 v) { return x*v.x + y*v.y + z*v.z; }
	f32 Length() { return Sqrt(x*x + y*y + z*z); }
	Vector3 Normal() { return *this / Vector3(Length()); }

	Vector3 operator +(Vector3 v) { return Vector3(x + v.x, y + v.y, z + v.z); }
	Vector3 operator -(Vector3 v) { return Vector3(x - v.x, y - v.y, z - v.z); }
	Vector3 operator *(Vector3 v) { return Vector3(x * v.x, y * v.y, z * v.z); }
	Vector3 operator /(Vector3 v) { return Vector3(x / v.x, y / v.y, z / v.z); }

	Vector3 operator +(f32 f) { return Vector3(x + f, y + f, z + f); }
	Vector3 operator -(f32 f) { return Vector3(x - f, y - f, z - f); }
	Vector3 operator *(f32 f) { return Vector3(x * f, y * f, z * f); }
	Vector3 operator /(f32 f) { return Vector3(x / f, y / f, z / f); }
};

struct Vector4 {
	f32 x;
	f32 y;
	f32 z;
	f32 w;

	Vector4(f32 x, f32 y, f32 z, f32 w)
		: x(x), y(y), z(z), w(w) { }

	explicit operator Vector2() { return Vector2(x, y); }
	explicit operator Vector3() { return Vector3(x, y, z); }

	f32 Dot(Vector4 v) { return x*v.x + y*v.y + z*v.z + w*v.w; }
	f32 Length() { return Sqrt(x*x + y*y + z*z + w*w); }

	Vector4 operator +(Vector4 v) { return Vector4(x + v.x, y + v.y, z + v.z, w + v.w); }
	Vector4 operator -(Vector4 v) { return Vector4(x - v.x, y - v.y, z - v.z, w - v.w); }
	Vector4 operator *(Vector4 v) { return Vector4(x * v.x, y * v.y, z * v.z, w * v.w); }
	Vector4 operator /(Vector4 v) { return Vector4(x / v.x, y / v.y, z / v.z, w / v.w); }

	Vector4 operator +(f32 f) { return Vector4(x + f, y + f, z + f, w + f); }
	Vector4 operator -(f32 f) { return Vector4(x - f, y - f, z - f, w - f); }
	Vector4 operator *(f32 f) { return Vector4(x * f, y * f, z * f, w * f); }
	Vector4 operator /(f32 f) { return Vector4(x / f, y / f, z / f, w / f); }
};

static f32 Dot(Vector2 a, Vector2 b) { return a.Dot(b); }
static f32 Dot(Vector3 a, Vector3 b) { return a.Dot(b); }
static f32 Dot(Vector4 a, Vector4 b) { return a.Dot(b); }

static Vector3 Cross(Vector3 a, Vector3 b) {
	return Vector3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

#endif // VECTOR_H_INCLUDED
