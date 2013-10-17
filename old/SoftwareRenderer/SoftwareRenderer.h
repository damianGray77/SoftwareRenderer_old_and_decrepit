#ifndef SOFTWARERENDERER_H
#define SOFTWARERENDERER_H

#include "Color.h"
#include "WinBuffer.h"
#include "Buffer.h"
#include "Matrix.h"
#include "Bitmap.h"
#include "Triangle.h"
#include "Model.h"

class SoftwareRenderer {
public:
	SoftwareRenderer() {}

	HRESULT Init();
	HRESULT SwapBuffers();
	HRESULT CreateBuffer(WinBuffer &buffer);
	BOOL CreateBuffer(Buffer &buffer);
	VOID ReleaseBuffer(WinBuffer &buffer);
	VOID ReleaseBuffer(Buffer &buffer);
	VOID SetFov(FLOAT newFov);
	VOID SetClip(LONG cNear, LONG cFar);
	VOID RecalcDist();
	VOID Project(const INT i);
	VOID Clear(DWORD color);
	HRESULT Render();
	HRESULT SetupGeometry();
	HRESULT SetupTextures();

	VOID DrawScanLineTriangle(Triangle &t);
    VOID DrawHalfSpaceTriangle(Triangle &t);

    /*VOID DrawLine(Vertex2c &v1, Vertex2c &v2);
	VOID DrawLine(Vertex3c &v1, Vertex3c &v2);*/

	HRESULT SetSize(DWORD height, DWORD width);
	VOID CleanUp();

	~SoftwareRenderer() {}

	WinBuffer mBuffer;
	WinBuffer cBuffer;
	Buffer zBuffer;
	HDC mDeviceContext;
	FLOAT fov, dist, clipScale;
	LONG clipNear, clipFar;
    LONG width, height;
	static Model player;
	Bitmap *textures[256];
	
	Matrix4x4 mWorld;
	Matrix3x3 mWorldRot;
};

extern SoftwareRenderer *g_3D;

#endif