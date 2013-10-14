#include "Matrix.h"

#ifndef SOFTWARERENDERER_VERTEX_H
#define SOFTWARERENDERER_VERTEX_H

#include "Vector.h"
class Vector3;

struct Vertex2 {
    FLOAT x, y;
};

struct Vertex2c {
    FLOAT x, y;
	DWORD color;
};

class Vertex3 {
public:
	const Vertex3 operator +(const Vertex3 &v);
	const Vertex3 operator -(const Vertex3 &v);
	const Vertex3 operator *(const Vertex3 &v);
	const Vertex3 operator /(const Vertex3 &v);
	const Vertex3 operator *(const FLOAT num);
	const Vertex3 operator /(const FLOAT num);
	const Vertex3 operator *(const Matrix4x4 &m);
	const Vertex3 operator *(const Matrix3x3 &m);
	Vertex3 &operator +=(const Vertex3 &v);
	Vertex3 &operator -=(const Vertex3 &v);
	Vertex3 &operator *=(const Vertex3 &v);
	Vertex3 &operator /=(const Vertex3 &v);
	Vertex3 &operator *=(const FLOAT num);
	Vertex3 &operator /=(const FLOAT num);
	Vertex3 &operator *=(const Matrix4x4 &m);
	Vertex3 &operator *=(const Matrix3x3 &m);
	const BOOL operator ==(const Vertex3 &v);
	const BOOL operator !=(const Vertex3 &v);
	
	FLOAT x, y, z;
};

struct Vertex3c {
	FLOAT x, y, z;
    FLOAT u, v;
	DWORD color;
};

struct PVertex3 {
	Vertex3 position;
	Vertex3 posTrans;
	Vector3 normal;
    Vertex2 uvMap;
};

struct PVertex3c {
	Vertex3 position;
	Vertex3 posTrans;
	Vertex3c posProj;
	Vertex2 uvMap;
	Vector3 normal;
	DWORD color;

	static const VOID Copy(PVertex3c &v1, const PVertex3c &v2) {
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
			, v2.color
		};
		v1 = res;
	}
	
	static const VOID Copy(PVertex3c &v1, const Vertex3c &v2) {
		PVertex3c res = {
			{ v2.x, v2.y, v2.z, }
			, { 0.0f, 0.0f, 0.0f }
			, { 0.0f, 0.0f, 0.0f }
			, { v2.u, v2.v }
			, { 0.0f, 0.0f, 0.0f }
			, v2.color
		};
		v1 = res;
	}
};

#endif