#include "stdafx.h"
#include "Structs/Color4f.h"
#include "Structs/Vertex3c.h"
#include "Structs/Vertex2.h"
#include "Structs/Polygon3cuv.h"
#include "Core.h"

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

FLOAT inverseSqrt(FLOAT number) {
	LONG i;
	FLOAT x2, y;
	const FLOAT threehalfs = 1.5f;

	x2 = number * 0.5f;
	y  = number;
	i  = *(LONG *)&y;							// evil floating point bit level hacking
	i  = 0x5f3759df - (i >> 1);					// what the fuck?
	y  = *(FLOAT*)&i;
	y  = y * (threehalfs - (x2 * y * y));		// 1st iteration
	y  = y * (threehalfs - (x2 * y * y));		// 2nd iteration, this can be removed

	return y;
}

FLOAT sinTbl[SINCOSMAX] = {0};
FLOAT cosTbl[SINCOSMAX] = {0};
FLOAT invSinTbl[SINCOSMAX] = {0};
FLOAT invCosTbl[SINCOSMAX] = {0};

INT numCores = 0;
BOOL paused = FALSE;

WindowProperties props = {
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

const Vertex3c verticesc[] = {
	  {  0.0f,  0.0f,  0.0f,  0.0f,  0.0f, { 0.0f,  0.0f, 0.0f, 1.0f } }
	, {  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, { 1.0f,  0.0f, 0.0f, 1.0f } }
	, {  1.0f,  1.0f, -1.0f, 63.0f,  0.0f, { 1.0f,  1.0f, 0.0f, 1.0f } }
	, {  1.0f, -1.0f,  1.0f, 63.0f, 63.0f, { 0.0f,  1.0f, 0.0f, 1.0f } }
	, {  1.0f, -1.0f, -1.0f,  0.0f, 63.0f, { 0.0f,  1.0f, 1.0f, 1.0f } }
	, { -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, { 0.0f,  0.0f, 1.0f, 1.0f } }
	, { -1.0f,  1.0f, -1.0f, 63.0f,  0.0f, { 1.0f,  0.0f, 1.0f, 1.0f } }
	, { -1.0f, -1.0f,  1.0f, 63.0f, 63.0f, { 1.0f,  1.0f, 1.0f, 1.0f } }
	, { -1.0f, -1.0f, -1.0f,  0.0f, 63.0f, { 0.0f,  0.0f, 0.0f, 1.0f } }
};

const Polygon3cuv indices[] = {
	  { 1, 2, 3, 0, {  0.0f, 0.0f }, { 63.0f,  0.0f }, {  0.0f, 63.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }
	, { 2, 4, 3, 0, { 63.0f, 0.0f }, { 63.0f, 63.0f }, {  0.0f, 63.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }
	, { 2, 6, 4, 0, {  0.0f, 0.0f }, { 63.0f,  0.0f }, {  0.0f, 63.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } }
	, { 6, 8, 4, 0, { 63.0f, 0.0f }, { 63.0f, 63.0f }, {  0.0f, 63.0f }, { 1.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } }
	, { 6, 5, 8, 0, {  0.0f, 0.0f }, { 63.0f,  0.0f }, {  0.0f, 63.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }
	, { 5, 7, 8, 0, { 63.0f, 0.0f }, { 63.0f, 63.0f }, {  0.0f, 63.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }
	, { 5, 1, 7, 0, {  0.0f, 0.0f }, { 63.0f,  0.0f }, {  0.0f, 63.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } }
	, { 1, 3, 7, 0, { 63.0f, 0.0f }, { 63.0f, 63.0f }, {  0.0f, 63.0f }, { 0.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } }
	, { 4, 8, 3, 0, {  0.0f, 0.0f }, { 63.0f,  0.0f }, {  0.0f, 63.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
	, { 8, 7, 3, 0, { 63.0f, 0.0f }, { 63.0f, 63.0f }, {  0.0f, 63.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
	, { 2, 1, 6, 0, {  0.0f, 0.0f }, { 63.0f,  0.0f }, {  0.0f, 63.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f, 1.0f } }
	, { 1, 5, 6, 0, { 63.0f, 0.0f }, { 63.0f, 63.0f }, {  0.0f, 63.0f }, { 1.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 1.0f, 1.0f } }
};

/*const Vertex3c verticesc[] = {
	  {  0.0f, 0.00f, 0.00f, 0.0f, 0.0f, 0xff000000 }
	, {  1.0f, 0.00f, 0.00f, 0.0f, 1.0f, 0xff000000 }
	, {  1.0f, 0.50f, 0.50f, 1.0f, 1.0f, 0xffff0000 }
	, {  1.0f, 0.50f,-0.50f, 0.0f, 0.0f, 0xffffff00 }
	, {  1.0f,-0.50f, 0.50f, 0.0f, 1.0f, 0xff00ff00 }
	, {  1.0f,-0.50f,-0.50f, 1.0f, 1.0f, 0xff00ffff }
	, {  1.0f, 0.70f, 0.00f, 0.0f, 0.0f, 0xff0000ff }
	, {  1.0f,-0.70f, 0.00f, 0.0f, 1.0f, 0xffff00ff }
	, {  1.0f, 0.00f, 0.70f, 1.0f, 1.0f, 0xffff0000 }
	, {  1.0f, 0.00f,-0.70f, 0.0f, 0.0f, 0xffffff00 }
	, {  1.0f, 0.25f,-0.65f, 0.0f, 1.0f, 0xff00ffff }
	, {  1.0f,-0.25f,-0.65f, 1.0f, 1.0f, 0xff0000ff }
	, {  1.0f, 0.25f, 0.65f, 0.0f, 0.0f, 0xffff00ff }
	, {  1.0f,-0.25f, 0.65f, 0.0f, 1.0f, 0xffff0000 }
	, {  1.0f,-0.65f, 0.25f, 1.0f, 1.0f, 0xffffff00 }
	, {  1.0f,-0.65f,-0.25f, 0.0f, 0.0f, 0xff00ff00 }
	, {  1.0f, 0.65f, 0.25f, 0.0f, 1.0f, 0xff00ffff }
	, {  1.0f, 0.65f,-0.25f, 1.0f, 1.0f, 0xff0000ff }

	, { -1.0f, 0.00f, 0.00f, 0.0f, 0.0f, 0xffffffff }
	, { -1.0f, 0.50f, 0.50f, 0.0f, 1.0f, 0xffff00ff }
	, { -1.0f, 0.50f,-0.50f, 1.0f, 1.0f, 0xffff0000 }
	, { -1.0f,-0.50f, 0.50f, 0.0f, 0.0f, 0xffffff00 }
	, { -1.0f,-0.50f,-0.50f, 0.0f, 1.0f, 0xff00ff00 }
	, { -1.0f, 0.70f, 0.00f, 1.0f, 1.0f, 0xff00ffff }
	, { -1.0f,-0.70f, 0.00f, 0.0f, 0.0f, 0xff0000ff }
	, { -1.0f, 0.00f, 0.70f, 0.0f, 1.0f, 0xffff00ff }
	, { -1.0f, 0.00f,-0.70f, 1.0f, 1.0f, 0xffff0000 }
	, { -1.0f, 0.25f,-0.65f, 0.0f, 0.0f, 0xffffff00 }
	, { -1.0f,-0.25f,-0.65f, 0.0f, 1.0f, 0xff00ff00 }
	, { -1.0f, 0.25f, 0.65f, 1.0f, 1.0f, 0xff00ffff }
	, { -1.0f,-0.25f, 0.65f, 0.0f, 0.0f, 0xff0000ff }
	, { -1.0f,-0.65f, 0.25f, 0.0f, 1.0f, 0xffff00ff }
	, { -1.0f,-0.65f,-0.25f, 1.0f, 1.0f, 0xffff0000 }
	, { -1.0f, 0.65f, 0.25f, 0.0f, 0.0f, 0xffffff00 }
	, { -1.0f, 0.65f,-0.25f, 0.0f, 1.0f, 0xff00ff00 }
};

const INT indices[] = {
	  9, 1, 10, 10, 1, 3, 3, 1, 17, 17, 1, 6
	, 6, 1, 16, 16, 1, 2, 2, 1, 12, 12, 1, 8
	, 8, 1, 13, 13, 1, 4, 4, 1, 14, 14, 1, 7
	, 7, 1, 15, 15, 1, 5, 5, 1, 11, 11, 1, 9

	, 9, 26, 11, 26, 28, 11, 11, 28, 5, 28, 22, 5
	, 5, 22, 15, 22, 32, 15, 15, 32, 7, 32, 24, 7
	, 7, 24, 14, 24, 31, 14, 14, 31, 4, 31, 21, 4
	, 4, 21, 13, 21, 30, 13, 13, 30, 8, 30, 25, 8
	, 8, 25, 12, 25, 29, 12, 12, 29, 2, 29, 19, 2
	, 2, 19, 16, 19, 33, 16, 16, 33, 6, 33, 23, 6
	, 6, 23, 17, 23, 34, 17, 17, 34, 3, 34, 20, 3
	, 3, 20, 10, 20, 27, 10, 10, 27, 9, 27, 26, 9

	, 18, 26, 27, 18, 27, 20, 18, 20, 34, 18, 34, 23
	, 18, 23, 33, 18, 33, 19, 18, 19, 29, 18, 29, 25
	, 18, 25, 30, 18, 30, 21, 18, 21, 31, 18, 31, 24
	, 18, 24, 32, 18, 32, 22, 18, 22, 28, 18, 28, 26
};*/

const WCHAR *textureFiles[] = {
	L"texture.bmp"
};

const INT tIndices[] = { 0 };

const size_t verticesc_size = sizeof(verticesc);
const size_t indices_size = sizeof(indices);
const size_t textureFiles_size = sizeof(textureFiles);
const size_t tIndices_size = sizeof(tIndices);