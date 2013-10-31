#ifndef SOFTWARERENDERER_MATRIX3X3_H
#define SOFTWARERENDERER_MATRIX3X3_H

#include "Matrix4x4.h"

class Matrix3x3 {
public:
	union {
		struct {
			FLOAT _00, _01, _02,
				  _10, _11, _12,
				  _20, _21, _22;
		};
		struct {
			FLOAT matrix[3][3];
		};
	};

	static const VOID Set(Matrix3x3 &m, FLOAT _00, FLOAT _01, FLOAT _02, FLOAT _10, FLOAT _11, FLOAT _12, FLOAT _20, FLOAT _21, FLOAT _22);
	static const VOID Copy(Matrix3x3 &m1, Matrix4x4 &m2);
};

#endif