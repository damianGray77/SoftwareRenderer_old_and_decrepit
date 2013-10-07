#include "stdafx.h"

const Vertex3 Vertex3::operator +(const Vertex3 &v) {
	Vertex3 res = {
		x + v.x,
		y + v.y,
		z + v.z,
	};

	return res;
}

const Vertex3 Vertex3::operator -(const Vertex3 &v) {
	Vertex3 res = {
		x - v.x,
		y - v.y,
		z - v.z,
	};

	return res;
}

const Vertex3 Vertex3::operator *(const Vertex3 &v) {
	Vertex3 res = {
		x * v.x,
		y * v.y,
		z * v.z,
	};

	return res;
}

const Vertex3 Vertex3::operator /(const Vertex3 &v) {
	Vertex3 res = {
		x / v.x,
		y / v.y,
		z / v.z,
	};

	return res;
}

const Vertex3 Vertex3::operator *(const FLOAT num) {
	Vertex3 res = {
		x * num,
		y * num,
		z * num,
	};

	return res;
}

const Vertex3 Vertex3::operator /(const FLOAT num) {
	Vertex3 res = {
		x / num,
		y / num,
		z / num,
	};

	return res;
}

const Vertex3 Vertex3::operator *(const Matrix4x4 &m) {
	Vertex3 res = { 
		(x * m._00) + (y * m._10) + (z * m._20) + m._30,
		(x * m._01) + (y * m._11) + (z * m._21) + m._31,
		(x * m._02) + (y * m._12) + (z * m._22) + m._32,
	};

	return res;
}

const Vertex3 Vertex3::operator *(const Matrix3x3 &m) {
	Vertex3 res = { 
		(x * m._00) + (y * m._10) + (z * m._20),
		(x * m._01) + (y * m._11) + (z * m._21),
		(x * m._02) + (y * m._12) + (z * m._22),
	};

	return res;
}

Vertex3 &Vertex3::operator +=(const Vertex3 &v) {
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}

Vertex3 &Vertex3::operator -=(const Vertex3 &v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return *this;
}

Vertex3 &Vertex3::operator *=(const Vertex3 &v) {
	x *= v.x;
	y *= v.y;
	z *= v.z;

	return *this;
}

Vertex3 &Vertex3::operator /=(const Vertex3 &v) {
	x /= v.x;
	y /= v.y;
	z /= v.z;

	return *this;
}

Vertex3 &Vertex3::operator *=(const FLOAT num) {
	x *= num;
	y *= num;
	z *= num;

	return *this;
}

Vertex3 &Vertex3::operator /=(const FLOAT num) {
	x /= num;
	y /= num;
	z /= num;

	return *this;
}

Vertex3 &Vertex3::operator *=(const Matrix4x4 &m) {
	FLOAT tX = (x * m._00) + (y * m._10) + (z * m._20) + m._30;
	FLOAT tY = (x * m._01) + (y * m._11) + (z * m._21) + m._31;
	FLOAT tZ = (x * m._02) + (y * m._12) + (z * m._22) + m._32;

	x = tX;
	y = tY;
	z = tZ;

	return *this;
}

Vertex3 &Vertex3::operator *=(const Matrix3x3 &m) {
	FLOAT tX = (x * m._00) + (y * m._10) + (z * m._20);
	FLOAT tY = (x * m._01) + (y * m._11) + (z * m._21);
	FLOAT tZ = (x * m._02) + (y * m._12) + (z * m._22);

	x = tX;
	y = tY;
	z = tZ;

	return *this;
}

const BOOL Vertex3::operator ==(const Vertex3 &v){
	return
		   (x < v.x ? v.x - x : x - v.x) <= FLT_MARGIN
		&& (y < v.y ? v.y - y : y - v.y) <= FLT_MARGIN
		&& (z < v.z ? v.z - z : z - v.z) <= FLT_MARGIN
	;
}

const BOOL Vertex3::operator !=(const Vertex3 &v) {
	return
		   (x < v.x ? v.x - x : x - v.x) > FLT_MARGIN
		&& (y < v.y ? v.y - y : y - v.y) > FLT_MARGIN
		&& (z < v.z ? v.z - z : z - v.z) > FLT_MARGIN
	;
}

