#ifndef SOFTWARERENDERER_VERTEX_H
#define SOFTWARERENDERER_VERTEX_H

class Vector3;
struct Color4f;

struct Vertex2c {
    FLOAT x, y;
	Color4f color;
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
	Color4f color;
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
	Color4f color;

	static const VOID Copy(PVertex3c &v1, const PVertex3c &v2);
	static const VOID Copy(PVertex3c &v1, const Vertex3c &v2);
};

#endif