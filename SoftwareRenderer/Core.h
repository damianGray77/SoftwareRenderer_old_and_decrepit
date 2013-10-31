#ifndef SOFTWARERENDERER_CORE_H
#define SOFTWARERENDERER_CORE_H

#include "Structs/Vertex3c.h"
#include "Structs/Polygon3cuv.h"
#include "Structs/Light.h"

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

#define SINCOSMAX 32767

#define SIN(x) (sinTbl[(INT)x & SINCOSMAX])
#define COS(x) (cosTbl[(INT)x & SINCOSMAX])
#define INVSIN(x) (invSinTbl[(INT)x & SINCOSMAX])
#define INVCOS(x) (invCosTbl[(INT)x & SINCOSMAX]) 
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
	RECT screenRect;
	DWORD screenWidth;
	DWORD screenHeight;
	BYTE colorDepth;
	BOOL fullScreen;
	LPCWSTR className;
	LPCWSTR windowName;
	FLOAT delta;
};

FLOAT* DW2RGBAF(DWORD color);
BYTE* DW2RGBAB(DWORD color);
DWORD RGBAf(FLOAT r, FLOAT g, FLOAT b, FLOAT a);
DWORD MULTIPLYRGBA(DWORD color, FLOAT r, FLOAT g, FLOAT b, FLOAT a);
FLOAT inverseSqrt(FLOAT number);

extern FLOAT sinTbl[SINCOSMAX];
extern FLOAT cosTbl[SINCOSMAX];
extern FLOAT invSinTbl[SINCOSMAX];
extern FLOAT invCosTbl[SINCOSMAX];

extern INT numCores;
extern BOOL paused;

extern WindowProperties props;

extern const Vertex3c verticesc[];
extern const Polygon3cuv indices[];
extern const WCHAR *textureFiles[];
extern const INT tIndices[];
extern const Light lights[];

extern const size_t verticesc_size;
extern const size_t indices_size;
extern const size_t textureFiles_size;
extern const size_t tIndices_size;
extern const size_t lights_size;

#endif