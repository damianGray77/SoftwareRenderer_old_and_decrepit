#ifndef SOFTWARERENDERER_GLOBALS_H
#define SOFTWARERENDERER_GLOBALS_H

typedef double DOUBLE;

#define SWAP(type, i, j) { type _temp_var = i; i = j; j = _temp_var; }

#define AB(color) ((BYTE)(color >> 24))
#define RB(color) ((BYTE)(color >> 16))
#define GB(color) ((BYTE)(color >> 8))
#define BB(color) ((BYTE)(color))

#define AF(color) ((AB(color) + 1.0f) / 256.0f)
#define RF(color) ((RB(color) + 1.0f) / 256.0f)
#define GF(color) ((GB(color) + 1.0f) / 256.0f)
#define BF(color) ((BB(color) + 1.0f) / 256.0f)

#define SINCOSMAX 32767

#define SIN(x) (sinTbl[(INT)x&SINCOSMAX])
#define COS(x) (cosTbl[(INT)x&SINCOSMAX])
#define INVSIN(x) (invSinTbl[(INT)x&SINCOSMAX])
#define INVCOS(x) (invCosTbl[(INT)x&SINCOSMAX]) 
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define SGN(x) ((x) < 0 ? -1 : (x) > 0 ? 1 : 0)
#define INVERSE(x) ((1.0f) / (x))

#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)

#define COPYRGBA(dst, src) {			\
	dst.r = src.r;						\
	dst.g = src.g;						\
	dst.b = src.b;						\
	dst.a = src.a;						\
}

#define PI 3.14159265358
#define MAX_HEIGHT 4096
#define FLT_MARGIN 0 //1e-4

FLOAT* DW2RGBAF(DWORD color);
BYTE* DW2RGBAB(DWORD color);
DWORD RGBAb(BYTE r, BYTE g, BYTE b, BYTE a);
DWORD RGBAf(FLOAT r, FLOAT g, FLOAT b, FLOAT a);

extern FLOAT sinTbl[SINCOSMAX];
extern FLOAT cosTbl[SINCOSMAX];
extern FLOAT invSinTbl[SINCOSMAX];
extern FLOAT invCosTbl[SINCOSMAX];
extern BOOL paused;

float inverseSqrt(float number);

struct windowProperties {
	HWND hWnd;
	RECT screenRect;
	DWORD screenWidth;
	DWORD screenHeight;
	BYTE colorDepth;
	BOOL fullScreen;
	LPCWSTR className;
	LPCWSTR windowName;
	FLOAT delta;
};

extern windowProperties props;

static const Vertex3 vertices[] = {
	  { -1.0f, -1.0f, -1.0f }
	, {  1.0f, -1.0f, -1.0f }
	, { -1.0f,  1.0f, -1.0f }
	, {  1.0f,  1.0f, -1.0f }
	, { -1.0f, -1.0f,  1.0f }
	, {  1.0f, -1.0f,  1.0f }
	, { -1.0f,  1.0f,  1.0f }
	, {  1.0f,  1.0f,  1.0f }
};

static const Vertex3c verticesc[] = {
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

static const DWORD indices[] = {
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
};

static const WCHAR *textureFiles[] = {
	L"texture.bmp"
};

static const int tIndices[] = { 0 };

#endif