#ifndef SOFTWARERENDERER_MATRIX4X4_H
#define SOFTWARERENDERER_MATRIX4X4_H

class Matrix4x4 {
public:
	union {
		struct {
			FLOAT _00, _01, _02, _03,
				  _10, _11, _12, _13,
				  _20, _21, _22, _23,
				  _30, _31, _32, _33;
		};
		struct {
			FLOAT matrix[4][4];
		};
	};

	const Matrix4x4 operator +(const Matrix4x4 &m);
	const Matrix4x4 operator -(const Matrix4x4 &m);
	const Matrix4x4 operator *(const Matrix4x4 &m);
	Matrix4x4 &operator +=(const Matrix4x4 &m);
	Matrix4x4 &operator -=(const Matrix4x4 &m);
	const Matrix4x4 &operator *=(const Matrix4x4 &m);

	static const VOID Set(Matrix4x4 &m, FLOAT _00, FLOAT _01, FLOAT _02, FLOAT _03, FLOAT _10, FLOAT _11, FLOAT _12, FLOAT _13, FLOAT _20, FLOAT _21, FLOAT _22, FLOAT _23, FLOAT _30, FLOAT _31, FLOAT _32, FLOAT _33);
	static const VOID Copy(Matrix4x4 &m1, Matrix4x4 &m2);
	static const VOID Identity(Matrix4x4 &m);
	static const VOID Zero(Matrix4x4 &m);
	static const VOID Invert(Matrix4x4 &m1, Matrix4x4 &m2);
	static const VOID Scale(Matrix4x4 &m, FLOAT x, FLOAT y, FLOAT z);
	static const VOID Translate(Matrix4x4 &m, FLOAT x, FLOAT y, FLOAT z);
	static const VOID Rotate(Matrix4x4 &m, FLOAT x, FLOAT y, FLOAT z);
	static const VOID RotateX(Matrix4x4 &m, FLOAT num);
	static const VOID RotateY(Matrix4x4 &m, FLOAT num);
	static const VOID RotateZ(Matrix4x4 &m, FLOAT num);
};

#endif