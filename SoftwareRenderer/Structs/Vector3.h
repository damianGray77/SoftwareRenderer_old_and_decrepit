#ifndef SOFTWARERENDERER_VECTOR3_H
#define SOFTWARERENDERER_VECTOR3_H

#include "../Math/Matrix4x4.h"
#include "../Math/Matrix3x3.h"
#include "Vertex3.h"

class Vector3 {
public:
	FLOAT x, y, z;
	
	const Vector3 operator +(const Vector3 &v);
	const Vector3 operator -(const Vector3 &v);
	const Vector3 operator *(const Vector3 &v);
	const Vector3 operator /(const Vector3 &v);
	const Vector3 operator *(const FLOAT num);
	const Vector3 operator /(const FLOAT num);
	const Vector3 operator *(const Matrix4x4 &m);
	const Vector3 operator *(const Matrix3x3 &m);
	Vector3 &operator +=(const Vector3 &v);
	Vector3 &operator -=(const Vector3 &v);
	Vector3 &operator *=(const Vector3 &v);
	Vector3 &operator /=(const Vector3 &v);
	Vector3 &operator *=(const FLOAT num);
	Vector3 &operator /=(const FLOAT num);
	Vector3 &operator *=(const Matrix4x4 &m);
	Vector3 &operator *=(const Matrix3x3 &m);
	const BOOL operator ==(const Vector3 &v);
	const BOOL operator !=(const Vector3 &v);
	static const FLOAT Magnitude(const Vector3 &v);
	static const FLOAT Dot(const Vector3 &v1, const Vector3 &v2);
	static const Vector3 Cross(const Vertex3 &v1, const Vertex3 &v2);
	static const Vector3 Cross(const Vector3 &v1, const Vector3 &v2);
	static const Vector3 Normal(Vector3 &v);
};

#endif