#include "stdafx.h"
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
		|| !CreateBuffer(zBuffer)
	) {
		return E_FAIL;
	}

	SetFov(SINCOSMAX / 16);
	
	numLights = lights_size / sizeof(Light);
	
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

VOID SoftwareRenderer::Project(PVertex3c &v, Matrix4x4 &m) {
	Vertex3 &screen = v.posScreen;
	Vertex3 &world = v.posWorld;

	world = v.position * m;
	
	if(0 == world.z) {
		return;
	}

	const FLOAT z = dist / world.z;

	screen.x = (world.x * z) + mBuffer.mWidth;
	screen.y = (-world.y * z) + mBuffer.mHeight;
	screen.z = world.z;
}

VOID SoftwareRenderer::Clear(DWORD color) {
	//mBuffer.Clear(color);
	zBuffer.Clear(0.0f);
}

HRESULT SoftwareRenderer::Render() {
	// in an actual 'level' there will be no space on the screen without
	// something in it so clearing the draw buffer is an unnecessary step.
	Clear(0xff000000);

	// however, the zBuffer still does need clearing on each frame.
	//zBuffer.Clear(0.0f);

	mWorld = g_Camera.Look();
	
	Matrix4x4::Identity(player.projection);
	Matrix4x4::Rotate(player.projection, player.rotation.x, player.rotation.y, player.rotation.z);
	Matrix4x4::Translate(player.projection, player.position.x, player.position.y, 9.0f);
	
	//player.projection *= mWorld;
	
	player.rotation.x += 5000.0f * g_Camera.moveAdjust; if(player.rotation.x > SINCOSMAX) { player.rotation.x -= SINCOSMAX; }
	player.rotation.y += 2500.0f * g_Camera.moveAdjust; if(player.rotation.y > SINCOSMAX) { player.rotation.y -= SINCOSMAX; }
	player.rotation.z += 1250.0f * g_Camera.moveAdjust; if(player.rotation.z > SINCOSMAX) { player.rotation.z -= SINCOSMAX; }

	player.numVisible = 0;
	INT i1, i2, i3;
	FLOAT biggest;
	INT poly;
	Vertex3 sv1, sv2, sv3;
	FLOAT area;
	
	/*player.CalculateNormals(TRUE);
	
	for(INT i = 0; i < player.numLights; ++i) {
		Project(player.lights[i]);
	}*/
	
	for(INT i = 0; i < player.numFaces; ++i) {
		i1 = player.polygons[i].verts[0];
		i2 = player.polygons[i].verts[1];
		i3 = player.polygons[i].verts[2];
		
		Project(player.vertices[i1], player.projection);
		Project(player.vertices[i2], player.projection);
		Project(player.vertices[i3], player.projection);
		
		sv1 = player.vertices[i1].posScreen;
		sv2 = player.vertices[i2].posScreen;
		sv3 = player.vertices[i3].posScreen;
		
		area = sv1.x * sv2.y - sv2.x * sv1.y;
		area += sv2.x * sv3.y - sv3.x * sv2.y;
		area += sv3.x * sv1.y - sv1.x * sv3.y;
		if(area > 0.0f) {
			continue;
		}

		player.polygons[i].cenZ = (
			player.vertices[i1].posWorld.z
			+ player.vertices[i2].posWorld.z
			+ player.vertices[i3].posWorld.z
		) * 0.333333f;
		player.visible[player.numVisible] = i;
		
		/*for(INT j = 0; j < player.numLights; ++j) {
			CalculatePolygonLight(i, player.lights[j]);
		}*/
		
		++player.numVisible;
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
		Polygon3 p = player.polygons[poly];
		Vertex3 v1 = player.vertices[p.verts[0]].posScreen;
		Vertex3 v2 = player.vertices[p.verts[1]].posScreen;
		Vertex3 v3 = player.vertices[p.verts[2]].posScreen;
		
		Vertex3c vc1 = { v1.x, v1.y, v1.z, p.uvs[0].x, p.uvs[0].y, p.cols[0] };
		Vertex3c vc2 = { v2.x, v2.y, v2.z, p.uvs[1].x, p.uvs[1].y, p.cols[1] };
		Vertex3c vc3 = { v3.x, v3.y, v3.z, p.uvs[2].x, p.uvs[2].y, p.cols[2] };
		
		t.v1 = vc1;
		t.v2 = vc2;
		t.v3 = vc3;
		t.texId = p.texID;
		
		/*const INT minX = MAX(0, (INT)(MIN(MIN(t.v1.x, t.v2.x), t.v3.x)));
		const INT maxX = MIN(width, (INT)(MAX(MAX(t.v1.x, t.v2.x), t.v3.x)));
		const INT minY = MAX(0, (INT)(MIN(MIN(t.v1.y, t.v2.y), t.v3.y)));
		const INT maxY = MIN(height, (INT)(MAX(MAX(t.v1.y, t.v2.y), t.v3.y)));
		
		if(maxX - minX > maxY - minY) {*/
			DrawScanLineTriangle(t);
		/*} else {
			DrawScanLineTriangleX(t);
		}*/
		//DrawScanLineTriangleFlat(t);
		//DrawHalfSpaceTriangle(t);
	}
	
	/*for(INT i = 0; i < player.numLights; ++i) {
		PVertex3c v = player.lights[i];

		Vertex2 v1 = { v.posScreen.x - 10, v.posScreen.y };
		Vertex2 v2 = { v.posScreen.x + 10, v.posScreen.y };
		DrawLine(v1, v2, 0xffffff00);
		
		Vertex2 v3 = { v.posScreen.x, v.posScreen.y - 10 };
		Vertex2 v4 = { v.posScreen.x, v.posScreen.y + 10 };
		DrawLine(v3, v4, 0xffffff00);
	}
	
	for(INT i = 0; i < player.numFaces; ++i) {
		Polygon3 p = player.polygons[i];
		Vertex3 v1 = player.vertices[p.verts[0]].posScreen;
		Vertex3 v2 = player.vertices[p.verts[1]].posScreen;
		Vertex3 v3 = player.vertices[p.verts[2]].posScreen;
		
		Vertex2 v21 = { v1.x, v1.y };
		Vertex2 v22 = { v2.x, v2.y };
		Vertex2 v23 = { v3.x, v3.y };
		
		DrawLine(v21, v22, 0xffffffff);
		DrawLine(v22, v23, 0xffffffff);
		DrawLine(v23, v21, 0xffffffff);
		
		Vertex2 vn1 = {
			(v1.x + v2.x + v3.x) * 0.33333f
			, (v1.y + v2.y + v3.y) * 0.33333f
		};
		
		Vector3 n = player.polygons[i].normal * mWorldInv;
		
		Vertex2 vn2 = {
			vn1.x + n.x * 100
			, vn1.y + n.y * 100
		};
		
		DrawLine(vn1, vn2, 0xffffff00);
	}*/

	SwapBuffers();

	return S_OK;
}

VOID SoftwareRenderer::CalculatePolygonLight(const INT i, const PVertex3c &light) {
	for(INT j = 0; j < 3; ++j) {
		Vector3 vPos = {
			player.vertices[player.polygons[i].verts[j]].posWorld.x
			, player.vertices[player.polygons[i].verts[j]].posWorld.y
			, player.vertices[player.polygons[i].verts[j]].posWorld.z
		};
		
		vPos = Vector3::Normal(vPos);
		
		Vector3 lPos = {
			light.posWorld.x
			, light.posWorld.y
			, light.posWorld.z
		};
		
		lPos = Vector3::Normal(lPos);
		
		Vector3 dir = vPos - lPos;
		FLOAT dot = Vector3::Dot(player.vertices[player.polygons[i].verts[j]].normal, dir);
		FLOAT intensity = MAX(0.0f, dot);
	    
		player.polygons[i].cols[j].r = light.color.r * intensity; 
		player.polygons[i].cols[j].g = light.color.g * intensity;
		player.polygons[i].cols[j].b = light.color.b * intensity;
	}
    //player.vertices[i].color.a = 1.0f;
}

/*FLOAT ComputeNDotL(Vector3 vertex, Vector3 normal, Vector3 lightPosition) 
{
    var lightDirection = lightPosition - vertex;

    normal.Normalize();
    lightDirection.Normalize();

    return Math.Max(0, Vector3.Dot(normal, lightDirection));
}*/

HRESULT SoftwareRenderer::SetupGeometry() {
	player.numFaces = indices_size / sizeof(Polygon3cuv);
	player.numVerts = verticesc_size / sizeof(Vertex3c);
	player.numLights = lights_size / sizeof(Light);
	
	player.lights = new PVertex3c[player.numLights];
	for(INT i = 0; i < player.numLights; ++i) {
		PVertex3c::Copy(player.lights[i], lights[i]);
	}
	
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

VOID SoftwareRenderer::DrawScanLineTriangleFlat(Triangle &t) {
	if(t.v1.y > t.v2.y) { SWAP(Vertex3c, t.v1, t.v2); }
	if(t.v1.y > t.v3.y) { SWAP(Vertex3c, t.v1, t.v3); }
	if(t.v2.y > t.v3.y) { SWAP(Vertex3c, t.v2, t.v3); }
	
	if(t.v3.y <= t.v1.y || t.v3.y <= 0 || t.v1.y >= height) {
		return;
	}

	const FLOAT x1 = t.v1.x, y1 = t.v1.y;
	const FLOAT x2 = t.v2.x, y2 = t.v2.y;
	const FLOAT x3 = t.v3.x, y3 = t.v3.y;

	BOOL swapX = FALSE;

	DWORD *dBitsY, *dBits = (DWORD *)mBuffer.bits;
		
	const FLOAT dy1 = 1.0f / (y3 - y1);
	
	FLOAT sx2, sx1 = (x3 - x1) * dy1;
	FLOAT xx2, xx1;
	
	FLOAT subX, subY;
	INT startX, startY;
	INT endX, endY;
	
	if(y2 != y1) {
		endY = (y2 >= height
			? height
			: (INT)ceil(y2)
		) - 1;
		if(y1 <= 0) {
			startY = 0;
			subY = -y1;
		} else {
			startY = (INT)ceil(y1);
			subY = (FLOAT)startY - y1;
		}
		
		const FLOAT dy2 = 1.0f / (y2 - y1);
		sx2 = (x2 - x1) * dy2;
		if(sx2 < sx1) {
			SWAP(FLOAT, sx1, sx2);
			
			swapX = TRUE;
		}
		
		xx1 = x1 + (sx1 * subY);
		xx2 = x1 + (sx2 * subY);
		
		dBitsY = dBits + mBuffer.yOffsets[startY];
		
		for(INT y = startY; y <= endY; ++y) {
			const FLOAT dx = 1.0f / (xx2 - xx1);
			
			endX = (xx2 >= width
				? width
				: (INT)ceil(xx2)
			) - 1;
			if(xx1 <= 0) {
				startX = 0;
				subX = -xx1;
			} else {
				startX = (INT)ceil(xx1);
				subX = (FLOAT)startX - xx1;
			}
			
			for(INT x = startX; x <= endX; ++x) {
				dBitsY[x] = 0xff007f00;
			}

			xx1 += sx1;
			xx2 += sx2;
			
			dBitsY += width;
		}
		
		if(TRUE == swapX) {
			xx1 = xx2;
			sx1 = sx2;
		}
	}

	if(y3 != y2) {
		endY = (y3 >= height
			? height
			: (INT)ceil(y3)
		) - 1;
		if(y2 <= 0) {
			startY = 0;
			subY = -y2;
		} else {
			startY = (INT)ceil(y2);
			subY = (FLOAT)startY - y2;
		}
		
		const FLOAT dy2 = 1.0f / (y3 - y2);
		const FLOAT dy = y2 != y1 ? ((FLOAT)startY - y1) : subY;
		sx2 = (x3 - x2) * dy2;
		if(TRUE == swapX) {
			SWAP(FLOAT, sx1, sx2);
			
			xx1 = x2 + (sx1 * subY);
			xx2 = x1 + (sx2 * dy);
		} else {
			xx1 = x1 + (sx1 * dy);
			xx2 = x2 + (sx2 * subY);
		}
		
		dBitsY = dBits + mBuffer.yOffsets[startY];

		for(INT y = startY; y <= endY; ++y) {
			const FLOAT dx = 1.0f / (xx2 - xx1);

			endX = (xx2 >= width
				? width
				: (INT)ceil(xx2)
			) - 1;
			if(xx1 <= 0) {
				startX = 0;
				subX = -xx1;
			} else {
				startX = (INT)ceil(xx1);
				subX = (FLOAT)startX - xx1;
			}
			
			for(INT x = startX; x <= endX; ++x) {
				dBitsY[x] = 0xff007f00;
			}

			xx1 += sx1;
			xx2 += sx2;

			dBitsY += width;
		}
	}
}

VOID SoftwareRenderer::DrawScanLineTriangle(Triangle &t) {
	if(t.v1.y > t.v2.y) { SWAP(Vertex3c, t.v1, t.v2); }
	if(t.v1.y > t.v3.y) { SWAP(Vertex3c, t.v1, t.v3); }
	if(t.v2.y > t.v3.y) { SWAP(Vertex3c, t.v2, t.v3); }
	
	if(t.v3.y <= t.v1.y || t.v3.y <= 0 || t.v1.y >= height) {
		return;
	}
	
	const FLOAT z1 = 1.0f / (t.v1.z);
	const FLOAT z2 = 1.0f / (t.v2.z);
	const FLOAT z3 = 1.0f / (t.v3.z);
	
	if(!(
		(z1 >= 0.0f && z1 <= 1.0f)
		|| (z2 >= 0.0f && z2 <= 1.0f)
		|| (z3 >= 0.0f && z3 <= 1.0f)
	)) {
		return;
	}
	
	const Bitmap *texture = textures[t.texId];
	const INT tWidth = texture -> infoHeader.biWidth - 1;
	const INT tHeight = texture -> infoHeader.biHeight - 1;
	const BYTE *tBits = (BYTE *)texture -> data;
	FLOAT *zBitsY, *zBits = (FLOAT *)zBuffer.bits;
	DWORD *dBitsY, *dBits = (DWORD *)mBuffer.bits;

	const FLOAT x1 = t.v1.x, y1 = t.v1.y;
	const FLOAT x2 = t.v2.x, y2 = t.v2.y;
	const FLOAT x3 = t.v3.x, y3 = t.v3.y;
	const FLOAT u1 = t.v1.u * z1 * tWidth, v1 = t.v1.v * z1 * tHeight;
	const FLOAT u2 = t.v2.u * z2 * tWidth, v2 = t.v2.v * z2 * tHeight;
	const FLOAT u3 = t.v3.u * z3 * tWidth, v3 = t.v3.v * z3 * tHeight;
	const FLOAT r1 = t.v1.c.r, g1 = t.v1.c.g, b1 = t.v1.c.b;//, a1 = t.v1.c.a;
	const FLOAT r2 = t.v2.c.r, g2 = t.v2.c.g, b2 = t.v2.c.b;//, a2 = t.v2.c.a;
	const FLOAT r3 = t.v3.c.r, g3 = t.v3.c.g, b3 = t.v3.c.b;//, a3 = t.v3.c.a;

	BOOL swapX = FALSE;
	
	const FLOAT dy1 = 1.0f / (y3 - y1);
	
	FLOAT sx1 = (x3 - x1) * dy1;
	FLOAT sz1 = (z3 - z1) * dy1;
	FLOAT su1 = (u3 - u1) * dy1;
	FLOAT sv1 = (v3 - v1) * dy1;
	FLOAT sr1 = (r3 - r1) * dy1;
	FLOAT sg1 = (g3 - g1) * dy1;
	FLOAT sb1 = (b3 - b1) * dy1;
	//FLOAT sa1 = (a3 - a1) * dy1;
	
	FLOAT sx2, sz2, su2, sv2;
	FLOAT sr2, sg2, sb2;//, sa2;
	
	FLOAT xx1 = x1, zz1 = z1;
	FLOAT uu1 = u1, vv1 = v1;
	FLOAT rr1 = r1, gg1 = g1;
	FLOAT bb1 = b1;//, aa1 = a1;
	
	FLOAT xx2, zz2, uu2, vv2;
	FLOAT rr2, gg2, bb2;//, aa2;
	
	FLOAT zx, ux, vx;
	FLOAT rx, gx, bx;//, ax;
	
	FLOAT swap;
	
	FLOAT subX, subY;
	INT startX, startY;
	INT endX, endY;
	
	if(y2 != y1) {
		endY = (y2 >= height
			? height
			: (INT)ceil(y2)
		) - 1;
		if(y1 <= 0) {
			subY = -y1;
			startY = 0;
		} else {
			startY = (INT)ceil(y1);
			subY = (FLOAT)startY - y1;
		}
		if(startY >= endY) { return; }
		
		const FLOAT dy2 = 1.0f / (y2 - y1);
		sx2 = (x2 - x1) * dy2;
		if(sx2 < sx1) {
			swap = sx1; sx1 = sx2; sx2 = swap;

			sz2 = sz1; sz1 = (z2 - z1) * dy2;
			su2 = su1; su1 = (u2 - u1) * dy2;
			sv2 = sv1; sv1 = (v2 - v1) * dy2;
			sr2 = sr1; sr1 = (r2 - r1) * dy2;
			sg2 = sg1; sg1 = (g2 - g1) * dy2;
			sb2 = sb1; sb1 = (b2 - b1) * dy2;
			//sa2 = sa1; sa1 = (a2 - a1) * dy2;
			
			swapX = TRUE;
		} else {
			sz2 = (z2 - z1) * dy2; su2 = (u2 - u1) * dy2;
			sv2 = (v2 - v1) * dy2; sr2 = (r2 - r1) * dy2;
			sg2 = (g2 - g1) * dy2; sb2 = (b2 - b1) * dy2;
			//sa2 = (a2 - a1) * dy2;
		}
		
		xx2 = x1 + (sx2 * subY); xx1 = x1 + (sx1 * subY);
		zz2 = z1 + (sz2 * subY); zz1 = z1 + (sz1 * subY);
		uu2 = u1 + (su2 * subY); uu1 = u1 + (su1 * subY);
		vv2 = v1 + (sv2 * subY); vv1 = v1 + (sv1 * subY);
		rr2 = r1 + (sr2 * subY); rr1 = r1 + (sr1 * subY);
		gg2 = g1 + (sg2 * subY); gg1 = g1 + (sg1 * subY);
		bb2 = b1 + (sb2 * subY); bb1 = b1 + (sb1 * subY);
		//aa2 = a1 + (sa2 * subY); aa1 = a1 + (sa1 * subY);
		
		zBitsY = zBits + mBuffer.yOffsets[startY];
		dBitsY = dBits + mBuffer.yOffsets[startY];
		
		for(INT y = startY; y <= endY; ++y) {
			const FLOAT dx = 1.0f / (xx2 - xx1);
			
			const FLOAT szx = (zz2 - zz1) * dx;
			const FLOAT sux = (uu2 - uu1) * dx;
			const FLOAT svx = (vv2 - vv1) * dx;
			const FLOAT srx = (rr2 - rr1) * dx;
			const FLOAT sgx = (gg2 - gg1) * dx;
			const FLOAT sbx = (bb2 - bb1) * dx;
			//const FLOAT sax = (aa2 - aa1) * dx;
			
			endX = (xx2 >= width
				? width
				: (INT)ceil(xx2)
			) - 1;
			if(xx1 <= 0) {
				subX = -xx1;
				startX = 0;
			} else {
				startX = (INT)ceil(xx1);
				subX = (FLOAT)startX - xx1;
			}
			
			zx = zz1 + (szx * subX);
			ux = uu1 + (sux * subX);
			vx = vv1 + (svx * subX);
			rx = rr1 + (srx * subX);
			gx = gg1 + (sgx * subX);
			bx = bb1 + (sbx * subX);
			//ax = aa1 + (sax * subX);
			
			for(INT x = startX; x <= endX; ++x) {
				if(zx < 1.0f && zx > zBitsY[x]) {
					const FLOAT zp = 1.0f / zx;
					INT u = (INT)(ux * zp);
					INT v = (INT)(vx * zp);
					
					if(u >= tWidth || u <= 0) {
						u = ABS(u) & tWidth;
					}
					if(v >= tHeight || v <= 0) {
						v = ABS(v) & tHeight;
					}
					
					const LONG tOff =
						texture -> xOffsets[u]
						+ texture -> yOffsets[v]
					;

					dBitsY[x] = RGBd(
						(FLOAT)tBits[tOff] * rx
						, (FLOAT)tBits[tOff + 1] * gx
						, (FLOAT)tBits[tOff + 2] * bx
					);
					zBitsY[x] = zx;
					
					//BYTE zx2 = (BYTE)(zx * 255.0f);
					//dBitsX[x] = RGBAb(zx2, zx2, zx2, 1.0f);
				}
				
				zx += szx; ux += sux;
				vx += svx; rx += srx;
				gx += sgx; bx += sbx;
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
			
			dBitsY += width;
			zBitsY += width;
		}
		
		if(TRUE == swapX) {
			xx1 = xx2; sx1 = sx2;
			zz1 = zz2; sz1 = sz2;
			uu1 = uu2; su1 = su2;
			vv1 = vv2; sv1 = sv2;
			rr1 = rr2; sr1 = sr2;
			gg1 = gg2; sg1 = sg2;
			bb1 = bb2; sb1 = sb2;
			// aa1 = aa2; sa1 = sa2;
		}
	}

	if(y3 != y2) {
		endY = (y3 >= height
			? height
			: (INT)ceil(y3)
		) - 1;
		if(y2 <= 0) {
			subY = -y2;
			startY = 0;
		} else {
			startY = (INT)ceil(y2);
			subY = (FLOAT)startY - y2;
		}
		if(startY >= endY) { return; }
		
		const FLOAT dy2 = 1.0f / (y3 - y2);
		if(TRUE == swapX) {
			sx2 = sx1; sx1 = (x3 - x2) * dy2;
			sz2 = sz1; sz1 = (z3 - z2) * dy2;
			su2 = su1; su1 = (u3 - u2) * dy2;
			sv2 = sv1; sv1 = (v3 - v2) * dy2;
			sr2 = sr1; sr1 = (r3 - r2) * dy2;
			sg2 = sg1; sg1 = (g3 - g2) * dy2;
			sb2 = sb1; sb1 = (b3 - b2) * dy2;
			//sa2 = sa1; sa1 = (a3 - a2) * dy2;
			
			const FLOAT dy = y2 != y1 ? ((FLOAT)startY - y1) : subY;

			xx2 = x1 + (sx2 * dy); zz2 = z1 + (sz2 * dy);
			uu2 = u1 + (su2 * dy); vv2 = v1 + (sv2 * dy);
			rr2 = r1 + (sr2 * dy); gg2 = g1 + (sg2 * dy);
			bb2 = b1 + (sb2 * dy);// aa2 = a1 + (sa2 * dy);
			
			xx1 = x2 + (sx1 * subY); zz1 = z2 + (sz1 * subY);
			uu1 = u2 + (su1 * subY); vv1 = v2 + (sv1 * subY);
			rr1 = r2 + (sr1 * subY); gg1 = g2 + (sg1 * subY);
			bb1 = b2 + (sb1 * subY);// aa1 = a2 + (sa1 * subY);
		} else {
			sx2 = (x3 - x2) * dy2;
			sz2 = (z3 - z2) * dy2;
			su2 = (u3 - u2) * dy2;
			sv2 = (v3 - v2) * dy2;
			sr2 = (r3 - r2) * dy2;
			sg2 = (g3 - g2) * dy2;
			sb2 = (b3 - b2) * dy2;
			//sa2 = (a3 - a2) * dy2;
			
			const FLOAT dy = y2 != y1 ? ((FLOAT)startY - y1) : subY;
			
			xx1 = x1 + (sx1 * dy); zz1 = z1 + (sz1 * dy);
			uu1 = u1 + (su1 * dy); vv1 = v1 + (sv1 * dy);
			rr1 = r1 + (sr1 * dy); gg1 = g1 + (sg1 * dy);
			bb1 = b1 + (sb1 * dy);// aa1 = a1 + (sa1 * dy);
		
			xx2 = x2 + (sx2 * subY); zz2 = z2 + (sz2 * subY);
			uu2 = u2 + (su2 * subY); vv2 = v2 + (sv2 * subY);
			rr2 = r2 + (sr2 * subY); gg2 = g2 + (sg2 * subY);
			bb2 = b2 + (sb2 * subY);// aa2 = a2 + (sa2 * subY);
		}
		
		zBitsY = zBits + mBuffer.yOffsets[startY];
		dBitsY = dBits + mBuffer.yOffsets[startY];

		for(INT y = startY; y <= endY; ++y) {
			const FLOAT dx = 1.0f / (xx2 - xx1);
			
			const FLOAT szx = (zz2 - zz1) * dx;
			const FLOAT sux = (uu2 - uu1) * dx;
			const FLOAT svx = (vv2 - vv1) * dx;
			const FLOAT srx = (rr2 - rr1) * dx;
			const FLOAT sgx = (gg2 - gg1) * dx;
			const FLOAT sbx = (bb2 - bb1) * dx;
			//const FLOAT sax = (aa2 - aa1) * dx;
			
			endX = (xx2 >= width
				? width
				: (INT)ceil(xx2)
			) - 1;
			if(xx1 <= 0) {
				subX = -xx1;
				startX = 0;
			} else {
				startX = (INT)ceil(xx1);
				subX = (FLOAT)startX - xx1;
			}
			
			zx = zz1 + (szx * subX);
			ux = uu1 + (sux * subX);
			vx = vv1 + (svx * subX);
			rx = rr1 + (srx * subX);
			gx = gg1 + (sgx * subX);
			bx = bb1 + (sbx * subX);
			//ax = aa1 + (sax * subX);
			
			for(INT x = startX; x <= endX; ++x) {
				if(zx < 1.0f && zx > zBitsY[x]) {
					const FLOAT zp = 1.0f / zx;
					INT u = (INT)(ux * zp);
					INT v = (INT)(vx * zp);
					
					if(u >= tWidth || u <= 0) {
						u = ABS(u) & tWidth;
					}
					if(v >= tHeight || v <= 0) {
						v = ABS(v) & tHeight;
					}
					
					const LONG tOff =
						texture -> xOffsets[u]
						+ texture -> yOffsets[v]
					;
					
					dBitsY[x] = RGBd(
						(FLOAT)tBits[tOff] * rx
						, (FLOAT)tBits[tOff + 1] * gx
						, (FLOAT)tBits[tOff + 2] * bx
					);
					zBitsY[x] = zx;

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
			
			dBitsY += width;
			zBitsY += width;
		}
	}
}

VOID SoftwareRenderer::DrawHalfSpaceTriangle(Triangle &t) {
	DWORD *dBits = (DWORD *)mBuffer.bits;

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
			
			if(0xF == a && 0xF == b && 0xF == c) {
				for(INT iy = y, iyq = y + q; iy < iyq; ++iy) {
					for(INT ix = x, ixq = x + q; ix < ixq; ++ix) {
						dBitsX[ix] = 0xff007f00;
					}

					dBitsX += width;
				}
			} else {
				for(INT iy = y, iyq = y + q; iy < iyq; ++iy) {
					INT cx1 = cy1 - 1;
					INT cx2 = cy2 - 1;
					INT cx3 = cy3 - 1;

					for(INT ix = x, ixq = x + q; ix < ixq; ++ix) {
						if((cx1 | cx2 | cx3) >= 0) {
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
				}
			}
		}

		dBits += mBuffer.yOffsets[q];
	}
}

VOID SoftwareRenderer::DrawLine(Vertex2 &v1, Vertex2 &v2, DWORD color) {
	Vertex2 v[2];

	v[0] = v1;
	v[1] = v2;

	const FLOAT dy = v[0].y - v[1].y;
	const FLOAT dx = v[0].x - v[1].x;

	DWORD *bits = (DWORD *)mBuffer.bits;

	if(abs(dy) > abs(dx)) {
		if(v[0].y > v[1].y) {
			SWAP(Vertex2, v[0], v[1]);
		}

		const FLOAT sub = dx / dy;

		FLOAT x;
		INT y1;
		const INT y2 = v[1].y >= height ? height - 1 : (INT)ceil(v[1].y);
		if(v[0].y <= 0) {
			y1 = 0;
			x = v[0].x - sub * (INT)ceil(v[0].y);
		} else {
			y1 = (INT)ceil(v[0].y);
			x = v[0].x;
		}
		
		DWORD *bitsX = bits;
		for(INT y = y1; y < y2; ++y) {
			if(x >= width) {
				break;
			}

			if(x >= 0) {
				bitsX = bits + mBuffer.yOffsets[y];
				bitsX[(INT)x] = color;
			}

			x += sub;
		}
	} else {
		if(v[0].x > v[1].x) {
			SWAP(Vertex2, v[0], v[1]);
		}

		const FLOAT sub = dy / dx;

		FLOAT y;
		INT x1;
		const INT x2 = v[1].x >= width ? width - 1 : (INT)ceil(v[1].x);
		if(v[0].x < 0) {
			x1 = 0;
			y = v[0].y - sub * (INT)ceil(v[0].x);
		} else {
			x1 = (INT)ceil(v[0].x);
			y = v[0].y;
		}

		DWORD *bitsX = bits;
		for(INT x = x1; x < x2; ++x) {
			if(y >= height) {
				break;
			}

			if(y >= 0) {
				bitsX = bits + mBuffer.yOffsets[(INT)y];
				bitsX[x] = color;
			}

			y += sub;
		}
	}
}

HRESULT SoftwareRenderer::SetSize(DWORD width, DWORD height) {
	ReleaseBuffer(mBuffer);
	ReleaseBuffer(zBuffer);

	props.screenHeight = height;
	props.screenWidth = width;
	GetWindowRect(props.hWnd, &props.screenRect);

	if(
		FAILED(mBuffer.Init(mDeviceContext, width, height))
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
	ReleaseBuffer(zBuffer);

	if(NULL != mDeviceContext) {
		ReleaseDC(props.hWnd, mDeviceContext);
	}

	mDeviceContext = NULL;
}

SoftwareRenderer sftObj;
SoftwareRenderer *g_3D = &sftObj;