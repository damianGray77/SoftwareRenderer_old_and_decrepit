#ifndef SOFTWARERENDERER_MATRIX_H
#define SOFTWARERENDERER_MATRIX_H

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

	const Matrix4x4 operator +(const Matrix4x4 &m) {
		Matrix4x4 res;
		for(SHORT i = 0; i < 4; i++) {
			for(SHORT j = 0; j < 4; j++) {
				res.matrix[i][j] = matrix[i][j] + m.matrix[i][j];
			}
		}

		return res;
	}

	const Matrix4x4 operator -(const Matrix4x4 &m) {
		Matrix4x4 res;
		for(SHORT i = 0; i < 4; i++) {
			for(SHORT j = 0; j < 4; j++) {
				res.matrix[i][j] = matrix[i][j] - m.matrix[i][j];
			}
		}

		return res;
	}

	const Matrix4x4 operator *(const Matrix4x4 &m) {
		Matrix4x4 res;
		FLOAT e1, e2, e3, e4;
		for(SHORT i = 0; i < 4; i++) {
			e1 = m.matrix[i][0];
			e2 = m.matrix[i][1];
			e3 = m.matrix[i][2];
			e4 = m.matrix[i][3];

			res.matrix[i][0] = (e1 * _00) + (e2 * _10) + (e3 * _20) + (e4 * _30);
			res.matrix[i][1] = (e1 * _01) + (e2 * _11) + (e3 * _21) + (e4 * _31);
			res.matrix[i][2] = (e1 * _02) + (e2 * _12) + (e3 * _22) + (e4 * _32);
			res.matrix[i][3] = (e1 * _03) + (e2 * _13) + (e3 * _23) + (e4 * _33);
		}

		return res;
	}

	Matrix4x4 &operator +=(const Matrix4x4 &m) {
		for(SHORT i = 0; i < 4; i++) {
			for(SHORT j = 0; j < 4; j++) {
				matrix[i][j] += m.matrix[i][j];
			}
		}

		return *this;
	}

	Matrix4x4 &operator -=(const Matrix4x4 &m) {
		for(SHORT i = 0; i < 4; i++) {
			for(SHORT j = 0; j < 4; j++) {
				matrix[i][j] -= m.matrix[i][j];
			}
		}

		return *this;
	}

	const Matrix4x4 &operator *=(const Matrix4x4 &m) {
		FLOAT e1, e2, e3, e4;
		for(SHORT i = 0; i < 4; i++) {
			e1 = m.matrix[i][0];
			e2 = m.matrix[i][1];
			e3 = m.matrix[i][2];
			e4 = m.matrix[i][3];

			matrix[i][0] = (e1 * _00) + (e2 * _10) + (e3 * _20) + (e4 * _30);
			matrix[i][1] = (e1 * _01) + (e2 * _11) + (e3 * _21) + (e4 * _31);
			matrix[i][2] = (e1 * _02) + (e2 * _12) + (e3 * _22) + (e4 * _32);
			matrix[i][3] = (e1 * _03) + (e2 * _13) + (e3 * _23) + (e4 * _33);
		}

		return *this;
	}

	static const VOID Set(Matrix4x4 &m, FLOAT _00, FLOAT _01, FLOAT _02, FLOAT _03, FLOAT _10, FLOAT _11, FLOAT _12, FLOAT _13, FLOAT _20, FLOAT _21, FLOAT _22, FLOAT _23, FLOAT _30, FLOAT _31, FLOAT _32, FLOAT _33);
	static const VOID Copy(Matrix4x4 &m1, Matrix4x4 &m2);
	static const VOID Identity(Matrix4x4 &m);
	static const VOID Zero(Matrix4x4 &m);
	static const VOID Scale(Matrix4x4 &m, FLOAT x, FLOAT y, FLOAT z);
	static const VOID Translate(Matrix4x4 &m, FLOAT x, FLOAT y, FLOAT z);
	static const VOID Rotate(Matrix4x4 &m, FLOAT x, FLOAT y, FLOAT z);
	static const VOID RotateX(Matrix4x4 &m, FLOAT num);
	static const VOID RotateY(Matrix4x4 &m, FLOAT num);
	static const VOID RotateZ(Matrix4x4 &m, FLOAT num);
};

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