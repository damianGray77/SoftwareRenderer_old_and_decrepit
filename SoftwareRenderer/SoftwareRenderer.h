#ifndef SOFTWARERENDERER_H
#define SOFTWARERENDERER_H

struct ScanLineData2 {
	BOOL left;
	FLOAT invDeltaY[3], slopeX[2], x[2];
	Color4f slopeC[2], c[2];
	DWORD *sBits;
	FLOAT *zBits;
};

struct ScanLine2 {
	LONG y;
	FLOAT x[2];
	Color4f c[2];
	DWORD *sBits;
	FLOAT *zBits;
};

struct ScanLineData3 {
	BOOL left;
	FLOAT invDeltaY[3], slopeX[2], x[2], slopeZ[2], z[2];
	Color4f slopeC[2], c[2];
	DWORD *sBits;
	FLOAT *zBits;
};

struct ScanLine3 {
	LONG y;
	FLOAT x[2], z[2];
	Color4f c[2];
	DWORD *sBits;
	FLOAT *zBits;
};

struct ScanLineDataT {
	BOOL left;
	FLOAT invDeltaY[3];
	FLOAT slopeX[2], x[2];
	FLOAT slopeZ[2], z[2];
	FLOAT slopeU[2], u[2];
	Color4f slopeC[2], c[2];
};

struct ScanLineT {
	LONG y;
	FLOAT v;
	FLOAT x[2], z[2], u[2];
	Color4f c[2];
};

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
	VOID Project2(Vertex2c &sCoords, const Vertex3 &coords, DWORD color);
	VOID Project3(Vertex3c &sCoords, const Vertex3 &coords, DWORD color);
	VOID Clear(DWORD color);
	HRESULT Render();
	HRESULT SetupGeometry();
	HRESULT SetupTextures();

	VOID DrawScanLineTriangle(Triangle &t);
    VOID DrawHalfSpaceTriangle(Triangle &t);

    /*VOID DrawLine(Vertex2c &v1, Vertex2c &v2);
	VOID DrawLine(Vertex3c &v1, Vertex3c &v2);
	VOID DrawTriangle2(DWORD* sBits, FLOAT* zBits, const Vertex3c &v1, const Vertex3c &v2, const Vertex3c &v3);
	VOID DrawTriangleTextured(Vertex3c &v1, Vertex3c &v2, Vertex3c &v3);
	VOID DrawTriangle(Vertex2c &v1, Vertex2c &v2, Vertex2c &v3);
	VOID DrawTriangle(DWORD* sBits, FLOAT* zBits, const Vertex3c &v1, const Vertex3c &v2, const Vertex3c &v3);*/

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