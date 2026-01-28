#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include "general.h"
#include "math.h"

struct Vector2 {
	f32 x = 0;
	f32 y = 0;

	Vector2(f32 x, f32 y) : x(x), y(y) { }
	Vector2(f32 f) : Vector2(f, f) { }
	Vector2() = default;

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

	Vector2& operator +=(Vector2 v) { x += v.x; y += v.y; return *this; }
	Vector2& operator -=(Vector2 v) { x -= v.x; y -= v.y; return *this; }
	Vector2& operator *=(Vector2 v) { x *= v.x; y *= v.y; return *this; }
	Vector2& operator /=(Vector2 v) { x /= v.x; y /= v.y; return *this; }

	Vector2& operator +=(f32 f) { x += f; y += f; return *this; }
	Vector2& operator -=(f32 f) { x -= f; y -= f; return *this; }
	Vector2& operator *=(f32 f) { x *= f; y *= f; return *this; }
	Vector2& operator /=(f32 f) { x /= f; y /= f; return *this; }

	Vector2 operator -() { return Vector2(-x, -y); }

	Vector2 Normal() { return *this / Vector2(Length()); }
	// Vector2 Cross(Vector2 v) { return Normal(); }
};

struct Vector3 {
	f32 x = 0;
	f32 y = 0;
	f32 z = 0;

	Vector3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) { }
	Vector3(Vector2 v, f32 z) : Vector3(v.x, v.y, z) { }
	Vector3(f32 f) : Vector3(f, f, f) { }
	Vector3() = default;

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

	Vector3& operator +=(Vector3 v) { x += v.x; y += v.y; z += v.z; return *this; }
	Vector3& operator -=(Vector3 v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	Vector3& operator *=(Vector3 v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
	Vector3& operator /=(Vector3 v) { x /= v.x; y /= v.y; z /= v.z; return *this; }

	Vector3& operator +=(f32 f) { x += f; y += f; z += f; return *this; }
	Vector3& operator -=(f32 f) { x -= f; y -= f; z -= f; return *this; }
	Vector3& operator *=(f32 f) { x *= f; y *= f; z *= f; return *this; }
	Vector3& operator /=(f32 f) { x /= f; y /= f; z /= f; return *this; }

	Vector3 operator -() { return Vector3(-x, -y, -z); }
};

struct Vector4 {
	f32 x = 0;
	f32 y = 0;
	f32 z = 0;
	f32 w = 0;

	Vector4(f32 x, f32 y, f32 z, f32 w) : x(x), y(y), z(z), w(w) { }
	Vector4(Vector3 v3, f32 w) : x(v3.x), y(v3.y), z(v3.z), w(w) { }
	Vector4() = default;

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

	Vector4& operator +=(Vector4 v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	Vector4& operator -=(Vector4 v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
	Vector4& operator *=(Vector4 v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
	Vector4& operator /=(Vector4 v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }

	Vector4& operator +=(f32 f) { x += f; y += f; z += f; w += f; return *this; }
	Vector4& operator -=(f32 f) { x -= f; y -= f; z -= f; w -= f; return *this; }
	Vector4& operator *=(f32 f) { x *= f; y *= f; z *= f; w *= f; return *this; }
	Vector4& operator /=(f32 f) { x /= f; y /= f; z /= f; w /= f; return *this; }

	Vector4 operator -() { return Vector4(-x, -y, -z, -w); }
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
