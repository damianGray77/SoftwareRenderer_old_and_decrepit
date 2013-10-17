#ifndef SOFTWARERENDERER_VERTEX3_H
#define SOFTWARERENDERER_VERTEX3_H

class Vertex3 {
public:
	FLOAT x, y, z;
	
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
};

#endif