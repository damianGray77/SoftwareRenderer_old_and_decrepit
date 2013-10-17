#ifndef SOFTWARERENDERER_GLOBALS_H
#define SOFTWARERENDERER_GLOBALS_H

#include "Vertex.h"

typedef double DOUBLE;

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
DWORD MULTIPLYRGBA(DWORD color, FLOAT r, FLOAT g, FLOAT b, FLOAT a);

extern FLOAT sinTbl[SINCOSMAX];
extern FLOAT cosTbl[SINCOSMAX];
extern FLOAT invSinTbl[SINCOSMAX];
extern FLOAT invCosTbl[SINCOSMAX];
extern BOOL paused;
extern INT numCores;

FLOAT inverseSqrt(FLOAT number);

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

/*extern Vertex3c verticesc[];
extern INT indices[];
extern WCHAR *textureFiles[];
extern INT tIndices[];*/

#endif