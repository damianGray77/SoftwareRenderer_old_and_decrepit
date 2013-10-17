#include "stdafx.h"
#include "Vertex.h"

const Vertex3 Vertex3::operator +(const Vertex3 &v) {
	Vertex3 res = {
		x + v.x
		, y + v.y
		, z + v.z
	};

	return res;
}

const Vertex3 Vertex3::operator -(const Vertex3 &v) {
	Vertex3 res = {
		x - v.x
		, y - v.y
		, z - v.z
	};

	return res;
}

const Vertex3 Vertex3::operator *(const Vertex3 &v) {
	Vertex3 res = {
		x * v.x
		, y * v.y
		, z * v.z
	};

	return res;
}

const Vertex3 Vertex3::operator /(const Vertex3 &v) {
	Vertex3 res = {
		x / v.x
		, y / v.y
		, z / v.z
	};

	return res;
}

const Vertex3 Vertex3::operator *(const FLOAT num) {
	Vertex3 res = {
		x * num
		, y * num
		, z * num
	};

	return res;
}

const Vertex3 Vertex3::operator /(const FLOAT num) {
	Vertex3 res = {
		x / num
		, y / num
		, z / num
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

const VOID PVertex3c::Copy(PVertex3c &v1, const PVertex3c &v2) {
	PVertex3c res = {
		{
			v2.position.x
			, v2.position.y
			, v2.position.z
		}, {
			v2.posTrans.x
			, v2.posTrans.y
			, v2.posTrans.z
		}, {
			v2.posProj.x
			, v2.posProj.y
			, v2.posProj.z
		}, {
			v2.uvMap.x
			, v2.uvMap.y
		}, {
			v2.normal.x
			, v2.normal.y
			, v2.normal.z
		}
		, {
			v2.color.r
			, v2.color.g
			, v2.color.b
			, v2.color.a
		}
	};
	v1 = res;
}

const VOID PVertex3c::Copy(PVertex3c &v1, const Vertex3c &v2) {
	PVertex3c res = {
		{ v2.x, v2.y, v2.z, }
		, { 0.0f, 0.0f, 0.0f }
		, { 0.0f, 0.0f, 0.0f }
		, { v2.u, v2.v }
		, { 0.0f, 0.0f, 0.0f }
		, { v2.color.r, v2.color.g, v2.color.b, v2.color.a }
	};
	v1 = res;
}