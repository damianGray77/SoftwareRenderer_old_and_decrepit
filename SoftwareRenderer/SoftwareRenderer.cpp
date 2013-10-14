#include "stdafx.h"

//FLOAT SoftwareRenderer::fov;
//FLOAT SoftwareRenderer::dist;
//LONG SoftwareRenderer::vOffsets[MAX_HEIGHT];
//Buffer SoftwareRenderer::mBuffer;
//HDC SoftwareRenderer::mDeviceContext = NULL;
//Matrix4x4 SoftwareRenderer::mWorld;
//HScan *SoftwareRenderer::mHScan;
//VScan SoftwareRenderer::mVScan;

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
	
	dist = cotFOV * mBuffer.GetMidHeight();
}

VOID SoftwareRenderer::Project2(Vertex2c &sCoords, const Vertex3 &coords, DWORD color) {
	if(coords.z == 0) {
		return;
	}

	FLOAT z = INVERSE(coords.z) * dist;

	sCoords.x = (coords.x * z) + mBuffer.GetMidWidth();
	sCoords.y = (-coords.y * z) + mBuffer.GetMidHeight();

	sCoords.color = color;
}

VOID SoftwareRenderer::Project3(Vertex3c &sCoords, const Vertex3 &coords, DWORD color) {
	if(0 == coords.z) {
		return;
	}

	FLOAT z = INVERSE(coords.z) * dist;

	sCoords.x = (coords.x * z) + mBuffer.GetMidWidth();
	sCoords.y = (-coords.y * z) + mBuffer.GetMidHeight();
	sCoords.z = coords.z;

	sCoords.color = color;
}

VOID SoftwareRenderer::Project4(const INT i) {
	Vertex3c &sCoords = player.vertices[i].posProj;
	Vertex3 &coords = player.vertices[i].posTrans;
	const Vertex2 &uvMap = player.vertices[i].uvMap;
	DWORD color = player.vertices[i].color;
	
	coords = player.vertices[i].position * mWorld;
	
	if(0 == coords.z) {
		return;
	}

	FLOAT z = INVERSE(coords.z) * dist;

	sCoords.x = (coords.x * z) + mBuffer.GetMidWidth();
	sCoords.y = (-coords.y * z) + mBuffer.GetMidHeight();
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

	for(INT i = 0; i < player.numFaces; i++) {
		/*FLOAT dot = Vector3::Dot(g_Camera.Position(), player.polygons[i].normal * mWorldRot);
		if(dot > 0.0f) {
			continue;
		}*/

		i1 = player.polygons[i].verts[0];
		i2 = player.polygons[i].verts[1];
		i3 = player.polygons[i].verts[2];
		
		Project4(i1);
		Project4(i2);
		Project4(i3);

		/*player.vertices[i1].posTrans = player.vertices[i1].position * mWorld;
		player.vertices[i2].posTrans = player.vertices[i2].position * mWorld;
		player.vertices[i3].posTrans = player.vertices[i3].position * mWorld;
		
		Project3(player.vertices[i1].posProj, player.vertices[i1].posTrans, player.vertices[i1].color);
		Project3(player.vertices[i2].posProj, player.vertices[i2].posTrans, player.vertices[i2].color);
		Project3(player.vertices[i3].posProj, player.vertices[i3].posTrans, player.vertices[i3].color);*/
		
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
		FLOAT *zBits = (FLOAT*)zBuffer.GetBits();
		DWORD *sBits = (DWORD*)mBuffer.GetBits();
		
		Triangle t;

		for(INT i = 0; i < player.numVisible; ++i) {
			poly = player.visible[i];
			
			t.v1 = player.vertices[player.polygons[poly].verts[0]].posProj;
			t.v2 = player.vertices[player.polygons[poly].verts[1]].posProj;
			t.v3 = player.vertices[player.polygons[poly].verts[2]].posProj;
			t.texId = player.textures[0].textureId;
			
			//Project3(sv1, player.vertices[i1].posTrans, player.vertices[i1].color);
			//Project3(sv2, player.vertices[i2].posTrans, player.vertices[i2].color);
			//Project3(sv3, player.vertices[i3].posTrans, player.vertices[i3].color);
			
			//FLOAT area = (sv1.x * sv2.y - sv2.x * sv1.y) + (sv2.x * sv3.y - sv3.x * sv2.y) + (sv3.x * sv1.y - sv1.x * sv3.y);
			//if(area > 0) {
			//	continue;
			//}
			
			DrawScanLineTriangle(t);

			//DrawTriangle(sBits, zBits, player.vertices[i1].posProj, player.vertices[i2].posProj, player.vertices[i3].posProj);
			
	/*		
			DrawLine(sv1, sv2);
			DrawLine(sv2, sv3);
			DrawLine(sv3, sv1);
			
			Vertex2c sv, sn;
			Vertex3 av = {
				(player.vertices[i1].posTrans.x + player.vertices[i2].posTrans.x + player.vertices[i3].posTrans.x) / 3.0f
				, (player.vertices[i1].posTrans.y + player.vertices[i2].posTrans.y + player.vertices[i3].posTrans.y) / 3.0f
				, (player.vertices[i1].posTrans.z + player.vertices[i2].posTrans.z + player.vertices[i3].posTrans.z) / 3.0f
			};
			Vector3 n = player.polygons[poly].normal * mWorldRot;
			Vertex3 an = { av.x + n.x, av.y + n.y, av.z + n.z };

			Project2(sv, av, 0xffff0000);
			Project2(sn, an, 0xffffffff);
			
			DrawLine(sv, sn);
	*/
		}
	}

	SwapBuffers();

	return S_OK;
}

HRESULT SoftwareRenderer::SetupGeometry() {
	player.numFaces = sizeof(indices) / sizeof(DWORD) / 3;
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

	FLOAT *zBits = (FLOAT *)zBuffer.GetBits();
	DWORD *dBits = (DWORD *)mBuffer.GetBits();
	DWORD *tBits = (DWORD *)textures[t.texId] -> data;
	
	const INT tWidth = textures[t.texId] -> infoHeader.biWidth;
	const INT tHeight = textures[t.texId] -> infoHeader.biHeight;
	
	const FLOAT x1 = t.v1.x, y1 = t.v1.y, z1 = t.v1.z, u1 = t.v1.u * tWidth, v1 = t.v1.v * tHeight;
	const FLOAT x2 = t.v2.x, y2 = t.v2.y, z2 = t.v2.z, u2 = t.v2.u * tWidth, v2 = t.v2.v * tHeight;
	const FLOAT x3 = t.v3.x, y3 = t.v3.y, z3 = t.v3.z, u3 = t.v3.u * tWidth, v3 = t.v3.v * tHeight;
	
	const FLOAT dy1 = INVERSE(y3 - y1);
	FLOAT sx1 = (x3 - x1) * dy1;
	FLOAT sz1 = (z3 - z1) * dy1;
	FLOAT su1 = (u3 - u1) * dy1;
	FLOAT sv1 = (v3 - v1) * dy1;
	
	FLOAT xx1 = x1, zz1 = z1;
	FLOAT uu1 = u1, vv1 = v1;
	
	if(y2 != y1) {
		const FLOAT dy2 = INVERSE(y2 - y1);
		FLOAT sx2 = (x2 - x1) * dy2;
		FLOAT sz2 = (z2 - z1) * dy2;
		FLOAT su2 = (u2 - u1) * dy2;
		FLOAT sv2 = (v2 - v1) * dy2;
		
		FLOAT xx2 = x1, zz2 = z1;
		FLOAT uu2 = u2, vv2 = v2;

		INT startY = iRound(ceil(y1));
		INT endY = iRound(ceil(y2)) - 1;
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
		
		if(sx2 < sx1) {
			SWAP(FLOAT, xx1, xx2);
			SWAP(FLOAT, sx1, sx2);
			
			SWAP(FLOAT, zz1, zz2);
			SWAP(FLOAT, sz1, sz2);
			
			SWAP(FLOAT, uu1, uu2);
			SWAP(FLOAT, su1, su2);
			
			SWAP(FLOAT, vv1, vv2);
			SWAP(FLOAT, sv1, sv2);
			
			swapX = TRUE;
		}
		
		for(INT y = startY; y <= endY; ++y) {
			const FLOAT dx = INVERSE(xx2 - xx1);
			
			FLOAT szx = (zz1 - zz2) * dx;
			FLOAT sux = (uu1 - uu2) * dx;
			FLOAT svx = (vv1 - vv2) * dx;
			
			FLOAT zx = zz2;
			FLOAT ux = uu2;
			FLOAT vx = vv2;

			INT startX = iRound(ceil(xx1));
			INT endX = iRound(ceil(xx2)) - 1;
			FLOAT subX = (FLOAT)startX - xx1;
			
			if(startX < 0) {
				subX -= startX;
				startX = 0;
			}
			if(endX >= width) {
				endX = width - 1;
			}
			endX -= startX;
			
			zx += szx * subX;
			ux += sux * subX;
			vx += svx * subX;
			
			LONG offset = (y * width) + startX;
			FLOAT *zBitsX = zBits + offset;
			DWORD *dBitsX = dBits + offset;
			
			for(INT x = 0; x <= endX; ++x) {
				if(zx > zBitsX[x]) {
					zBitsX[x] = zx;
					LONG tOffset = ((INT)ux % tWidth) + (tWidth * ((INT)vx % tHeight));
					dBitsX[x] = tBits[tOffset];
				}
				
				zx += szx;
				ux += sux;
				vx += svx;
			}

			xx1 += sx1; xx2 += sx2;
			zz1 += sz1; zz2 += sz2;
			uu1 += su1; uu2 += su2;
			vv1 += sv1; vv2 += sv2;
		}
		
		if(TRUE == swapX) {
			SWAP(FLOAT, xx1, xx2);
			SWAP(FLOAT, sx1, sx2);
			
			SWAP(FLOAT, zz1, zz2);
			SWAP(FLOAT, sz1, sz2);
			
			SWAP(FLOAT, uu1, uu2);
			SWAP(FLOAT, su1, su2);
			
			SWAP(FLOAT, vv1, vv2);
			SWAP(FLOAT, sv1, sv2);
		}
	}

	if(y3 != y2) {
		const FLOAT dy3 = INVERSE(y3 - y2);
		FLOAT sx2 = (x3 - x2) * dy3;
		FLOAT sz2 = (z3 - z2) * dy3;
		FLOAT su2 = (u3 - u2) * dy3;
		FLOAT sv2 = (v3 - v2) * dy3;
		
		FLOAT xx2 = x2, zz2 = z2;
		FLOAT uu2 = u2, vv2 = v2;

		INT startY = iRound(ceil(y2));
		INT endY = iRound(ceil(y3)) - 1;
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
		}
		
		xx1 += sx1 * subY; xx2 += sx2 * subY;
		zz1 += sz1 * subY; zz2 += sz2 * subY;
		uu1 += su1 * subY; uu2 += su2 * subY;
		vv1 += sv1 * subY; vv2 += sv2 * subY;
		
		if(TRUE == swapX) {
			SWAP(FLOAT, xx1, xx2);
			SWAP(FLOAT, sx1, sx2);
			
			SWAP(FLOAT, zz1, zz2);
			SWAP(FLOAT, sz1, sz2);
			
			SWAP(FLOAT, uu1, uu2);
			SWAP(FLOAT, su1, su2);
			
			SWAP(FLOAT, vv1, vv2);
			SWAP(FLOAT, sv1, sv2);
		}

		for(INT y = startY; y <= endY; ++y) {
			const FLOAT dx = INVERSE(xx2 - xx1);
			
			FLOAT szx = (zz1 - zz2) * dx;
			FLOAT sux = (uu1 - uu2) * dx;
			FLOAT svx = (vv1 - vv2) * dx;
			
			FLOAT zx = zz2;
			FLOAT ux = uu2;
			FLOAT vx = vv2;
			
			INT startX = iRound(ceil(xx1));
			INT endX = iRound(ceil(xx2)) - 1;
			FLOAT subX = (FLOAT)startX - xx1;
			
			if(startX < 0) {
				subX -= startX;
				startX = 0;
			}
			if(endX >= width) {
				endX = width - 1;
			}
			endX -= startX;
			
			zx += szx * subX;
			ux += sux * subX;
			vx += svx * subX;
			
			LONG offset = (y * width) + startX;
			FLOAT *zBitsX = zBits + offset;
			DWORD *dBitsX = dBits + offset;
			
			for(INT x = 0; x <= endX; ++x) {
				if(zx > zBitsX[x]) {
					zBitsX[x] = zx;
					LONG tOffset = ((INT)ux % tWidth) + (tWidth * ((INT)vx % tHeight));
					dBitsX[x] = tBits[tOffset];
				}

				zx += szx;
				ux += sux;
				vx += svx;
			}

			xx1 += sx1; xx2 += sx2;
			zz1 += sz1; zz2 += sz2;
			uu1 += uu1; uu2 += uu2;
			vv1 += vv1; vv2 += vv2;
		}
	}
}

/*VOID SoftwareRenderer::DrawScanLineTriangle(Triangle &t) {
	INT startY, endY;
	INT startX, endX;
	FLOAT subY, subX;
	
	FLOAT xx1, xx2;
	FLOAT dx;
	
	if(t.v1.y > t.v2.y) { SWAP(Vertex3c, t.v1, t.v2); }
	if(t.v1.y > t.v3.y) { SWAP(Vertex3c, t.v1, t.v3); }
	if(t.v2.y > t.v3.y) { SWAP(Vertex3c, t.v2, t.v3); }
	
	if(t.v3.y <= t.v1.y) {
		return;
	}

	DWORD *dBitsX, *dBitsY;

	DWORD *dBits = (DWORD *)mBuffer.GetBits();
	
	const FLOAT x1 = t.v1.x, y1 = t.v1.y;
	const FLOAT x2 = t.v2.x, y2 = t.v2.y;
	const FLOAT x3 = t.v3.x, y3 = t.v3.y;
	
	const FLOAT dy1 = y3 - y1;
	const FLOAT sx1 = (x3 - x1) / dy1;
	
	if(y1 != y2) {
		const FLOAT dy2 = y2 - y1;
		const FLOAT sx2 = (x2 - x1) / dy2;
		
		xx1 = x1;
		xx2 = x1;
		
		startY = iRound(ceil(y1));
		endY = iRound(ceil(y2)) - 1;
		subY = (FLOAT)startY - y1;
		
		if(startY < 0) {
			subY -= startY;
			startY = 0;
		}
		if(endY >= height) {
			endY = height - 1;
		}
		endY -= startY;
		
		dBitsY = dBits + width * startY;
		
		xx1 += sx1 * subY;
		xx2 += sx2 * subY;
		
		if(xx2 < xx1 || xx2 + (endY * sx2) < xx1 + (endY * sx1)) {
			SWAP(FLOAT, xx1, xx2);
			SWAP(FLOAT, sx1, sx2);
		}

		for(INT y = 0; y <= endY; ++y) {
			startX = iRound(ceil(xx1));
			endX = iRound(ceil(xx2)) - 1;
			subX = (FLOAT)startX - xx1;
			
			if(startX < 0) {
				subX -= startX;
				startX = 0;
			}
			if(endX >= width) {
				endX = width - 1;
			}
			endX -= startX;

			dBitsX = dBitsY + startX;

			for(INT x = 0; x <= endX; ++x) {
				dBitsX[x] = 0x00007F00;
			}

			xx1 += sx1;
			xx2 += sx2;

			dBitsY += width;
		}
	}
	
	if(y3 != y2) {
		const FLOAT dy3 = y3 - y2;
		const FLOAT sx3 = (x3 - x2) / dy3;

		xx1 = x1;
		xx2 = x2;
		
		startY = iRound(ceil(y2));
		endY = iRound(ceil(y3)) - 1;
		subY = (FLOAT)startY - y2;

		if(startY < 0) {
			subY -= startY;
			startY = 0;
		}
		if(endY >= height) {
			endY = height - 1;
		}
		endY -= startY;
		
		if(y1 != y2) {
			xx1 += sx1 * (y2 - y1);
		} else {
			dBitsY = dBits + width * startY;
		}
		
		xx1 += sx1 * subY;
		xx2 += sx3 * subY;
		
		if(xx2 < xx1 || xx2 + (endY * sx3) < xx1 + (endY * sx1)) {
			SWAP(FLOAT, xx1, xx2);
			SWAP(FLOAT, sx1, sx3);
		}

		for(INT y = 0; y <= endY; ++y) {
			startX = iRound(ceil(xx1));
			endX = iRound(ceil(xx2)) - 1;
			subX = (FLOAT)startX - xx1;
			
			if(startX < 0) {
				subX -= startX;
				startX = 0;
			}
			if(endX >= width) {
				endX = width - 1;
			}
			endX -= startX;
			
			dBitsX = dBitsY + startX;
			
			for(INT x = 0; x <= endX; ++x) {
				dBitsX[x] = 0x00007F00;
			}

			xx1 += sx1;
			xx2 += sx3;

			dBitsY += width;
		}
	}
}
*/
VOID SoftwareRenderer::DrawHalfSpaceTriangle(Triangle &t) {
	FLOAT *zBits = (FLOAT *)zBuffer.GetBits();
	DWORD *dBits = (DWORD *)mBuffer.GetBits();
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

/*
VOID SoftwareRenderer::DrawLine(Vertex2c &v1, Vertex2c &v2) {
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
}

VOID SoftwareRenderer::DrawTriangle(Vertex2c &v1, Vertex2c &v2, Vertex2c &v3) {
	Vertex2c v[3];
	FLOAT sub;
	LONG y1, y2;
	Color4f c[3];

	v[0] = v1;
	v[1] = v2;
	v[2] = v3;

	if(v[0].y > v[1].y) {
		SWAP(Vertex2c, v[0], v[1]);
	}
	if(v[0].y > v[2].y) {
		SWAP(Vertex2c, v[0], v[2]);
	}
	if(v[1].y > v[2].y) {
		SWAP(Vertex2c, v[1], v[2]);
	}

	c[0] = v[0].color;
	c[1] = v[1].color;
	c[2] = v[2].color;

	scanData2.invDeltaY[0] = v[2].y - v[0].y;
	scanData2.invDeltaY[1] = v[1].y - v[0].y;
	scanData2.invDeltaY[2] = v[2].y - v[1].y;

	if(scanData2.invDeltaY[0] >= FLT_MARGIN) {
		scanData2.invDeltaY[0] = INVERSE(scanData2.invDeltaY[0]);
	}

	scanData2.slopeX[0] = (v[2].x - v[0].x) * scanData2.invDeltaY[0];
	scanData2.slopeC[0] = (c[2] - c[0]) * scanData2.invDeltaY[0];
	scanData2.x[0] = v[0].x;
	scanData2.c[0] = c[0];

	if(scanData2.invDeltaY[1] >= FLT_MARGIN) {
		scanData2.invDeltaY[1] = INVERSE(scanData2.invDeltaY[1]);

		scanData2.slopeX[1] = (v[1].x - v[0].x) * scanData2.invDeltaY[1];
		scanData2.slopeC[1] = (c[1] - c[0]) * scanData2.invDeltaY[1];
		scanData2.x[1] = v[0].x;
		scanData2.c[1] = c[0];

		y1 = (LONG)ceil(v[0].y);
		y2 = (LONG)ceil(v[1].y) - 1;
		sub = (FLOAT)y1 - v[0].y;

		if(y1 < 0) {
			sub -= y1;
			y1 = 0;
		}
		if(y2 >= (INT)props.screenHeight) {
			y2 = props.screenHeight - 1;
		}

		scanData2.x[0] += scanData2.slopeX[0] * sub;
		scanData2.c[0] += scanData2.slopeC[0] * sub;
		
		scanData2.x[1] += scanData2.slopeX[1] * sub;
		scanData2.c[1] += scanData2.slopeC[1] * sub;

		for(INT y = y1; y <= y2; y++) {
			scanLine2.y = y;
			scanLine2.x[0] = scanData2.x[0];
			scanLine2.x[1] = scanData2.x[1];

			scanLine2.c[0] = scanData2.c[0];
			scanLine2.c[1] = scanData2.c[1];

			ScanLine(scanLine2);

			scanData2.x[0] += scanData2.slopeX[0];
			scanData2.c[0] += scanData2.slopeC[0];
		
			scanData2.x[1] += scanData2.slopeX[1];
			scanData2.c[1] += scanData2.slopeC[1];
		}
	}

	if(scanData2.invDeltaY[2] >= FLT_MARGIN) {
		scanData2.invDeltaY[2] = INVERSE(scanData2.invDeltaY[2]);

		if(scanData2.invDeltaY[1] >= FLT_MARGIN) {
			FLOAT dy = v[1].y - v[0].y;

			scanData2.x[0] = v[0].x + (scanData2.slopeX[0] * dy);
			scanData2.c[0] = c[0] + (scanData2.slopeC[0] * dy);
		}

		scanData2.slopeX[1] = (v[2].x - v[1].x) * scanData2.invDeltaY[2];
		scanData2.slopeC[1] = (c[2] - c[1]) * scanData2.invDeltaY[2];
		scanData2.x[1] = v[1].x;
		scanData2.c[1] = c[1];

		y1 = (LONG)ceil(v[1].y);
		y2 = (LONG)ceil(v[2].y) - 1;
		sub = (FLOAT)y1 - v[1].y;

		if(y1 < 0) {
			sub -= y1;
			y1 = 0;
		}
		if(y2 >= (INT)props.screenHeight) {
			y2 = props.screenHeight - 1;
		}

		scanData2.x[0] += scanData2.slopeX[0] * sub;
		scanData2.c[0] += scanData2.slopeC[0] * sub;

		scanData2.x[1] += scanData2.slopeX[1] * sub;
		scanData2.c[1] += scanData2.slopeC[1] * sub;

		for(INT y = y1; y <= y2; y++) {
			scanLine2.y = y;
			scanLine2.x[0] = scanData2.x[0];
			scanLine2.x[1] = scanData2.x[1];
			
			scanLine2.c[0] = scanData2.c[0];
			scanLine2.c[1] = scanData2.c[1];
			
			ScanLine(scanLine2);

			scanData2.x[0] += scanData2.slopeX[0];
			scanData2.c[0] += scanData2.slopeC[0];
		
			scanData2.x[1] += scanData2.slopeX[1];
			scanData2.c[1] += scanData2.slopeC[1];
		}
	}
}

VOID SoftwareRenderer::ScanLine(ScanLine2 &data) {
	DWORD *bits, *cBits;
	FLOAT invDeltaX, sub;
	Color4f slopeC;
	LONG x1, x2;

	if(data.x[1] < data.x[0]) {
		SWAP(FLOAT, data.x[0], data.x[1]);
		SWAP(Color4f, data.c[0], data.c[1]);
	}

	invDeltaX = data.x[1] - data.x[0];

	if(invDeltaX >= FLT_MARGIN) {
		invDeltaX = INVERSE(invDeltaX);
		slopeC = (data.c[1] - data.c[0]) * invDeltaX;
	}

	x1 = (LONG)ceil(data.x[0]);
	x2 = (LONG)ceil(data.x[1]) - 1;
	sub = (FLOAT)x1 - data.x[0];

	if(x1 < 0) {
		sub -= x1;
		x1 = 0;
	}
	if(x2 >= (INT)props.screenWidth) {
		x2 = props.screenWidth - 1;
	}

	data.c[0] += slopeC * sub;

	bits = (DWORD*)mBuffer.GetBits() + (data.y * mBuffer.GetWidth()) + x1;
	cBits = (DWORD*)cBuffer.GetBits() + (data.y * cBuffer.GetWidth()) + x1;
	for(INT x = x1; x <= x2; x++) {
		if(0x0 == *cBits) {
			*bits = RGBAColor4f(data.c[0]);
			*cBits = 0xffffffff;
		}
		*cBits++;
		*bits++;
		data.c[0] += slopeC;
	}
}



VOID SoftwareRenderer::DrawTriangle(DWORD *sBits, FLOAT *zBits, const Vertex3c &v1, const Vertex3c &v2, const Vertex3c &v3) {
	Vertex3c v[3];
	FLOAT x1, x2, x3, y1, y2, y3, z1, z2, z3;
	//FLOAT r1, r2, r3, g1, g2, g3, b1, b2, b3, a1, a2, a3;
	FLOAT xx1, xx2, zz1, zz2;
	//FLOAT rr1, rr2, gg1, gg2, bb1, bb2, aa1, aa2;
	FLOAT xxx1, xxx2;
	FLOAT z; //, r, g, b, a;
	FLOAT sx1, sx2, sz1, sz2;
	//FLOAT sr1, sr2, sg1, sg2, sb1, sb2, sa1, sa2;
	FLOAT sz, sr, sg, sb, sa;
	FLOAT dy1, dy2, dy3, dx;
	LONG startY, endY, startX, endX;
	FLOAT subX, subY;
	FLOAT *zBitsX, *zBitsY;
	DWORD *sBitsX, *sBitsY;
	LONG width, height;
	
	width = mBuffer.GetWidth();
	height = mBuffer.GetHeight();
	
	v[0] = v1;
	v[1] = v2;
	v[2] = v3;

	if(v[0].y > v[1].y) {
		SWAP(Vertex3c, v[0], v[1]);
	}
	if(v[0].y > v[2].y) {
		SWAP(Vertex3c, v[0], v[2]);
	}
	if(v[1].y > v[2].y) {
		SWAP(Vertex3c, v[1], v[2]);
	}

	x1 = v[0].x;
	x2 = v[1].x;
	x3 = v[2].x;
	
	y1 = v[0].y;
	y2 = v[1].y;
	y3 = v[2].y;
	
	z1 = v[0].z;
	z2 = v[1].z;
	z3 = v[2].z;
	
	r1 = (FLOAT)RB(v[0].color);
	r2 = (FLOAT)RB(v[1].color);
	r3 = (FLOAT)RB(v[2].color);
	
	g1 = (FLOAT)GB(v[0].color);
	g2 = (FLOAT)GB(v[1].color);
	g3 = (FLOAT)GB(v[2].color);
	
	b1 = (FLOAT)BB(v[0].color);
	b2 = (FLOAT)BB(v[1].color);
	b3 = (FLOAT)BB(v[2].color);
	
	a1 = (FLOAT)AB(v[0].color);
	a2 = (FLOAT)AB(v[1].color);
	a3 = (FLOAT)AB(v[2].color);
	
	dy1 = y3 - y1;
	dy2 = y2 - y1;
	dy3 = y3 - y2;

	if(dy1 != 0.0f) {
		dy1 = INVERSE(dy1);
	}

	sx1 = (x3 - x1) * dy1;
	sz1 = (z3 - z1) * dy1;
	sr1 = (r3 - r1) * dy1;
	sg1 = (g3 - g1) * dy1;
	sb1 = (b3 - b1) * dy1;
	sa1 = (a3 - a1) * dy1;
	
	xx1 = x1;
	zz1 = z1;
	rr1 = r1;
	gg1 = g1;
	bb1 = b1;
	aa1 = a1;

	if(dy2 != 0.0f) {
		dy2 = INVERSE(dy2);

		sx2 = (x2 - x1) * dy2;
		sz2 = (z2 - z1) * dy2;
		sr2 = (r2 - r1) * dy2;
		sg2 = (g2 - g1) * dy2;
		sb2 = (b2 - b1) * dy2;
		sa2 = (a2 - a1) * dy2;
		
		xx2 = x1;
		zz2 = z1;
		rr2 = r1;
		gg2 = g1;
		bb2 = b1;
		aa2 = a1;

		startY = (LONG)ceil(y1);
		endY = (LONG)ceil(y2) - 1;
		subY = (FLOAT)startY - y1;

		if(startY < 0) {
			subY -= startY;
			startY = 0;
		}
		if(endY >= height) {
			endY = height - 1;
		}
		
		zBitsY = zBits + (width * startY);
		sBitsY = sBits + (width * startY);

		xx1 += sx1 * subY;
		xx2 += sx2 * subY;

		zz1 += sz1 * subY;
		zz2 += sz2 * subY;
		
		rr1 += sr1 * subY;
		rr2 += sr2 * subY;
		
		gg1 += sg1 * subY;
		gg2 += sg2 * subY;
		
		bb1 += sb1 * subY;
		bb2 += sb2 * subY;
		
		aa1 += sa1 * subY;
		aa2 += sa2 * subY;

		for(LONG y = startY; y <= endY; ++y) {
			if(xx2 < xx1) {
				xxx1 = xx2; xxx2 = xx1;
				
				dx = INVERSE(xxx2 - xxx1);
				sz = (zz1 - zz2) * dx;
				sr = (rr1 - rr2) * dx;
				sg = (gg1 - gg2) * dx;
				sb = (bb1 - bb2) * dx;
				sa = (aa1 - aa2) * dx;
				
				z = zz2;
				r = rr2;
				g = gg2;
				b = bb2;
				a = aa2;
			} else {
				xxx1 = xx1; xxx2 = xx2;
				
				dx = INVERSE(xxx2 - xxx1);
				sz = (zz2 - zz1) * dx;
				sr = (rr2 - rr1) * dx;
				sg = (gg2 - gg1) * dx;
				sb = (bb2 - bb1) * dx;
				sa = (aa2 - aa1) * dx;
				
				z = zz1;
				r = rr1;
				g = gg1;
				b = bb1;
				a = aa1;
			}

			startX = (LONG)ceil(xxx1);
			endX = (LONG)ceil(xxx2) - 1;
			subX = (FLOAT)startX - xxx1;
			
			if(startX < 0) {
				subX -= startX;
				startX = 0;
			}
			if(endX >= width) {
				endX = width - 1;
			}
			
			z += sz * subX;
			r += sr * subX;
			g += sg * subX;
			b += sb * subX;
			a += sa * subX;
			
			sBitsX = sBitsY + startX;
			zBitsX = zBitsY + startX;

			for(LONG x = startX; x <= endX; ++x) {
				if(z > *zBitsX) {
					*zBitsX = z;
					*sBitsX = RGBAb((BYTE)r, (BYTE)g, (BYTE)b, (BYTE)a);
				}
				
				z += sz;
				r += sr;
				g += sg;
				b += sb;
				a += sa;
				
				++zBitsX;
				++sBitsX;
			}

			xx1 += sx1;
			xx2 += sx2;

			zz1 += sz1;
			zz2 += sz2;
			
			rr1 += sr1;
			rr2 += sr2;
			
			gg1 += sg1;
			gg2 += sg2;
			
			bb1 += sb1;
			bb2 += sb2;
			
			aa1 += sa1;
			aa2 += sa2;
			
			zBitsY += width;
			sBitsY += width;
		}
	}

	if(dy3 != 0.0f) {
		dy3 = INVERSE(dy3);

		sx2 = (x3 - x2) * dy3;
		sz2 = (z3 - z2) * dy3;
		sr2 = (r3 - r2) * dy3;
		sg2 = (g3 - g2) * dy3;
		sb2 = (b3 - b2) * dy3;
		sa2 = (a3 - a2) * dy3;
		
		xx2 = x2;
		zz2 = z2;
		rr2 = r2;
		gg2 = g2;
		bb2 = b2;
		aa2 = a2;

		startY = (LONG)ceil(y2);
		endY = (LONG)ceil(y3) - 1;
		subY = (FLOAT)startY - y2;

		if(startY < 0) {
			subY -= startY;
			startY = 0;
		}
		if(endY >= height) {
			endY = height - 1;
		}
		
		if(dy2 != 0.0f) {
			FLOAT dy = y2 - y1;

			xx1 = x1 + (sx1 * dy);
			zz1 = z1 + (sz1 * dy);
			rr1 = r1 + (sr1 * dy);
			gg1 = g1 + (sg1 * dy);
			bb1 = b1 + (sb1 * dy);
			aa1 = a1 + (sa1 * dy);
		} else {
			zBitsY = zBits + (width * startY);
			sBitsY = sBits + (width * startY);
		}
		
		xx1 += sx1 * subY;
		xx2 += sx2 * subY;

		zz1 += sz1 * subY;
		zz2 += sz2 * subY;
		
		rr1 += sr1 * subY;
		rr2 += sr2 * subY;
		
		gg1 += sg1 * subY;
		gg2 += sg2 * subY;
		
		bb1 += sb1 * subY;
		bb2 += sb2 * subY;
		
		aa1 += sa1 * subY;
		aa2 += sa2 * subY;

		for(LONG y = startY; y <= endY; ++y) {
			if(xx2 < xx1) {
				xxx1 = xx2; xxx2 = xx1;
				
				dx = INVERSE(xxx2 - xxx1);
				sz = (zz1 - zz2) * dx;
				sr = (rr1 - rr2) * dx;
				sg = (gg1 - gg2) * dx;
				sb = (bb1 - bb2) * dx;
				sa = (aa1 - aa2) * dx;
				
				z = zz2;
				r = rr2;
				g = gg2;
				b = bb2;
				a = aa2;
			} else {
				xxx1 = xx1; xxx2 = xx2;
				
				dx = INVERSE(xxx2 - xxx1);
				sz = (zz2 - zz1) * dx;
				sr = (rr2 - rr1) * dx;
				sg = (gg2 - gg1) * dx;
				sb = (bb2 - bb1) * dx;
				sa = (aa2 - aa1) * dx;
				
				z = zz1;
				r = rr1;
				g = gg1;
				b = bb1;
				a = aa1;
			}
			
			startX = (LONG)ceil(xxx1);
			endX = (LONG)ceil(xxx2) - 1;
			subX = (FLOAT)startX - xxx1;
			
			if(startX < 0) {
				subX -= startX;
				startX = 0;
			}
			if(endX >= width) {
				endX = width - 1;
			}
			
			z += sz * subX;
			r += sr * subX;
			g += sg * subX;
			b += sb * subX;
			a += sa * subX;
			
			zBitsX = zBitsY + startX;
			sBitsX = sBitsY + startX;
			
			for(LONG x = startX; x <= endX; ++x) {
				if(z > *zBitsX) {
					*zBitsX = z;
					*sBitsX = RGBAb((BYTE)r, (BYTE)g, (BYTE)b, (BYTE)a);
				}

				z += sz;
				r += sr;
				g += sg;
				b += sb;
				a += sa;
				
				++zBitsX;
				++sBitsX;
			}

			xx1 += sx1;
			xx2 += sx2;

			zz1 += sz1;
			zz2 += sz2;
			
			rr1 += sr1;
			rr2 += sr2;
			
			gg1 += sg1;
			gg2 += sg2;
			
			bb1 += sb1;
			bb2 += sb2;
			
			aa1 += sa1;
			aa2 += sa2;
			
			zBitsY += width;
			sBitsY += width;
		}
	}
}

VOID SoftwareRenderer::DrawTriangle2(DWORD* sBits, FLOAT* zBits, const Vertex3c &v1, const Vertex3c &v2, const Vertex3c &v3) {
	const INT Y1 = iRound(16.0f * v1.y);
	const INT Y2 = iRound(16.0f * v2.y);
	const INT Y3 = iRound(16.0f * v3.y);

	const INT X1 = iRound(16.0f * v1.x);
	const INT X2 = iRound(16.0f * v2.x);
	const INT X3 = iRound(16.0f * v3.x);
	
	const LONG width = (LONG)props.screenWidth;
	const LONG height = (LONG)props.screenHeight;
	
	const INT q = 8;
	
	const INT minx = MAX(0, (MIN(X1, MIN(X2, X3)) + 0xF) >> 4) & ~(q - 1);
	const INT maxx = MIN(width, (MAX(X1, MAX(X2, X3)) + 0xF) >> 4);
	const INT miny = MAX(0, (MIN(Y1, MIN(Y2, Y3)) + 0xF) >> 4) & ~(q - 1);
	const INT maxy = MIN(height, (MAX(Y1, MAX(Y2, Y3)) + 0xF) >> 4);
	
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
	
	sBits += miny * width;
	
	INT c1 = dy12 * X1 - dx12 * Y1;
	INT c2 = dy23 * X2 - dx23 * Y2;
	INT c3 = dy31 * X3 - dx31 * Y3;
	
	if(dy12 < 0 || (dy12 == 0 && dx12 > 0)) {
		c1++;
	}
	if(dy23 < 0 || (dy23 == 0 && dx23 > 0)) {
		c2++;
	}
	if(dy31 < 0 || (dy31 == 0 && dx31 > 0)) {
		c3++;
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
			
			if(a == 0x0) {
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
			
			if(b == 0x0) {
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
			
			if(c == 0x0) {
				continue;
			}
			
			DWORD *sBitsX = sBits;
			
			if(a == 0xF && b == 0xF && c == 0xF) {
				for(INT iy = 0; iy < q; iy++) {
					for(INT ix = x; ix < x + q; ix += 4) {
						sBitsX[ix] = 0x00007F00; // Green
						sBitsX[ix + 1] = 0x00007F00; // Green
						sBitsX[ix + 2] = 0x00007F00; // Green
						sBitsX[ix + 3] = 0x00007F00; // Green
					}

				sBitsX += width;
				}
			} else {
				for(INT iy = 0; iy < q; iy++) {
					INT cx1 = cy1;
					INT cx2 = cy2;
					INT cx3 = cy3;

					for(INT ix = x; ix < x + q; ix++) {
						if(cx1 > 0 && cx2 > 0 && cx3 > 0) {
							sBitsX[ix] = 0x0000007F; // Blue
						}

						cx1 -= fdy12;
						cx2 -= fdy23;
						cx3 -= fdy31;
					}

					cy1 += fdx12;
					cy2 += fdx23;
					cy3 += fdx31;

					sBitsX += width;
				}
			}
		}

		sBits += q * width;
	}
}
*/

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