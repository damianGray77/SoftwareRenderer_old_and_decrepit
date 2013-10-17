#include "stdafx.h"
#include "Matrix4x4.h"

const Matrix4x4 Matrix4x4::operator +(const Matrix4x4 &m) {
	Matrix4x4 res;
	for(SHORT i = 0; i < 4; i++) {
		for(SHORT j = 0; j < 4; j++) {
			res.matrix[i][j] = matrix[i][j] + m.matrix[i][j];
		}
	}

	return res;
}

const Matrix4x4 Matrix4x4::operator -(const Matrix4x4 &m) {
	Matrix4x4 res;
	for(SHORT i = 0; i < 4; i++) {
		for(SHORT j = 0; j < 4; j++) {
			res.matrix[i][j] = matrix[i][j] - m.matrix[i][j];
		}
	}

	return res;
}

const Matrix4x4 Matrix4x4::operator *(const Matrix4x4 &m) {
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

Matrix4x4 &Matrix4x4::operator +=(const Matrix4x4 &m) {
	for(SHORT i = 0; i < 4; i++) {
		for(SHORT j = 0; j < 4; j++) {
			matrix[i][j] += m.matrix[i][j];
		}
	}

	return *this;
}

Matrix4x4 &Matrix4x4::operator -=(const Matrix4x4 &m) {
	for(SHORT i = 0; i < 4; i++) {
		for(SHORT j = 0; j < 4; j++) {
			matrix[i][j] -= m.matrix[i][j];
		}
	}

	return *this;
}

const Matrix4x4 &Matrix4x4::operator *=(const Matrix4x4 &m) {
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

const VOID Matrix4x4::Set(Matrix4x4 &m, FLOAT _00, FLOAT _01, FLOAT _02, FLOAT _03, FLOAT _10, FLOAT _11, FLOAT _12, FLOAT _13, FLOAT _20, FLOAT _21, FLOAT _22, FLOAT _23, FLOAT _30, FLOAT _31, FLOAT _32, FLOAT _33) {
	m._00 = _00; m._01 = _01; m._02 = _02; m._03 = _03;
	m._10 = _10; m._11 = _11; m._12 = _12; m._13 = _13;
	m._20 = _20; m._21 = _21; m._22 = _22; m._23 = _23;
	m._30 = _30; m._31 = _31; m._32 = _32; m._33 = _33;
}

const VOID Matrix4x4::Copy(Matrix4x4 &m1, Matrix4x4 &m2) { 
	Set(m1, m2._00, m2._01, m2._02, m2._03, m2._10, m2._11, m2._12, m2._13, m2._20, m2._21, m2._22, m2._23, m2._30, m2._31, m2._32, m2._33);
}

const VOID Matrix4x4::Identity(Matrix4x4 &m) {
	Set(m, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

const VOID Matrix4x4::Zero(Matrix4x4 &m) { 
	Set(m, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

const VOID Matrix4x4::Scale(Matrix4x4 &m, FLOAT x, FLOAT y, FLOAT z) {
	Matrix4x4 id;
	Identity(id);
	id._00 = x;
	id._11 = y;
	id._22 = z;

	m = id * m;
}

const VOID Matrix4x4::Translate(Matrix4x4 &m, FLOAT x, FLOAT y, FLOAT z) {
	Matrix4x4 id;
	Identity(id);
	id._30 = x;
	id._31 = y;
	id._32 = z;

	m = id * m;
}

const VOID Matrix4x4::Rotate(Matrix4x4 &m, FLOAT x, FLOAT y, FLOAT z) {
	Matrix4x4 rX, rY,rZ;

	Identity(rX);
	Identity(rY);
	Identity(rZ);

	rX._11 = COS(x);
	rX._12 = SIN(x);
	rX._21 = -rX._12;
	rX._22 = rX._11;

	rY._00 = COS(y);
	rY._02 = -SIN(y);
	rY._20 = -rY._02;
	rY._22 = rY._00;

	rZ._00 = COS(z);
	rZ._01 = SIN(z);
	rZ._10 = -rZ._01;
	rZ._11 = rZ._00;

	m = rX * m;
	m = rY * m;
	m = rZ * m;
}

const VOID Matrix4x4::RotateX(Matrix4x4 &m, FLOAT num) {
	Matrix4x4 rX;

	Identity(rX);

	rX._11 = COS(num);
	rX._12 = SIN(num);
	rX._21 = -rX._12;
	rX._22 = rX._11;

	m = rX * m;
}

const VOID Matrix4x4::RotateY(Matrix4x4 &m, FLOAT num) {
	Matrix4x4 rY;

	Identity(rY);

	rY._00 = COS(num);
	rY._02 = -SIN(num);
	rY._20 = -rY._02;
	rY._22 = rY._00;

	m = rY * m;
}

const VOID Matrix4x4::RotateZ(Matrix4x4 &m, FLOAT num) {
	Matrix4x4 rZ;

	Identity(rZ);

	rZ._00 = COS(num);
	rZ._01 = SIN(num);
	rZ._10 = -rZ._01;
	rZ._11 = rZ._00;

	m = rZ * m;
}