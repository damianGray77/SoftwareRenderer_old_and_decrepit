#include "stdafx.h"

DWORD RGBAb(BYTE r, BYTE g, BYTE b, BYTE a) {
	return ((DWORD)a) << 24 | ((DWORD)r) << 16 | ((DWORD)g) << 8 | ((DWORD)b);
}

DWORD RGBAf(FLOAT r, FLOAT g, FLOAT b, FLOAT a) {
	DWORD dr = (DWORD)(r * 255);
	DWORD dg = (DWORD)(g * 255);
	DWORD db = (DWORD)(b * 255);
	DWORD da = (DWORD)(a * 255);
	return da << 24 | dr << 16 | dg << 8 | db;
}

DWORD MULTIPLYRGBA(DWORD color, FLOAT r, FLOAT g, FLOAT b, FLOAT a) {
	BYTE *c = DW2RGBAB(color);
	
	return RGBAb((BYTE)(c[0] * r), (BYTE)(c[1] * g), (BYTE)(c[2] * b), (BYTE)(c[3] * a));
}

FLOAT sinTbl[SINCOSMAX] = {0};
FLOAT cosTbl[SINCOSMAX] = {0};
FLOAT invSinTbl[SINCOSMAX] = {0};
FLOAT invCosTbl[SINCOSMAX] = {0};

BOOL paused = FALSE;

FLOAT* DW2RGBAF(DWORD color) {
	FLOAT* arr = new FLOAT[4];

	arr[0] = RF(color);
	arr[1] = GF(color);
	arr[2] = BF(color);
	arr[3] = AF(color);
	
	return arr;
};

BYTE* DW2RGBAB(DWORD color) {
	BYTE* arr = new BYTE[4];
	
	arr[0] = RB(color);
	arr[1] = GB(color);
	arr[2] = BB(color);
	arr[3] = AB(color);

	return arr;
};

float inverseSqrt(float number) {
	long i;
	float x2, y;
	const float threehalfs = 1.5f;

	x2 = number * 0.5f;
	y  = number;
	i  = *(long*)&y;							// evil floating point bit level hacking
	i  = 0x5f3759df - (i >> 1);					// what the fuck?
	y  = *(float*)&i;
	y  = y * (threehalfs - (x2 * y * y));		// 1st iteration
	y  = y * (threehalfs - (x2 * y * y));		// 2nd iteration, this can be removed

	return y;
}

windowProperties props = {
	NULL
	, { 0, 0, 0, 0, }
	, 640
	, 480
	, 16
	, TRUE
	, L"GameTutorials"
	, L"www.GameTutorials.com - First OpenGL Program" 
	, 0.0f
};