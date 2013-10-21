#include "stdafx.h"
#include "Buffers/WinBuffer.h"
#include "Buffers/Buffer.h"
#include "Files/Bitmap.h"
#include "Math/Matrix4x4.h"
#include "Math/Matrix3x3.h"
#include "Structs/Color4f.h"
#include "Structs/Color4b.h"
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
#include "Camera.h"
#include "SoftwareRenderer.h"

inline INT iRound(FLOAT x) {
	INT t;

	__asm	{
		fld  x
		fistp t
	}

	return t;
}

Model SoftwareRenderer::player;

HRESULT SoftwareRenderer::Init() {
	mDeviceContext = GetDC(props.hWnd);

	if(
		FAILED(CreateBuffer(mBuffer))
		|| FAILED(CreateBuffer(cBuffer))
		|| !CreateBuffer(zBuffer)
	) {
		return E_FAIL;
	}

	SetFov(SINCOSMAX / 16);
	
	SetClip(0, 100);

	return S_OK;
}

HRESULT SoftwareRenderer::SwapBuffers() {
	assert(NULL != mDeviceContext);
	assert(NULL != mBuffer.GetBufDC());

	GetClientRect(props.hWnd, &props.screenRect);

	if(!BitBlt(
		mDeviceContext
		, props.screenRect.left
		, props.screenRect.top
		, 1 + props.screenRect.right - props.screenRect.left
		, 1 + props.screenRect.bottom - props.screenRect.top
		, mBuffer.GetBufDC()
		, 0
		, 0
		, SRCCOPY
	)) {
		MessageBoxW(props.hWnd, L"SwapBuffers - Cannot swap buffers!", L"Error", MB_OK|MB_ICONERROR);
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT SoftwareRenderer::CreateBuffer(WinBuffer &buffer) {
	return buffer.Init(mDeviceContext, props.screenWidth, props.screenHeight);
}

VOID SoftwareRenderer::ReleaseBuffer(WinBuffer &buffer) {
	buffer.DeInit();
}

BOOL SoftwareRenderer::CreateBuffer(Buffer &buffer) {
	return buffer.Init(props.screenWidth, props.screenHeight);
}

VOID SoftwareRenderer::ReleaseBuffer(Buffer &buffer) {
	buffer.DeInit();
}

VOID SoftwareRenderer::SetFov(FLOAT newFov) {
	fov = newFov;

	RecalcDist();
}

VOID SoftwareRenderer::SetClip(LONG cNear, LONG cFar) {
	clipNear = cNear;
	clipFar = cFar;

	clipScale = 1.0f / (ABS(cNear) + ABS(cFar));
}

VOID SoftwareRenderer::RecalcDist() {
	FLOAT midFOV = fov * 0.5f;
	FLOAT cotFOV = COS(midFOV) * INVSIN(midFOV);
	
	dist = cotFOV * mBuffer.mHeight;
}

VOID SoftwareRenderer::Project(const INT i) {
	Vertex3c &sCoords = player.vertices[i].posProj;
	Vertex3 &coords = player.vertices[i].posTrans;
	const Vertex2 &uvMap = player.vertices[i].uvMap;
	const Color4f &color = player.vertices[i].color;
	
	coords = player.vertices[i].position * mWorld;
	
	if(0 == coords.z) {
		return;
	}

	FLOAT z = INVERSE(coords.z) * dist;

	sCoords.x = (coords.x * z) + mBuffer.mWidth;
	sCoords.y = (-coords.y * z) + mBuffer.mHeight;
	sCoords.z = coords.z;

	sCoords.c = color;
	sCoords.u = uvMap.x;
	sCoords.v = uvMap.y;
}

VOID SoftwareRenderer::Clear(DWORD color) {
	mBuffer.Clear(color);
	cBuffer.Clear(color);
	zBuffer.Clear(0.0f);
}

HRESULT SoftwareRenderer::Render() {
	Clear(0xff000000);

	static FLOAT rX = 1.0f, rY = 0.5f, rZ = 0.25f;

	Matrix4x4::Identity(mWorld);
	Matrix4x4::Rotate(mWorld, rX, rY, rZ);
	Matrix4x4::Translate(mWorld, g_Camera.Position().x, g_Camera.Position().y, g_Camera.Position().z);
	
	Matrix3x3::Copy(mWorldRot, mWorld);
	
	//Matrix4x4::Translate(mWorldInv, -g_Camera.Position().x, -g_Camera.Position().y, -g_Camera.Position().z);
	//Matrix4x4::Rotate(mWorldInv, -rX, -rY, -rZ);

	rX += 50.0f; if(rX > SINCOSMAX) { rX -= SINCOSMAX; }
	rY += 25.0f; if(rY > SINCOSMAX) { rY -= SINCOSMAX; }
	rZ += 12.5f; if(rZ > SINCOSMAX) { rZ -= SINCOSMAX; }

	player.numVisible = 0;

	INT i1, i2, i3;
	FLOAT biggest;
	INT poly;
	Vertex3c sv1, sv2, sv3;
	FLOAT area;

	for(INT i = 0; i < player.numFaces; ++i) {
		/*FLOAT dot = Vector3::Dot(g_Camera.Position(), player.polygons[i].normal * mWorldRot);
		if(dot > 0.0f) {
			continue;
		}*/

		i1 = player.polygons[i].verts[0];
		i2 = player.polygons[i].verts[1];
		i3 = player.polygons[i].verts[2];
		
		Project(i1);
		Project(i2);
		Project(i3);
		
		sv1 = player.vertices[i1].posProj;
		sv2 = player.vertices[i2].posProj;
		sv3 = player.vertices[i3].posProj;
		
		area = sv1.x * sv2.y - sv2.x * sv1.y;
		area += sv2.x * sv3.y - sv3.x * sv2.y;
		area += sv3.x * sv1.y - sv1.x * sv3.y;
		if(area > 0.0f) {
			continue;
		}

		player.polygons[i].cenZ = (
			player.vertices[i1].posTrans.z
			+ player.vertices[i2].posTrans.z
			+ player.vertices[i3].posTrans.z
		) / 3.0f;
		player.visible[player.numVisible++] = i;
		
		player.CalculateNormals(FALSE);
	}

	for(INT i = 0; i < player.numVisible; ++i) {
		biggest	= player.polygons[player.visible[i]].cenZ;

		for(INT j = i + 1; j < player.numVisible; ++j) {
			poly = player.visible[j];

			if(player.polygons[poly].cenZ >= biggest) {
				SWAP(INT, player.visible[j], player.visible[i]);
			}
		}
	}

	if(0 == player.numVisible) {
		SwapBuffers();

		return S_OK;
	}
		
	Triangle t;

	for(INT i = 0; i < player.numVisible; ++i) {
		poly = player.visible[i];
		
		t.v1 = player.vertices[player.polygons[poly].verts[0]].posProj;
		t.v2 = player.vertices[player.polygons[poly].verts[1]].posProj;
		t.v3 = player.vertices[player.polygons[poly].verts[2]].posProj;
		t.texId = player.polygons[poly].texID;
		t.v1.u = player.polygons[poly].uvs[0].x;
		t.v2.u = player.polygons[poly].uvs[1].x;
		t.v3.u = player.polygons[poly].uvs[2].x;
		t.v1.v = player.polygons[poly].uvs[0].y;
		t.v2.v = player.polygons[poly].uvs[1].y;
		t.v3.v = player.polygons[poly].uvs[2].y;
		t.v1.c = player.polygons[poly].cols[0];
		t.v2.c = player.polygons[poly].cols[1];
		t.v3.c = player.polygons[poly].cols[2];
		DrawScanLineTriangle(t);
		//DrawHalfSpaceTriangle(t);
	}

	SwapBuffers();

	return S_OK;
}

HRESULT SoftwareRenderer::SetupGeometry() {
	player.numFaces = indices_size / sizeof(Polygon3cuv);
	player.numVerts = verticesc_size / sizeof(Vertex3c);
	
	player.vertices = new PVertex3c[player.numVerts];
	for(INT i = 0; i < player.numVerts; ++i) {
		PVertex3c::Copy(player.vertices[i], verticesc[i]);
	}

	player.polygons = new Polygon3[player.numFaces];

	for(INT i = 0; i < player.numFaces; ++i) {
		player.polygons[i].verts[0] = indices[i].i1;
		player.polygons[i].verts[1] = indices[i].i2;
		player.polygons[i].verts[2] = indices[i].i3;
		
		player.polygons[i].cols[0] = indices[i].c1;
		player.polygons[i].cols[1] = indices[i].c2;
		player.polygons[i].cols[2] = indices[i].c3;
		
		player.polygons[i].uvs[0] = indices[i].uv1;
		player.polygons[i].uvs[1] = indices[i].uv2;
		player.polygons[i].uvs[2] = indices[i].uv3;
		
		player.polygons[i].texID = indices[i].tId;
	}

	player.visible = new INT[player.numFaces];
	memset(player.visible, 0, sizeof(player.visible));

	player.CalculateNormals(TRUE);

	return S_OK;
}

HRESULT SoftwareRenderer::SetupTextures() {
	int numTextures = textureFiles_size / sizeof(WCHAR*);

	for(int i = 0; i < numTextures; ++i) {
		Bitmap *bmp = new Bitmap(textureFiles[i]);
		
		textures[i] = bmp;
	}
	
	player.numTexts = tIndices_size / sizeof(INT);
	player.textures = new Texture[player.numTexts];
	for(int i = 0; i < player.numTexts; ++i) {
		player.textures[i].textureId = tIndices[i];
	}
	
	return S_OK;
}

VOID SoftwareRenderer::DrawScanLineTriangle(Triangle &t) {
	if(t.v1.y > t.v2.y) { SWAP(Vertex3c, t.v1, t.v2); }
	if(t.v1.y > t.v3.y) { SWAP(Vertex3c, t.v1, t.v3); }
	if(t.v2.y > t.v3.y) { SWAP(Vertex3c, t.v2, t.v3); }
	
	if(t.v3.y <= t.v1.y) {
		return;
	}
	
	const FLOAT z1 = 1.0f / t.v1.z;
	const FLOAT z2 = 1.0f / t.v2.z;
	const FLOAT z3 = 1.0f / t.v3.z;
	
	if(!(
		(z1 >= 0.0f && z1 <= 1.0f)
		|| (z2 >= 0.0f && z2 <= 1.0f)
		|| (z3 >= 0.0f && z3 <= 1.0f)
	)) {
		return;
	}

	const FLOAT x1 = t.v1.x, y1 = t.v1.y;
	const FLOAT x2 = t.v2.x, y2 = t.v2.y;
	const FLOAT x3 = t.v3.x, y3 = t.v3.y;
	const FLOAT u1 = t.v1.u * z1, v1 = t.v1.v * z1;
	const FLOAT u2 = t.v2.u * z2, v2 = t.v2.v * z2;
	const FLOAT u3 = t.v3.u * z3, v3 = t.v3.v * z3;
	const FLOAT r1 = t.v1.c.r, g1 = t.v1.c.g, b1 = t.v1.c.b;//, a1 = t.v1.c.a;
	const FLOAT r2 = t.v2.c.r, g2 = t.v2.c.g, b2 = t.v2.c.b;//, a2 = t.v2.c.a;
	const FLOAT r3 = t.v3.c.r, g3 = t.v3.c.g, b3 = t.v3.c.b;//, a3 = t.v3.c.a;

	BOOL swapX = FALSE;
	
	const Bitmap *texture = textures[t.texId];
	const INT tWidth = texture -> infoHeader.biWidth - 1;
	const INT tHeight = texture -> infoHeader.biHeight - 1;
	const BYTE *tBits = (BYTE *)texture -> data;
	FLOAT *zBits = (FLOAT *)zBuffer.bits;
	DWORD *dBits = (DWORD *)mBuffer.bits;
		
	const FLOAT dy1 = INVERSE(y3 - y1);
	
	FLOAT sx1 = (x3 - x1) * dy1;
	FLOAT sz1 = (z3 - z1) * dy1;
	FLOAT su1 = (u3 - u1) * dy1;
	FLOAT sv1 = (v3 - v1) * dy1;
	FLOAT sr1 = (r3 - r1) * dy1;
	FLOAT sg1 = (g3 - g1) * dy1;
	FLOAT sb1 = (b3 - b1) * dy1;
	//FLOAT sa1 = (a3 - a1) * dy1;
	
	FLOAT xx1 = x1, zz1 = z1;
	FLOAT uu1 = u1, vv1 = v1;
	FLOAT rr1 = r1, gg1 = g1;
	FLOAT bb1 = b1;//, aa1 = a1;
	
	if(y2 != y1) {
		const FLOAT dy2 = INVERSE(y2 - y1);
		
		FLOAT sx2 = (x2 - x1) * dy2;
		FLOAT sz2 = (z2 - z1) * dy2;
		FLOAT su2 = (u2 - u1) * dy2;
		FLOAT sv2 = (v2 - v1) * dy2;
		FLOAT sr2 = (r2 - r1) * dy2;
		FLOAT sg2 = (g2 - g1) * dy2;
		FLOAT sb2 = (b2 - b1) * dy2;
		//FLOAT sa2 = (a2 - a1) * dy2;
		
		FLOAT xx2 = x1, zz2 = z1;
		FLOAT uu2 = u1, vv2 = v1;
		FLOAT rr2 = r1, gg2 = g1;
		FLOAT bb2 = b1;//, aa2 = a1;
		
		INT endY = y2 >= height
			? height - 1
			: (INT)ceil(y2) - 1
		;
		FLOAT subY;
		INT startY;
		if(y1 <= 0) {
			subY = -y1;
			startY = 0;
		} else {
			startY = (INT)ceil(y1);
			subY = (FLOAT)startY - y1;
		}

		xx1 += sx1 * subY; xx2 += sx2 * subY;
		zz1 += sz1 * subY; zz2 += sz2 * subY;
		uu1 += su1 * subY; uu2 += su2 * subY;
		vv1 += sv1 * subY; vv2 += sv2 * subY;
		rr1 += sr1 * subY; rr2 += sr2 * subY;
		gg1 += sg1 * subY; gg2 += sg2 * subY;
		bb1 += sb1 * subY; bb2 += sb2 * subY;
		//aa1 += sa1 * subY; aa2 += sa2 * subY;
		
		if(sx2 < sx1) {
			SWAP(FLOAT, xx1, xx2); SWAP(FLOAT, sx1, sx2);
			SWAP(FLOAT, zz1, zz2); SWAP(FLOAT, sz1, sz2);
			SWAP(FLOAT, uu1, uu2); SWAP(FLOAT, su1, su2);
			SWAP(FLOAT, vv1, vv2); SWAP(FLOAT, sv1, sv2);
			SWAP(FLOAT, rr1, rr2); SWAP(FLOAT, sr1, sr2);
			SWAP(FLOAT, gg1, gg2); SWAP(FLOAT, sg1, sg2);
			SWAP(FLOAT, bb1, bb2); SWAP(FLOAT, sb1, sb2);
			//SWAP(FLOAT, aa1, aa2); SWAP(FLOAT, sa1, sa2);
			swapX = TRUE;
		}
		
		for(INT y = startY; y <= endY; ++y) {
			const FLOAT dx = INVERSE(xx2 - xx1);
			
			const FLOAT szx = (zz2 - zz1) * dx;
			const FLOAT sux = (uu2 - uu1) * dx;
			const FLOAT svx = (vv2 - vv1) * dx;
			const FLOAT srx = (rr2 - rr1) * dx;
			const FLOAT sgx = (gg2 - gg1) * dx;
			const FLOAT sbx = (bb2 - bb1) * dx;
			//const FLOAT sax = (aa2 - aa1) * dx;
			
			FLOAT zx = zz1;
			FLOAT ux = uu1;
			FLOAT vx = vv1;
			FLOAT rx = rr1;
			FLOAT gx = gg1;
			FLOAT bx = bb1;
			//FLOAT ax = aa1;

			INT endX = xx2 >= width
				? width - 1
				: (INT)ceil(xx2) - 1
			;
			FLOAT subX;
			INT startX;
			if(xx1 <= 0) {
				subX = -xx1;
				startX = 0;
			} else {
				startX = (INT)ceil(xx1);
				subX = (FLOAT)startX - xx1;
			}
			
			zx += szx * subX;
			ux += sux * subX;
			vx += svx * subX;
			rx += srx * subX;
			gx += sgx * subX;
			bx += sbx * subX;
			//ax += sax * subX;
			
			FLOAT *zBitsX = zBits + zBuffer.yOffsets[y];
			DWORD *dBitsX = dBits + mBuffer.yOffsets[y];
			
			for(INT x = startX; x <= endX; ++x) {
				if(zx < 1.0f && zx > zBitsX[x]) {
					FLOAT zp = 1.0f / zx;
					INT u = (INT)(ux * zp);
					INT v = (INT)(vx * zp);
					
					if(u < 0) {
						u = 0;
					} else if(u > tWidth) {
						u = tWidth;
					}
					if(v < 0) {
						v = 0;
					} else if(v > tHeight) {
						v = tHeight;
					}
				
					zBitsX[x] = zx;
					
					const LONG tOff = texture -> xOffsets[u] + texture -> yOffsets[v];

					dBitsX[x] = RGB(
						(FLOAT)tBits[tOff] * rx
						, (FLOAT)tBits[tOff + 1] * gx
						, (FLOAT)tBits[tOff + 2] * bx
					);
					
					//BYTE zx2 = (BYTE)(zx * 255.0f);
					//dBitsX[x] = RGBAb(zx2, zx2, zx2, 1.0f);
				}
				
				zx += szx;
				ux += sux;
				vx += svx;
				rx += srx;
				gx += sgx;
				bx += sbx;
				//ax += sax;
			}

			xx1 += sx1; xx2 += sx2;
			zz1 += sz1; zz2 += sz2;
			uu1 += su1; uu2 += su2;
			vv1 += sv1; vv2 += sv2;
			rr1 += sr1; rr2 += sr2;
			gg1 += sg1; gg2 += sg2;
			bb1 += sb1; bb2 += sb2;
			//aa1 += sa1; aa2 += sa2;
		}
		
		if(TRUE == swapX) {
			xx1 = xx2; sx1 = sx2;
			zz1 = zz2; sz1 = sz2;
			uu1 = uu2; su1 = su2;
			vv1 = vv2; sv1 = sv2;
			rr1 = rr2; sr1 = sr2;
			gg1 = gg2; sg1 = sg2;
			bb1 = bb2; sb1 = sb2;
			//aa1 = aa2; sa1 = sa2;
		}
	}

	if(y3 != y2) {
		const FLOAT dy2 = INVERSE(y3 - y2);
		
		FLOAT sx2 = (x3 - x2) * dy2;
		FLOAT sz2 = (z3 - z2) * dy2;
		FLOAT su2 = (u3 - u2) * dy2;
		FLOAT sv2 = (v3 - v2) * dy2;
		FLOAT sr2 = (r3 - r2) * dy2;
		FLOAT sg2 = (g3 - g2) * dy2;
		FLOAT sb2 = (b3 - b2) * dy2;
		//FLOAT sa2 = (a3 - a2) * dy2;
		
		FLOAT xx2 = x2, zz2 = z2;
		FLOAT uu2 = u2, vv2 = v2;
		FLOAT rr2 = r2, gg2 = g2;
		FLOAT bb2 = b2;//, aa2 = a2;
		
		INT endY = y3 >= height
			? height - 1
			: (INT)ceil(y3) - 1
		;
		FLOAT subY;
		INT startY;
		if(y2 <= 0) {
			subY = -y2;
			startY = 0;
		} else {
			startY = (INT)ceil(y2);
			subY = (FLOAT)startY - y2;
		}
		
		if(y2 != y1) {
			FLOAT dy = y2 - y1;

			xx1 = x1 + (sx1 * dy);
			zz1 = z1 + (sz1 * dy);
			uu1 = u1 + (su1 * dy);
			vv1 = v1 + (sv1 * dy);
			rr1 = r1 + (sr1 * dy);
			gg1 = g1 + (sg1 * dy);
			bb1 = b1 + (sb1 * dy);
			//aa1 = a1 + (sa1 * dy);
		}
		
		xx1 += sx1 * subY; xx2 += sx2 * subY;
		zz1 += sz1 * subY; zz2 += sz2 * subY;
		uu1 += su1 * subY; uu2 += su2 * subY;
		vv1 += sv1 * subY; vv2 += sv2 * subY;
		rr1 += sr1 * subY; rr2 += sr2 * subY;
		gg1 += sg1 * subY; gg2 += sg2 * subY;
		bb1 += sb1 * subY; bb2 += sb2 * subY;
		//aa1 += sa1 * subY; aa2 += sa2 * subY;
		
		if(TRUE == swapX) {
			SWAP(FLOAT, xx1, xx2); SWAP(FLOAT, sx1, sx2);
			SWAP(FLOAT, zz1, zz2); SWAP(FLOAT, sz1, sz2);
			SWAP(FLOAT, uu1, uu2); SWAP(FLOAT, su1, su2);
			SWAP(FLOAT, vv1, vv2); SWAP(FLOAT, sv1, sv2);
			SWAP(FLOAT, rr1, rr2); SWAP(FLOAT, sr1, sr2);
			SWAP(FLOAT, gg1, gg2); SWAP(FLOAT, sg1, sg2);
			SWAP(FLOAT, bb1, bb2); SWAP(FLOAT, sb1, sb2);
			//SWAP(FLOAT, aa1, aa2); SWAP(FLOAT, sa1, sa2);
		}

		for(INT y = startY; y <= endY; ++y) {
			const FLOAT dx = INVERSE(xx2 - xx1);
			
			const FLOAT szx = (zz2 - zz1) * dx;
			const FLOAT sux = (uu2 - uu1) * dx;
			const FLOAT svx = (vv2 - vv1) * dx;
			const FLOAT srx = (rr2 - rr1) * dx;
			const FLOAT sgx = (gg2 - gg1) * dx;
			const FLOAT sbx = (bb2 - bb1) * dx;
			//const FLOAT sax = (aa2 - aa1) * dx;
			
			FLOAT zx = zz1;
			FLOAT ux = uu1;
			FLOAT vx = vv1;
			FLOAT rx = rr1;
			FLOAT gx = gg1;
			FLOAT bx = bb1;
			//FLOAT ax = aa1;
			
			INT endX = xx2 >= width
				? width - 1
				: (INT)ceil(xx2) - 1
			;
			FLOAT subX;
			INT startX;
			if(xx1 <= 0) {
				subX = -xx1;
				startX = 0;
			} else {
				startX = (INT)ceil(xx1);
				subX = (FLOAT)startX - xx1;
			}
			
			zx += szx * subX;
			ux += sux * subX;
			vx += svx * subX;
			rx += srx * subX;
			gx += sgx * subX;
			bx += sbx * subX;
			//ax += sax * subX;
			
			FLOAT *zBitsX = zBits + zBuffer.yOffsets[y];
			DWORD *dBitsX = dBits + mBuffer.yOffsets[y];
			
			for(INT x = startX; x <= endX; ++x) {
				if(zx < 1.0f && zx > zBitsX[x]) {
					FLOAT zp = 1.0f / zx;
					INT u = (INT)(ux * zp);
					INT v = (INT)(vx * zp);
					
					if(u < 0) {
						u = 0;
					} else if(u > tWidth) {
						u = tWidth;
					}
					if(v < 0) {
						v = 0;
					} else if(v > tHeight) {
						v = tHeight;
					}
				
					zBitsX[x] = zx;
					
					const LONG tOff = texture -> xOffsets[u] + texture -> yOffsets[v];
					
					dBitsX[x] = RGB(
						(FLOAT)tBits[tOff] * rx
						, (FLOAT)tBits[tOff + 1] * gx
						, (FLOAT)tBits[tOff + 2] * bx
					);

					//BYTE zx2 = (BYTE)(zx * 255.0f);
					//dBitsX[x] = RGBAb(zx2, zx2, zx2, 1.0f);
				}

				zx += szx;
				ux += sux;
				vx += svx;
				rx += srx;
				gx += sgx;
				bx += sbx;
				//ax += sax;
			}

			xx1 += sx1; xx2 += sx2;
			zz1 += sz1; zz2 += sz2;
			uu1 += su1; uu2 += su2;
			vv1 += sv1; vv2 += sv2;
			rr1 += sr1; rr2 += sr2;
			gg1 += sg1; gg2 += sg2;
			bb1 += sb1; bb2 += sb2;
			//aa1 += sa1; aa2 += sa2;
		}
	}
}

VOID SoftwareRenderer::DrawHalfSpaceTriangle(Triangle &t) {
	//const Bitmap *texture = textures[t.texId];
	
	//FLOAT *zBits = (FLOAT *)zBuffer.bits;
	DWORD *dBits = (DWORD *)mBuffer.bits;
	//BYTE *tBits = (BYTE *)texture -> data;

	const INT X1 = (INT)(16.0f * t.v1.x);
	const INT X2 = (INT)(16.0f * t.v2.x);
	const INT X3 = (INT)(16.0f * t.v3.x);
	
	const INT Y1 = (INT)(16.0f * t.v1.y);
	const INT Y2 = (INT)(16.0f * t.v2.y);
	const INT Y3 = (INT)(16.0f * t.v3.y);
	
	const INT Z1 = (INT)(16.0f * t.v1.z);
	const INT Z2 = (INT)(16.0f * t.v2.z);
	const INT Z3 = (INT)(16.0f * t.v3.z);

	const INT minx = MIN(X1, MIN(X2, X3));
	const INT maxx = MAX(X1, MAX(X2, X3));
	const INT miny = MIN(Y1, MIN(Y2, Y3));
	const INT maxy = MAX(Y1, MAX(Y2, Y3));
	
	const INT xBound = maxx - minx;
	const INT yBound = maxy - miny;
	
	const int q = xBound <= 3200 || yBound <= 3200 ? 8 : 16;
	
	const INT cminx = MAX(0, (minx + 0xF) >> 4) & ~(q - 1);
	const INT cmaxx = MIN(width, (maxx + 0xF) >> 4);
	const INT cminy = MAX(0, (miny + 0xF) >> 4) & ~(q - 1);
	const INT cmaxy = MIN(height, (maxy + 0xF) >> 4);
	const INT minz = MIN(Z1, MIN(Z2, Z3));
	const INT maxz = MAX(Z1, MAX(Z2, Z3));
	
	if(cmaxx < cminx || cmaxy < cminy) {
		return;
	}
	
	const INT dy12 = Y1 - Y2;
	const INT dy23 = Y2 - Y3;
	const INT dy31 = Y3 - Y1;
	
	const INT dx12 = X1 - X2;
	const INT dx23 = X2 - X3;
	const INT dx31 = X3 - X1;
	
	const INT fdx12 = dx12 << 4;
	const INT fdx23 = dx23 << 4;
	const INT fdx31 = dx31 << 4;

	const INT fdy12 = dy12 << 4;
	const INT fdy23 = dy23 << 4;
	const INT fdy31 = dy31 << 4;
	
	dBits += mBuffer.yOffsets[cminy];
	
	INT c1 = dy12 * X1 - dx12 * Y1;
	INT c2 = dy23 * X2 - dx23 * Y2;
	INT c3 = dy31 * X3 - dx31 * Y3;
	
	if(dy12 < 0 || (0 == dy12 && dx12 > 0)) {
		++c1;
	}
	if(dy23 < 0 || (0 == dy23 && dx23 > 0)) {
		++c2;
	}
	if(dy31 < 0 || (0 == dy31 && dx31 > 0)) {
		++c3;
	}

	for(INT y = cminy; y < cmaxy; y += q) {
		INT y0 = y << 4;
		INT y1 = (y + q - 1) << 4;
		
		INT c1dx12y0 = c1 + dx12 * y0;
		INT c1dx12y1 = c1 + dx12 * y1;
		INT c2dx23y0 = c2 + dx23 * y0;
		INT c2dx23y1 = c2 + dx23 * y1;
		INT c3dx31y0 = c3 + dx31 * y0;
		INT c3dx31y1 = c3 + dx31 * y1;
		
		for(INT x = cminx; x < cmaxx; x += q) {
			INT x0 = x << 4;
			INT x1 = (x + q - 1) << 4;
			
			INT dy12x0 = dy12 * x0;
			INT dy12x1 = dy12 * x1;
			INT cy1 = c1dx12y0 - dy12x0;

			BOOL a00 = cy1 > 0;
			BOOL a10 = c1dx12y0 - dy12x1 > 0;
			BOOL a01 = c1dx12y1 - dy12x0 > 0;
			BOOL a11 = c1dx12y1 - dy12x1 > 0;
			INT a = (a00 << 0) | (a10 << 1) | (a01 << 2) | (a11 << 3);
			
			if(0x0 == a) {
				continue;
			}
			
			INT dy23x0 = dy23 * x0;
			INT dy23x1 = dy23 * x1;
			INT cy2 = c2dx23y0 - dy23x0;

			BOOL b00 = cy2 > 0;
			BOOL b10 = c2dx23y0 - dy23x1 > 0;
			BOOL b01 = c2dx23y1 - dy23x0 > 0;
			BOOL b11 = c2dx23y1 - dy23x1 > 0;
			INT b = (b00 << 0) | (b10 << 1) | (b01 << 2) | (b11 << 3);
			
			if(0x0 == b) {
				continue;
			}
			
			INT dy31x0 = dy31 * x0;
			INT dy31x1 = dy31 * x1;
			INT cy3 = c3dx31y0 - dy31x0;

			BOOL c00 = cy3 > 0;
			BOOL c10 = c3dx31y0 - dy31x1 > 0;
			BOOL c01 = c3dx31y1 - dy31x0 > 0;
			BOOL c11 = c3dx31y1 - dy31x1 > 0;
			INT c = (c00 << 0) | (c10 << 1) | (c01 << 2) | (c11 << 3);
			
			if(0x0 == c) {
				continue;
			}
			
			DWORD *dBitsX = dBits;
			//BYTE *tBitsX = tBits + texture -> yOffsets[y % 64];
			//if(tBitsX >= tBits + 4096) { tBitsX -= 4096; }
			if(0xF == a && 0xF == b && 0xF == c) {
				//DWORD *dBitsX2 = dBitsX + width;
				
				for(INT iy = y, iyq = y + q; iy < iyq; ++iy) {
					for(INT ix = x, ixq = x + q; ix < ixq; ix += 4) {
						/*LONG offset = texture -> xOffsets[ix % 63];
						dBitsX[ix] = RGBAb(
							tBitsX[offset]
							, tBitsX[offset + 1]
							, tBitsX[offset + 2]
							, 255
						);*/
						
						dBitsX[ix] = 0xff007f00;
						dBitsX[ix + 1] = 0xff007f00;
						dBitsX[ix + 2] = 0xff007f00;
						dBitsX[ix + 3] = 0xff007f00;
						//dBitsX2[ix] = 0xff007f00;
						//dBitsX2[ix + 1] = 0xff007f00;
					}

					dBitsX += width;
					//dBitsX2 = dBitsX + width;
					//tBitsX = tBits + texture -> yOffsets[iy % 63];
				}
			} else {
				for(INT iy = y, iyq = y + q; iy < iyq; ++iy) {
					INT cx1 = cy1 - 1;
					INT cx2 = cy2 - 1;
					INT cx3 = cy3 - 1;

					for(INT ix = x, ixq = x + q; ix < ixq; ++ix) {
						if((cx1 | cx2 | cx3) >= 0) {
							/*LONG offset = texture -> xOffsets[ix % 63];
							dBitsX[ix] = RGBAb(
								tBitsX[offset]
								, tBitsX[offset + 1]
								, tBitsX[offset + 2]
								, 255
							);*/
							
							dBitsX[ix] = 0xff007f00;
						}

						cx1 -= fdy12;
						cx2 -= fdy23;
						cx3 -= fdy31;
					}

					cy1 += fdx12;
					cy2 += fdx23;
					cy3 += fdx31;

					dBitsX += width;
					//tBitsX = tBits + texture -> yOffsets[iy % 63];
				}
			}
		}

		dBits += mBuffer.yOffsets[q];
	}
}

/*VOID SoftwareRenderer::DrawLine(Vertex2c &v1, Vertex2c &v2) {
	DWORD *bits, *cBits;
	LONG width;
	LONG offset;
	Vertex2c v[2];
	Color4f dc, vc[2], c, subC;
	FLOAT x, y, sub;
	FLOAT dx, dy;

	v[0] = v1;
	v[1] = v2;

	vc[0] = v[0].color;
	vc[1] = v[1].color;

	dy = v[0].y - v[1].y;
	dx = v[0].x - v[1].x;
	dc = vc[0] - vc[1];

	bits = (DWORD*)mBuffer.GetBits();
	cBits = (DWORD*)cBuffer.GetBits();
	width = mBuffer.GetWidth();

	//sub = dc / dy;
	//FLOAT c = v[0].color;

	if(abs(dy) > abs(dx)) {
		if(v[0].y > v[1].y) {
			SWAP(Vertex2c, v[0], v[1]);
			SWAP(Color4f, vc[0], vc[1]);
		}

		subC = dc / dy;
		sub = dx / dy;

		c = vc[0];
		x = v[0].x;

		LONG y1 = (LONG)ceil(v[0].y);
		LONG y2 = (LONG)ceil(v[1].y);
		if(y1 < 0) {
			x -= sub * y1;
			c -= subC * y1;
			y1 = 0;
		}
		if(y2 >= (LONG)props.screenHeight) {
			y2 = props.screenHeight - 1;
		}
		for(INT y = y1; y < y2; y++) {
			if(x >= props.screenWidth) {
				break;
			}

			if(x >= 0) {
				offset = (y * width) + (INT)x;
				if(0x0 == *(cBits + offset)) {
					*(bits + offset) = RGBAColor4f(c);
					*(cBits + offset) = 0xffffff;
				}
			}

			x += sub;
			c += subC;
		}
	} else {
		if(v[0].x > v[1].x) {
			SWAP(Vertex2c, v[0], v[1]);
			SWAP(Color4f, vc[0], vc[1]);
		}

		subC = dc / dx;
		sub = dy / dx;

		c = vc[0];
		y = v[0].y;

		LONG x1 = (LONG)ceil(v[0].x);
		LONG x2 = (LONG)ceil(v[1].x);
		if(x1 < 0) {
			y -= sub * x1;
			c -= subC * x1;
			x1 = 0;
		}
		if(x2 >= (LONG)props.screenWidth) {
			x2 = props.screenWidth - 1;
		}

		for(INT x = x1; x < x2; x++) {
			if(y >= props.screenHeight) {
				break;
			}

			if(y >= 0) {
				offset = (((INT)y) * width) + x;
				if(0x0 == *(cBits + offset)) {
					*(bits + offset) = RGBAColor4f(c);
					*(cBits + offset) = 0xffffff;
				}
			}

			y += sub;
			c+= subC;
		}
	}
}

VOID SoftwareRenderer::DrawLine(Vertex3c &v1, Vertex3c &v2) {
	DWORD *bits, *cBits;
	LONG width;
	LONG offset;
	Vertex3c v[2];
	Color4f dc, vc[2], c, subC;
	FLOAT x, y, sub;
	FLOAT dx, dy;

	v[0] = v1;
	v[1] = v2;

	vc[0] = v[0].color;
	vc[1] = v[1].color;

	dy = v[0].y - v[1].y;
	dx = v[0].x - v[1].x;
	dc = vc[0] - vc[1];

	bits = (DWORD*)mBuffer.GetBits();
	cBits = (DWORD*)cBuffer.GetBits();
	width = mBuffer.GetWidth();

	//sub = dc / dy;
	//FLOAT c = v[0].color;

	if(abs(dy) > abs(dx)) {
		if(v[0].y > v[1].y) {
			SWAP(Vertex3c, v[0], v[1]);
			SWAP(Color4f, vc[0], vc[1]);
		}

		subC = dc / dy;
		sub = dx / dy;

		c = vc[0];
		x = v[0].x;

		LONG y1 = (LONG)ceil(v[0].y);
		LONG y2 = (LONG)ceil(v[1].y);
		if(y1 < 0) {
			x -= sub * y1;
			c -= subC * y1;
			y1 = 0;
		}
		if(y2 >= (LONG)props.screenHeight) {
			y2 = props.screenHeight - 1;
		}
		for(INT y = y1; y < y2; y++) {
			if(x >= props.screenWidth) {
				break;
			}

			if(x >= 0) {
				offset = (y * width) + (INT)x;
				if(*(cBits + offset) < 0xffffff) {
					*(bits + offset) = RGBAColor4f(c);
					*(cBits + offset) = 0xffffff;
				}
			}

			x += sub;
			c += subC;
		}
	} else {
		if(v[0].x > v[1].x) {
			SWAP(Vertex3c, v[0], v[1]);
			SWAP(Color4f, vc[0], vc[1]);
		}

		subC = dc / dx;
		sub = dy / dx;

		c = vc[0];
		y = v[0].y;

		LONG x1 = (LONG)ceil(v[0].x);
		LONG x2 = (LONG)ceil(v[1].x);
		if(x1 < 0) {
			y -= sub * x1;
			c -= subC * x1;
			x1 = 0;
		}
		if(x2 >= (LONG)props.screenWidth) {
			x2 = props.screenWidth - 1;
		}

		for(INT x = x1; x < x2; x++) {
			if(y >= props.screenHeight) {
				break;
			}

			if(y >= 0) {
				offset = (((INT)y) * width) + x;
				//if(*(cBits + offset) < 0xffffff) {
					*(bits + offset) = RGBAColor4f(c);
					*(cBits + offset) = 0xffffff;
				//}
			}

			y += sub;
			c+= subC;
		}
	}
}*/

HRESULT SoftwareRenderer::SetSize(DWORD width, DWORD height) {
	ReleaseBuffer(mBuffer);
	ReleaseBuffer(cBuffer);
	ReleaseBuffer(zBuffer);

	props.screenHeight = height;
	props.screenWidth = width;
	GetWindowRect(props.hWnd, &props.screenRect);

	if(
		FAILED(mBuffer.Init(mDeviceContext, width, height))
		|| FAILED(cBuffer.Init(mDeviceContext, width, height))
		|| FALSE == zBuffer.Init(width, height)
	) {
		return E_FAIL;
	}

    this -> width = (LONG)width;
    this -> height = (LONG)height;

	RecalcDist();

	return S_OK;
}

VOID SoftwareRenderer::CleanUp() {
	ReleaseBuffer(mBuffer);
	ReleaseBuffer(cBuffer);
	ReleaseBuffer(zBuffer);

	if(NULL != mDeviceContext) {
		ReleaseDC(props.hWnd, mDeviceContext);
	}

	mDeviceContext = NULL;
}

SoftwareRenderer sftObj;
SoftwareRenderer *g_3D = &sftObj;