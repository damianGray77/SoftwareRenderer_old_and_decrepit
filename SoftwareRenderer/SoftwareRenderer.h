#ifndef SOFTWARERENDERER_H
#define SOFTWARERENDERER_H

#include "Buffers/WinBuffer.h"
#include "Buffers/Buffer.h"
#include "Files/Bitmap.h"
#include "Math/Matrix4x4.h"
#include "Structs/Texture.h"
#include "Structs/Vertex2.h"
#include "Structs/Vertex3.h"
#include "Structs/Vertex3c.h"
#include "Structs/Vector3.h"
#include "Structs/Triangle.h"
#include "Structs/Polygon3.h"
#include "Structs/PVertex3c.h"
#include "Structs/Polygon3cuv.h"
#include "Structs/Model.h"
#include "Structs/Light.h"
#include "Camera.h"

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
	VOID Project(PVertex3c &v);
	VOID CalculatePolygonLight(const INT i, const PVertex3c &light);
	VOID Clear(DWORD color);
	HRESULT Render();
	HRESULT SetupGeometry();
	HRESULT SetupTextures();

	VOID DrawScanLineTriangle(Triangle &t);
	VOID DrawScanLineTriangleFlat(Triangle &t);
    VOID DrawHalfSpaceTriangle(Triangle &t);

    VOID DrawLine(Vertex2 &v1, Vertex2 &v2, DWORD color);
    
	HRESULT SetSize(DWORD height, DWORD width);
	VOID CleanUp();

	~SoftwareRenderer() {}

	WinBuffer mBuffer;
	Buffer zBuffer;
	HDC mDeviceContext;
	FLOAT fov, dist, clipScale;
	LONG clipNear, clipFar;
    LONG width, height;
	static Model player;
	Bitmap *textures[256];
	INT numLights;
	
	Matrix4x4 mWorld;
	Matrix4x4 mWorldInv;
};

extern SoftwareRenderer *g_3D;

#endif