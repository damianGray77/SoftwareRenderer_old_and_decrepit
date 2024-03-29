#include "stdafx.h"
#include "SoftwareRenderer.h"
#include "Model.h"
#include "Globals.h"
#include "stdafx.h"
#include "Camera.h"
#include "Color.h"
#include "Vertex.h"
#include "Matrix.h"
#include "Triangle.h"
#include "Polygon.h"

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

	clipScale = 1.0f / (abs(cNear) + abs(cFar));
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

	sCoords.color = color;
	sCoords.u = uvMap.x;
	sCoords.v = uvMap.y;
}

VOID SoftwareRenderer::Clear(DWORD color) {
	mBuffer.Clear(color);
	cBuffer.Clear(color);
	zBuffer.Clear(1.0f);
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

	if(player.numVisible > 0) {
		FLOAT *zBits = (FLOAT*)zBuffer.bits;
		DWORD *sBits = (DWORD*)mBuffer.bits;
		
		Triangle t;

		for(INT i = 0; i < player.numVisible; ++i) {
			poly = player.visible[i];
			
			t.v1 = player.vertices[player.polygons[poly].verts[0]].posProj;
			t.v2 = player.vertices[player.polygons[poly].verts[1]].posProj;
			t.v3 = player.vertices[player.polygons[poly].verts[2]].posProj;
			t.texId = player.textures[0].textureId;
			
			//FLOAT area = (sv1.x * sv2.y - sv2.x * sv1.y) + (sv2.x * sv3.y - sv3.x * sv2.y) + (sv3.x * sv1.y - sv1.x * sv3.y);
			//if(area > 0) {
			//	continue;
			//}
			
			DrawScanLineTriangle(t);
		}
	}

	SwapBuffers();

	return S_OK;
}

HRESULT SoftwareRenderer::SetupGeometry() {
	player.numFaces = sizeof(indices) / sizeof(INT) / 3;
	player.numVerts = sizeof(verticesc) / sizeof(Vertex3c);
	
	player.vertices = new PVertex3c[player.numVerts];
	for(INT i = 0; i < player.numVerts; ++i) {
		PVertex3c::Copy(player.vertices[i], verticesc[i]);
	}

	player.polygons = new Polygon3[player.numFaces];

	INT j = 0;
	for(INT i = 0; i < player.numFaces; ++i) {
		player.polygons[i].verts[0] = indices[j];
		player.polygons[i].verts[1] = indices[j + 1];
		player.polygons[i].verts[2] = indices[j + 2];

		j += 3;
	}

	player.visible = new INT[player.numFaces];
	memset(player.visible, 0, sizeof(player.visible));

	player.CalculateNormals(TRUE);

	return S_OK;
}

HRESULT SoftwareRenderer::SetupTextures() {
	int numTextures = sizeof(textureFiles) / sizeof(WCHAR*);

	for(int i = 0; i < numTextures; ++i) {
		Bitmap *bmp = new Bitmap(textureFiles[i]);
		
		textures[i] = bmp;
	}
	
	player.numTexts = sizeof(tIndices) / sizeof(INT);
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

	BOOL swapX = FALSE;
	
	const Bitmap *texture = textures[t.texId];

	FLOAT *zBits = (FLOAT *)zBuffer.bits;
	DWORD *dBits = (DWORD *)mBuffer.bits;
	const BYTE *tBits = (BYTE *)texture -> data;
	
	const INT tWidth = texture -> infoHeader.biWidth - 1;
	const INT tHeight = texture -> infoHeader.biHeight - 1;
	
	const FLOAT x1 = t.v1.x, y1 = t.v1.y, z1 = t.v1.z, u1 = t.v1.u * tWidth, v1 = t.v1.v * tHeight;
	const FLOAT x2 = t.v2.x, y2 = t.v2.y, z2 = t.v2.z, u2 = t.v2.u * tWidth, v2 = t.v2.v * tHeight;
	const FLOAT x3 = t.v3.x, y3 = t.v3.y, z3 = t.v3.z, u3 = t.v3.u * tWidth, v3 = t.v3.v * tHeight;
	
	const FLOAT* rgba1 = DW2RGBAF(t.v1.color);
	const FLOAT* rgba2 = DW2RGBAF(t.v2.color);
	const FLOAT* rgba3 = DW2RGBAF(t.v3.color);
	
	const FLOAT r1 = rgba1[0], g1 = rgba1[1], b1 = rgba1[2], a1 = rgba1[3];
	const FLOAT r2 = rgba2[0], g2 = rgba2[1], b2 = rgba2[2], a2 = rgba2[3];
	const FLOAT r3 = rgba3[0], g3 = rgba3[1], b3 = rgba3[2], a3 = rgba3[3];
	
	const FLOAT dy1 = INVERSE(y3 - y1);
	
	FLOAT sx1 = (x3 - x1) * dy1;
	FLOAT sz1 = (z3 - z1) * dy1;
	FLOAT su1 = (u3 - u1) * dy1;
	FLOAT sv1 = (v3 - v1) * dy1;
	FLOAT sr1 = (r3 - r1) * dy1;
	FLOAT sg1 = (g3 - g1) * dy1;
	FLOAT sb1 = (b3 - b1) * dy1;
	FLOAT sa1 = (a3 - a1) * dy1;
	
	FLOAT xx1 = x1, zz1 = z1;
	FLOAT uu1 = u1, vv1 = v1;
	FLOAT rr1 = r1, gg1 = g1;
	FLOAT bb1 = b1, aa1 = a1;
	
	if(y2 != y1) {
		const FLOAT dy2 = INVERSE(y2 - y1);
		
		FLOAT sx2 = (x2 - x1) * dy2;
		FLOAT sz2 = (z2 - z1) * dy2;
		FLOAT su2 = (u2 - u1) * dy2;
		FLOAT sv2 = (v2 - v1) * dy2;
		FLOAT sr2 = (r2 - r1) * dy2;
		FLOAT sg2 = (g2 - g1) * dy2;
		FLOAT sb2 = (b2 - b1) * dy2;
		FLOAT sa2 = (a2 - a1) * dy2;
		
		FLOAT xx2 = x1, zz2 = z1;
		FLOAT uu2 = u1, vv2 = v1;
		FLOAT rr2 = r1, gg2 = g1;
		FLOAT bb2 = b1, aa2 = a1;

		INT startY = (INT)ceil(y1);
		INT endY = (INT)ceil(y2) - 1;
		FLOAT subY = (FLOAT)startY - y1;

		if(startY < 0) {
			subY -= startY;
			startY = 0;
		}
		if(endY >= height) {
			endY = height - 1;
		}

		xx1 += sx1 * subY; xx2 += sx2 * subY;
		zz1 += sz1 * subY; zz2 += sz2 * subY;
		uu1 += su1 * subY; uu2 += su2 * subY;
		vv1 += sv1 * subY; vv2 += sv2 * subY;
		rr1 += sr1 * subY; rr2 += sr2 * subY;
		gg1 += sg1 * subY; gg2 += sg2 * subY;
		bb1 += sb1 * subY; bb2 += sb2 * subY;
		aa1 += sa1 * subY; aa2 += sa2 * subY;
		
		if(sx2 < sx1) {
			SWAP(FLOAT, xx1, xx2); SWAP(FLOAT, sx1, sx2);
			SWAP(FLOAT, zz1, zz2); SWAP(FLOAT, sz1, sz2);
			SWAP(FLOAT, uu1, uu2); SWAP(FLOAT, su1, su2);
			SWAP(FLOAT, vv1, vv2); SWAP(FLOAT, sv1, sv2);
			SWAP(FLOAT, rr1, rr2); SWAP(FLOAT, sr1, sr2);
			SWAP(FLOAT, gg1, gg2); SWAP(FLOAT, sg1, sg2);
			SWAP(FLOAT, bb1, bb2); SWAP(FLOAT, sb1, sb2);
			SWAP(FLOAT, aa1, aa2); SWAP(FLOAT, sa1, sa2);
			
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
			const FLOAT sax = (aa2 - aa1) * dx;
			
			FLOAT zx = zz1;
			FLOAT ux = uu1;
			FLOAT vx = vv1;
			FLOAT rx = rr1;
			FLOAT gx = gg1;
			FLOAT bx = bb1;
			FLOAT ax = aa1;

			INT startX = (INT)ceil(xx1);
			INT endX = (INT)ceil(xx2) - 1;
			FLOAT subX = (FLOAT)startX - xx1;
			
			if(startX < 0) {
				subX -= startX;
				startX = 0;
			}
			if(endX >= width) {
				endX = width - 1;
			}
			
			zx += szx * subX;
			ux += sux * subX;
			vx += svx * subX;
			rx += srx * subX;
			gx += sgx * subX;
			bx += sbx * subX;
			ax += sax * subX;
			
			FLOAT *zBitsX = zBits + zBuffer.yOffsets[y];
			DWORD *dBitsX = dBits + mBuffer.yOffsets[y];
			
			for(INT x = startX; x <= endX; ++x) {
				if(zx > zBitsX[x]) {
					zBitsX[x] = zx;
					
					const LONG tOffset =
						texture -> xOffsets[(INT)ux]
						+ texture -> yOffsets[(INT)vx]
					;

					dBitsX[x] = RGBAb(
						tBits[tOffset] * rx
						, tBits[tOffset + 1] * gx
						, tBits[tOffset + 2] * bx
						, 255 * ax
					);
					
					//FLOAT zx2 = zx / 10.0f;
					//CLAMP(zx2, 0.0f, 1.0f);

					//dBitsX[x] = RGBAf(zx2, zx2, zx2, 1.0f);
				}
				
				zx += szx;
				ux += sux;
				vx += svx;
				rx += srx;
				gx += sgx;
				bx += sbx;
				ax += sax;
			}

			xx1 += sx1; xx2 += sx2;
			zz1 += sz1; zz2 += sz2;
			uu1 += su1; uu2 += su2;
			vv1 += sv1; vv2 += sv2;
			rr1 += sr1; rr2 += sr2;
			gg1 += sg1; gg2 += sg2;
			bb1 += sb1; bb2 += sb2;
			aa1 += sa1; aa2 += sa2;
		}
		
		if(TRUE == swapX) {
			xx1 = xx2; sx1 = sx2;
			zz1 = zz2; sz1 = sz2;
			uu1 = uu2; su1 = su2;
			vv1 = vv2; sv1 = sv2;
			rr1 = rr2; sr1 = sr2;
			gg1 = gg2; sg1 = sg2;
			bb1 = bb2; sb1 = sb2;
			aa1 = aa2; sa1 = sa2;
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
		FLOAT sa2 = (a3 - a2) * dy2;
		
		FLOAT xx2 = x2, zz2 = z2;
		FLOAT uu2 = u2, vv2 = v2;
		FLOAT rr2 = r2, gg2 = g2;
		FLOAT bb2 = b2, aa2 = a2;

		INT startY = (INT)ceil(y2);
		INT endY = (INT)ceil(y3) - 1;
		FLOAT subY = (FLOAT)startY - y2;

		if(startY < 0) {
			subY -= startY;
			startY = 0;
		}
		if(endY >= height) {
			endY = height - 1;
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
			aa1 = a1 + (sa1 * dy);
		}
		
		xx1 += sx1 * subY; xx2 += sx2 * subY;
		zz1 += sz1 * subY; zz2 += sz2 * subY;
		uu1 += su1 * subY; uu2 += su2 * subY;
		vv1 += sv1 * subY; vv2 += sv2 * subY;
		rr1 += sr1 * subY; rr2 += sr2 * subY;
		gg1 += sg1 * subY; gg2 += sg2 * subY;
		bb1 += sb1 * subY; bb2 += sb2 * subY;
		aa1 += sa1 * subY; aa2 += sa2 * subY;
		
		if(TRUE == swapX) {
			SWAP(FLOAT, xx1, xx2); SWAP(FLOAT, sx1, sx2);
			SWAP(FLOAT, zz1, zz2); SWAP(FLOAT, sz1, sz2);
			SWAP(FLOAT, uu1, uu2); SWAP(FLOAT, su1, su2);
			SWAP(FLOAT, vv1, vv2); SWAP(FLOAT, sv1, sv2);
			SWAP(FLOAT, rr1, rr2); SWAP(FLOAT, sr1, sr2);
			SWAP(FLOAT, gg1, gg2); SWAP(FLOAT, sg1, sg2);
			SWAP(FLOAT, bb1, bb2); SWAP(FLOAT, sb1, sb2);
			SWAP(FLOAT, aa1, aa2); SWAP(FLOAT, sa1, sa2);
		}

		for(INT y = startY; y <= endY; ++y) {
			const FLOAT dx = INVERSE(xx2 - xx1);
			
			const FLOAT szx = (zz2 - zz1) * dx;
			const FLOAT sux = (uu2 - uu1) * dx;
			const FLOAT svx = (vv2 - vv1) * dx;
			const FLOAT srx = (rr2 - rr1) * dx;
			const FLOAT sgx = (gg2 - gg1) * dx;
			const FLOAT sbx = (bb2 - bb1) * dx;
			const FLOAT sax = (aa2 - aa1) * dx;
			
			FLOAT zx = zz1;
			FLOAT ux = uu1;
			FLOAT vx = vv1;
			FLOAT rx = rr1;
			FLOAT gx = gg1;
			FLOAT bx = bb1;
			FLOAT ax = aa1;
			
			INT startX = (INT)ceil(xx1);
			INT endX = (INT)ceil(xx2) - 1;
			FLOAT subX = (FLOAT)startX - xx1;
			
			if(startX < 0) {
				subX -= startX;
				startX = 0;
			}
			if(endX >= width) {
				endX = width - 1;
			}
			
			zx += szx * subX;
			ux += sux * subX;
			vx += svx * subX;
			rx += srx * subX;
			gx += sgx * subX;
			bx += sbx * subX;
			ax += sax * subX;
			
			FLOAT *zBitsX = zBits + zBuffer.yOffsets[y];
			DWORD *dBitsX = dBits + mBuffer.yOffsets[y];
			
			for(INT x = startX; x <= endX; ++x) {
				if(zx > zBitsX[x]) {
					zBitsX[x] = zx;
					
					const LONG tOffset =
						texture -> xOffsets[(INT)ux]
						+ texture -> yOffsets[(INT)vx]
					;
					
					dBitsX[x] = RGBAb(
						tBits[tOffset] * rx
						, tBits[tOffset + 1] * gx
						, tBits[tOffset + 2] * bx
						, 255 * ax
					);
					
					// zx2 = zx / 10.0f;
					//CLAMP(zx2, 0.0f, 1.0f);

					//dBitsX[x] = RGBAf(zx2, zx2, zx2, 1.0f);
				}

				zx += szx;
				ux += sux;
				vx += svx;
				rx += srx;
				gx += sgx;
				bx += sbx;
				ax += sax;
			}

			xx1 += sx1; xx2 += sx2;
			zz1 += sz1; zz2 += sz2;
			uu1 += su1; uu2 += su2;
			vv1 += sv1; vv2 += sv2;
			rr1 += sr1; rr2 += sr2;
			gg1 += sg1; gg2 += sg2;
			bb1 += sb1; bb2 += sb2;
			aa1 += sa1; aa2 += sa2;
		}
	}
}

VOID SoftwareRenderer::DrawHalfSpaceTriangle(Triangle &t) {
	FLOAT *zBits = (FLOAT *)zBuffer.bits;
	DWORD *dBits = (DWORD *)mBuffer.bits;
	DWORD *tBits = (DWORD *)textures[t.texId] -> data;

	const INT X1 = iRound(16.0f * t.v1.x);
	const INT X2 = iRound(16.0f * t.v2.x);
	const INT X3 = iRound(16.0f * t.v3.x);
	
	const INT Y1 = iRound(16.0f * t.v1.y);
	const INT Y2 = iRound(16.0f * t.v2.y);
	const INT Y3 = iRound(16.0f * t.v3.y);
	
	const INT Z1 = iRound(16.0f * t.v1.z);
	const INT Z2 = iRound(16.0f * t.v2.z);
	const INT Z3 = iRound(16.0f * t.v3.z);
	
	const INT q = 8;
	
	const INT minx = MAX(0, (MIN(X1, MIN(X2, X3)) + 0xF) >> 4) & ~(q - 1);
	const INT maxx = MIN(width, (MAX(X1, MAX(X2, X3)) + 0xF) >> 4);
	const INT miny = MAX(0, (MIN(Y1, MIN(Y2, Y3)) + 0xF) >> 4) & ~(q - 1);
	const INT maxy = MIN(height, (MAX(Y1, MAX(Y2, Y3)) + 0xF) >> 4);
	const INT minz = MIN(Z1, MIN(Z2, Z3));
	const INT maxz = MAX(Z1, MAX(Z2, Z3));
	
	if(maxx < minx || maxy < miny) {
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
	
	dBits += miny * width;
	
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

	for(INT y = miny; y < maxy; y += q) {
		INT y0 = y << 4;
		INT y1 = (y + q - 1) << 4;
		
		INT c1dx12y0 = c1 + dx12 * y0;
		INT c1dx12y1 = c1 + dx12 * y1;
		INT c2dx23y0 = c2 + dx23 * y0;
		INT c2dx23y1 = c2 + dx23 * y1;
		INT c3dx31y0 = c3 + dx31 * y0;
		INT c3dx31y1 = c3 + dx31 * y1;
		
		for(INT x = minx; x < maxx; x += q) {
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
			DWORD *tBitsX = tBits + ((y * 64) % 4096);
			if(tBitsX >= tBits + 4096) { tBitsX -= 4096; }
			if(0xF == a && 0xF == b && 0xF == c) {
				for(INT iy = y, iyq = y + q; iy < iyq; ++iy) {
					for(INT ix = x, ixq = x + q; ix < ixq; ++ix) {
						int tx = 63 - (ix % 64);

						dBitsX[ix] = tBitsX[tx];
					}

					dBitsX += width;
					tBitsX += 64;
					if(tBitsX >= tBits + 4096) { tBitsX -= 4096; }
				}
			} else {
				for(INT iy = y, iyq = y + q; iy < iyq; ++iy) {
					INT cx1 = cy1 - 1;
					INT cx2 = cy2 - 1;
					INT cx3 = cy3 - 1;

					for(INT ix = x, ixq = x + q; ix < ixq; ++ix) {
						if((cx1 | cx2 | cx3) >= 0) {
							int tx = 63 - (ix % 64);
							
							dBitsX[ix] = tBitsX[tx];
						}

						cx1 -= fdy12;
						cx2 -= fdy23;
						cx3 -= fdy31;
					}

					cy1 += fdx12;
					cy2 += fdx23;
					cy3 += fdx31;

					dBitsX += width;
					tBitsX += 64;
					if(tBitsX >= tBits + 4096) { tBitsX -= 4096; }
				}
			}
		}

		dBits += q * width;
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