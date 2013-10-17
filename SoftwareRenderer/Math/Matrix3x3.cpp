#include "stdafx.h"
#include "Matrix4x4.h"
#include "Matrix3x3.h"

const VOID Matrix3x3::Set(Matrix3x3 &m, FLOAT _00, FLOAT _01, FLOAT _02, FLOAT _10, FLOAT _11, FLOAT _12, FLOAT _20, FLOAT _21, FLOAT _22) {
	m._00 = _00; m._01 = _01; m._02 = _02;
	m._10 = _10; m._11 = _11; m._12 = _12;
	m._20 = _20; m._21 = _21; m._22 = _22;
}

const VOID Matrix3x3::Copy(Matrix3x3 &m1, Matrix4x4 &m2) { 
	Set(m1, m2._00, m2._01, m2._02, m2._10, m2._11, m2._12, m2._20, m2._21, m2._22);
}