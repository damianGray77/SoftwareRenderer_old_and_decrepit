#include "stdafx.h"
#include "../Math/Matrix4x4.h"
#include "../Math/Matrix3x3.h"
#include "Vertex3.h"
#include "Vector3.h"

const Vector3 Vector3::operator +(const Vector3 &v) {
	Vector3 res = {
		x + v.x,
		y + v.y,
		z + v.z,
	};

	return res;
}

const Vector3 Vector3::operator -(const Vector3 &v) {
	Vector3 res = {
		x - v.x,
		y - v.y,
		z - v.z,
	};

	return res;
}

const Vector3 Vector3::operator *(const Vector3 &v) {
	Vector3 res = {
		x * v.x,
		y * v.y,
		z * v.z,
	};

	return res;
}

const Vector3 Vector3::operator /(const Vector3 &v) {
	Vector3 res = {
		x / v.x,
		y / v.y,
		z / v.z,
	};

	return res;
}

const Vector3 Vector3::operator *(const FLOAT num) {
	Vector3 res = {
		x * num,
		y * num,
		z * num,
	};

	return res;
}

const Vector3 Vector3::operator /(const FLOAT num) {
	Vector3 res = {
		x / num,
		y / num,
		z / num,
	};

	return res;
}

const Vector3 Vector3::operator *(const Matrix4x4 &m) {
	Vector3 res = { 
		(x * m._00) + (y * m._10) + (z * m._20) + m._30,
		(x * m._01) + (y * m._11) + (z * m._21) + m._31,
		(x * m._02) + (y * m._12) + (z * m._22) + m._32,
	};

	return res;
}

const Vector3 Vector3::operator *(const Matrix3x3 &m) {
	Vector3 res = { 
		(x * m._00) + (y * m._10) + (z * m._20),
		(x * m._01) + (y * m._11) + (z * m._21),
		(x * m._02) + (y * m._12) + (z * m._22),
	};

	return res;
}

Vector3 &Vector3::operator +=(const Vector3 &v) {
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}

Vector3 &Vector3::operator -=(const Vector3 &v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return *this;
}

Vector3 &Vector3::operator *=(const Vector3 &v) {
	x *= v.x;
	y *= v.y;
	z *= v.z;

	return *this;
}

Vector3 &Vector3::operator /=(const Vector3 &v) {
	x /= v.x;
	y /= v.y;
	z /= v.z;

	return *this;
}

Vector3 &Vector3::operator *=(const FLOAT num) {
	x *= num;
	y *= num;
	z *= num;

	return *this;
}

Vector3 &Vector3::operator /=(const FLOAT num) {
	x /= num;
	y /= num;
	z /= num;

	return *this;
}

Vector3 &Vector3::operator *=(const Matrix4x4 &m) {
	FLOAT tX = (x * m._00) + (y * m._10) + (z * m._20) + m._30;
	FLOAT tY = (x * m._01) + (y * m._11) + (z * m._21) + m._31;
	FLOAT tZ = (x * m._02) + (y * m._12) + (z * m._22) + m._32;

	x = tX;
	y = tY;
	z = tZ;

	return *this;
}

Vector3 &Vector3::operator *=(const Matrix3x3 &m) {
	FLOAT tX = (x * m._00) + (y * m._10) + (z * m._20);
	FLOAT tY = (x * m._01) + (y * m._11) + (z * m._21);
	FLOAT tZ = (x * m._02) + (y * m._12) + (z * m._22);

	x = tX;
	y = tY;
	z = tZ;

	return *this;
}

const BOOL Vector3::operator ==(const Vector3 &v){
	return ((x >= v.x - FLT_MARGIN && x <= v.x + FLT_MARGIN) &&
			(y >= v.y - FLT_MARGIN && y <= v.y + FLT_MARGIN) &&
			(z >= v.z - FLT_MARGIN && z <= v.z + FLT_MARGIN));
}

const BOOL Vector3::operator !=(const Vector3 &v) {
	return ((x < v.x - FLT_MARGIN || x > v.x + FLT_MARGIN) ||
			(y < v.y - FLT_MARGIN || y > v.y + FLT_MARGIN) ||
			(z < v.z - FLT_MARGIN || z > v.z + FLT_MARGIN));
}

const FLOAT Vector3::Magnitude(const Vector3 &v) { 
	return (FLOAT)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

const FLOAT Vector3::Dot(const Vector3 &v1, const Vector3 &v2) { 
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

const Vector3 Vector3::Cross(const Vertex3 &v1, const Vertex3 &v2) {
	Vector3 res = {
		(v1.y * v2.z) - (v1.z * v2.y),
		(v1.z * v2.x) - (v1.x * v2.z),
		(v1.x * v2.y) - (v1.y * v2.x),
	};
	return res;
}

const Vector3 Vector3::Cross(const Vector3 &v1, const Vector3 &v2) {
	Vector3 res = {
		(v1.y * v2.z) - (v1.z * v2.y),
		(v1.z * v2.x) - (v1.x * v2.z),
		(v1.x * v2.y) - (v1.y * v2.x),
	};
	return res;
}

const Vector3 Vector3::Normal(Vector3 &v) {
	//return v / Magnitude(v);
	return v * inverseSqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}
