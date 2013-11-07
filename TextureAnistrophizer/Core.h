#ifndef TEXTURE_ANISTROPHIZER_CORE_H
#define TEXTURE_ANISTROPHIZER_CORE_H

#include "WinBuffer.h"

#define DOUBLE double

#define SWAP(type, i, j) { type _temp_var = i; i = j; j = _temp_var; }
#define CLAMP(num, min, max) { if(num > max) { num = max; } if(num < min) { num = min; } }

#define AB(color) ((BYTE)(color >> 24))
#define RB(color) ((BYTE)(color >> 16))
#define GB(color) ((BYTE)(color >> 8))
#define BB(color) ((BYTE)(color))

#define AF(color) ((AB(color) + 1.0f) * 0.00390625f)
#define RF(color) ((RB(color) + 1.0f) * 0.00390625f)
#define GF(color) ((GB(color) + 1.0f) * 0.00390625f)
#define BF(color) ((BB(color) + 1.0f) * 0.00390625f)

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

#define RGBAb(r, g, b, a) (((DWORD)(a)) << 24 | ((DWORD)(r)) << 16 | ((DWORD)(g)) << 8 | ((DWORD)(b)))
#define RGBd(r, g, b) (4278190080 | ((DWORD)(r)) << 16 | ((DWORD)(g)) << 8 | ((DWORD)(b)))

#define MAX_HEIGHT 4096
#define FLT_MARGIN 0 //1e-4
#define PI 3.14159265358

#define kSpeed 0.3f

struct WindowProperties {
	HWND hWnd;
	HDC hDC;
	WinBuffer *buffer;
	RECT screenRect;
	DWORD screenWidth;
	DWORD screenHeight;
	BYTE colorDepth;
	BOOL fullScreen;
	LPCWSTR className;
	LPCWSTR windowName;
	FLOAT delta;
};

extern WindowProperties props;

#endif