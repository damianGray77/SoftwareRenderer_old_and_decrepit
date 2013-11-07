#include "stdafx.h"
#include "Main.h"

VOID InitWindow() {
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, MsgProc, 0L, 0L, GetModuleHandle(NULL), LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW + 1), NULL, props.className, NULL };

    RegisterClassEx(&wc);

	DWORD winStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN; 
	DWORD winStyleEx = WS_EX_CLIENTEDGE;

	props.screenRect.bottom = props.screenHeight;
	props.screenRect.right = props.screenWidth;
	AdjustWindowRectEx(&props.screenRect, winStyle, FALSE, winStyleEx);

	props.hWnd = CreateWindowEx(winStyleEx, props.className, props.windowName, winStyle, CW_USEDEFAULT, CW_USEDEFAULT, props.screenRect.right - props.screenRect.left, props.screenRect.bottom - props.screenRect.top, NULL, NULL, wc.hInstance, NULL);
	
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
}

VOID DeInitWindow() {
	ReleaseBuffer();

	if(NULL != props.hDC) {
		ReleaseDC(props.hWnd, props.hDC);
	}

	props.hDC = NULL;
	
	UnregisterClass(props.className, GetModuleHandle(NULL));
}

BYTE AverageBYTE(BYTE b1, BYTE b2) {
	return (BYTE)((((INT)b1 + (INT)b2 + 2) / 2) - 1);
}

VOID DrawBitmap() {
	DWORD *bits = (DWORD *)props.buffer -> bits;
	
	Bitmap *bmp = new Bitmap(L"texture.bmp");
	INT tWidth = bmp -> infoHeader.biWidth;
	INT tHeight = bmp -> infoHeader.biHeight;
	BYTE *tBits = (BYTE *)bmp -> data;
	
	DWORD *bitsY = bits;
	for(INT y = 0; y < tHeight; ++y) {
		for(INT x = 0; x < tWidth; ++x) {
			LONG tOff = bmp -> yOffsets[y] + bmp -> xOffsets[tWidth - x - 1];
			
			bitsY[x] = RGBd(tBits[tOff + 2], tBits[tOff + 1], tBits[tOff]);
		}
		
		bitsY += props.buffer -> width;
	}
	
	bmp -> Release();
	
	delete bmp;
	
	bmp = NULL;
	
	bitsY = bits + tWidth;
	for(INT y = 0; y < tHeight; ++y) {
		for(INT x = 0; x < tWidth / 2; ++x) {
			LONG tOff1 = props.buffer -> yOffsets[y] + (x * 2);
			LONG tOff2 = tOff1 + 1;
			
			BYTE r = AverageBYTE(RB(bits[tOff1]), RB(bits[tOff2]));
			BYTE g = AverageBYTE(GB(bits[tOff1]), GB(bits[tOff2]));
			BYTE b = AverageBYTE(BB(bits[tOff1]), BB(bits[tOff2]));
			
			bitsY[x] = RGBd(r, g, b);
		}
		
		bitsY += props.buffer -> width;
	}
	
	bitsY = bits + tWidth + (tWidth / 2);
	for(INT y = 0; y < tHeight; ++y) {
		for(INT x = 0; x < tWidth / 4; ++x) {
			LONG tOff1 = props.buffer -> yOffsets[y] + tWidth + (x * 2);
			LONG tOff2 = tOff1 + 1;
			
			BYTE r = AverageBYTE(RB(bits[tOff1]), RB(bits[tOff2]));
			BYTE g = AverageBYTE(GB(bits[tOff1]), GB(bits[tOff2]));
			BYTE b = AverageBYTE(BB(bits[tOff1]), BB(bits[tOff2]));
			
			bitsY[x] = RGBd(r, g, b);
		}
		
		bitsY += props.buffer -> width;
	}
	
	bitsY = bits + tWidth + (tWidth / 2) + (tWidth / 4);
	for(INT y = 0; y < tHeight; ++y) {
		for(INT x = 0; x < tWidth / 8; ++x) {
			LONG tOff1 = props.buffer -> yOffsets[y] + tWidth + (tWidth / 2) + (x * 2);
			LONG tOff2 = tOff1 + 1;
			
			BYTE r = AverageBYTE(RB(bits[tOff1]), RB(bits[tOff2]));
			BYTE g = AverageBYTE(GB(bits[tOff1]), GB(bits[tOff2]));
			BYTE b = AverageBYTE(BB(bits[tOff1]), BB(bits[tOff2]));
			
			bitsY[x] = RGBd(r, g, b);
		}
		
		bitsY += props.buffer -> width;
	}
	
	bitsY = bits + tWidth + (tWidth / 2) + (tWidth / 4) + (tWidth / 8);
	for(INT y = 0; y < tHeight; ++y) {
		for(INT x = 0; x < tWidth / 16; ++x) {
			LONG tOff1 = props.buffer -> yOffsets[y] + tWidth + (tWidth / 2) + (tWidth / 4) + (x * 2);
			LONG tOff2 = tOff1 + 1;
			
			BYTE r = AverageBYTE(RB(bits[tOff1]), RB(bits[tOff2]));
			BYTE g = AverageBYTE(GB(bits[tOff1]), GB(bits[tOff2]));
			BYTE b = AverageBYTE(BB(bits[tOff1]), BB(bits[tOff2]));
			
			bitsY[x] = RGBd(r, g, b);
		}
		
		bitsY += props.buffer -> width;
	}
	
	bitsY = bits + tWidth + (tWidth / 2) + (tWidth / 4) + (tWidth / 8) + (tWidth / 16);
	for(INT y = 0; y < tHeight; ++y) {
		for(INT x = 0; x < tWidth / 32; ++x) {
			LONG tOff1 = props.buffer -> yOffsets[y] + tWidth + (tWidth / 2) + (tWidth / 4) + (tWidth / 8) + (x * 2);
			LONG tOff2 = tOff1 + 1;
			
			BYTE r = AverageBYTE(RB(bits[tOff1]), RB(bits[tOff2]));
			BYTE g = AverageBYTE(GB(bits[tOff1]), GB(bits[tOff2]));
			BYTE b = AverageBYTE(BB(bits[tOff1]), BB(bits[tOff2]));
			
			bitsY[x] = RGBd(r, g, b);
		}
		
		bitsY += props.buffer -> width;
	}
	
	bitsY = bits + tWidth + (tWidth / 2) + (tWidth / 4) + (tWidth / 8) + (tWidth / 16) + (tWidth / 32);
	for(INT y = 0; y < tHeight; ++y) {
		for(INT x = 0; x < tWidth / 64; ++x) {
			LONG tOff1 = props.buffer -> yOffsets[y] + tWidth + (tWidth / 2) + (tWidth / 4) + (tWidth / 8) + (tWidth / 16) + (x * 2);
			LONG tOff2 = tOff1 + 1;
			
			BYTE r = AverageBYTE(RB(bits[tOff1]), RB(bits[tOff2]));
			BYTE g = AverageBYTE(GB(bits[tOff1]), GB(bits[tOff2]));
			BYTE b = AverageBYTE(BB(bits[tOff1]), BB(bits[tOff2]));
			
			bitsY[x] = RGBd(r, g, b);
		}
		
		bitsY += props.buffer -> width;
	}
	
	bitsY = bits + props.buffer -> yOffsets[tHeight];
	for(INT y = 0; y < tHeight / 2; ++y) {
		for(INT x = 0; x < tWidth * 2; ++x) {
			LONG tOff1 = props.buffer -> yOffsets[y * 2] + x;
			LONG tOff2 = tOff1 + props.buffer -> width;
			
			BYTE r = AverageBYTE(RB(bits[tOff1]), RB(bits[tOff2]));
			BYTE g = AverageBYTE(GB(bits[tOff1]), GB(bits[tOff2]));
			BYTE b = AverageBYTE(BB(bits[tOff1]), BB(bits[tOff2]));
			
			bitsY[x] = RGBd(r, g, b);
		}
		
		bitsY += props.buffer -> width;
	}
	
	bitsY = bits + props.buffer -> yOffsets[tHeight] + props.buffer -> yOffsets[tHeight / 2];
	for(INT y = 0; y < tHeight / 4; ++y) {
		for(INT x = 0; x < tWidth * 2; ++x) {
			LONG tOff1 = props.buffer -> yOffsets[y * 2] + props.buffer -> yOffsets[tHeight] + x;
			LONG tOff2 = tOff1 + props.buffer -> width;
			
			BYTE r = AverageBYTE(RB(bits[tOff1]), RB(bits[tOff2]));
			BYTE g = AverageBYTE(GB(bits[tOff1]), GB(bits[tOff2]));
			BYTE b = AverageBYTE(BB(bits[tOff1]), BB(bits[tOff2]));
			
			bitsY[x] = RGBd(r, g, b);
		}
		
		bitsY += props.buffer -> width;
	}
	
	bitsY = bits + props.buffer -> yOffsets[tHeight] + props.buffer -> yOffsets[tHeight / 2] + props.buffer -> yOffsets[tHeight / 4];
	for(INT y = 0; y < tHeight / 8; ++y) {
		for(INT x = 0; x < tWidth * 2; ++x) {
			LONG tOff1 = props.buffer -> yOffsets[y * 2] + props.buffer -> yOffsets[tHeight] + props.buffer -> yOffsets[tHeight / 2] + x;
			LONG tOff2 = tOff1 + props.buffer -> width;
			
			BYTE r = AverageBYTE(RB(bits[tOff1]), RB(bits[tOff2]));
			BYTE g = AverageBYTE(GB(bits[tOff1]), GB(bits[tOff2]));
			BYTE b = AverageBYTE(BB(bits[tOff1]), BB(bits[tOff2]));
			
			bitsY[x] = RGBd(r, g, b);
		}
		
		bitsY += props.buffer -> width;
	}
	
	bitsY = bits + props.buffer -> yOffsets[tHeight] + props.buffer -> yOffsets[tHeight / 2] + props.buffer -> yOffsets[tHeight / 4] + props.buffer -> yOffsets[tHeight / 8];
	for(INT y = 0; y < tHeight / 16; ++y) {
		for(INT x = 0; x < tWidth * 2; ++x) {
			LONG tOff1 = props.buffer -> yOffsets[y * 2] + props.buffer -> yOffsets[tHeight] + props.buffer -> yOffsets[tHeight / 2] + props.buffer -> yOffsets[tHeight / 4] + x;
			LONG tOff2 = tOff1 + props.buffer -> width;
			
			BYTE r = AverageBYTE(RB(bits[tOff1]), RB(bits[tOff2]));
			BYTE g = AverageBYTE(GB(bits[tOff1]), GB(bits[tOff2]));
			BYTE b = AverageBYTE(BB(bits[tOff1]), BB(bits[tOff2]));
			
			bitsY[x] = RGBd(r, g, b);
		}
		
		bitsY += props.buffer -> width;
	}
	
	bitsY = bits + props.buffer -> yOffsets[tHeight] + props.buffer -> yOffsets[tHeight / 2] + props.buffer -> yOffsets[tHeight / 4] + props.buffer -> yOffsets[tHeight / 8] + props.buffer -> yOffsets[tHeight / 16];
	for(INT y = 0; y < tHeight / 32; ++y) {
		for(INT x = 0; x < tWidth * 2; ++x) {
			LONG tOff1 = props.buffer -> yOffsets[y * 2] + props.buffer -> yOffsets[tHeight] + props.buffer -> yOffsets[tHeight / 2] + props.buffer -> yOffsets[tHeight / 4] + props.buffer -> yOffsets[tHeight / 8] + x;
			LONG tOff2 = tOff1 + props.buffer -> width;
			
			BYTE r = AverageBYTE(RB(bits[tOff1]), RB(bits[tOff2]));
			BYTE g = AverageBYTE(GB(bits[tOff1]), GB(bits[tOff2]));
			BYTE b = AverageBYTE(BB(bits[tOff1]), BB(bits[tOff2]));
			
			bitsY[x] = RGBd(r, g, b);
		}
		
		bitsY += props.buffer -> width;
	}
	
	bitsY = bits + props.buffer -> yOffsets[tHeight] + props.buffer -> yOffsets[tHeight / 2] + props.buffer -> yOffsets[tHeight / 4] + props.buffer -> yOffsets[tHeight / 8] + props.buffer -> yOffsets[tHeight / 16] + props.buffer -> yOffsets[tHeight / 32];
	for(INT y = 0; y < tHeight / 64; ++y) {
		for(INT x = 0; x < tWidth * 2; ++x) {
			LONG tOff1 = props.buffer -> yOffsets[y * 2] + props.buffer -> yOffsets[tHeight] + props.buffer -> yOffsets[tHeight / 2] + props.buffer -> yOffsets[tHeight / 4] + props.buffer -> yOffsets[tHeight / 8] + props.buffer -> yOffsets[tHeight / 16] + x;
			LONG tOff2 = tOff1 + props.buffer -> width;
			
			BYTE r = AverageBYTE(RB(bits[tOff1]), RB(bits[tOff2]));
			BYTE g = AverageBYTE(GB(bits[tOff1]), GB(bits[tOff2]));
			BYTE b = AverageBYTE(BB(bits[tOff1]), BB(bits[tOff2]));
			
			bitsY[x] = RGBd(r, g, b);
		}
		
		bitsY += props.buffer -> width;
	}
}

WPARAM MainLoop() {
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while(TRUE) {
		if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
			ClearBuffer(0xff000000);
			DrawBitmap();
			SwapBuffers();
			//PostQuitMessage(0);
        } 
	}

	return msg.wParam;
}

INT WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR cmdLine, INT cmdshow) {
	WPARAM par = 0;

	InitWindow();
	
	if(NULL == props.hWnd) {
		DeInitWindow();

		return par;
	}
	
	ShowWindow(props.hWnd, SW_SHOWNORMAL);
	UpdateWindow(props.hWnd);

	SetFocus(props.hWnd);
	SetForegroundWindow(props.hWnd);
	
	if(FAILED(CreateBuffer())) {
		DeInitWindow();

		return par;
	}

	par = MainLoop();

	DeInitWindow();

	return par;
}

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
		case WM_SIZE:
			if(NULL != props.hDC) {
				SetSize(
					(LOWORD(lParam) < 64 ? 64 : LOWORD(lParam)) + (LOWORD(lParam) & 3)
					, (HIWORD(lParam) < 64 ? 64 : HIWORD(lParam)) + (HIWORD(lParam) & 3)
				);
			}
			return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            
            return 0;
		//case WM_PAINT:
            //ValidateRect(hWnd, NULL);

          //return 0;
		case WM_KEYDOWN:
			switch(wParam) {
				case VK_ESCAPE:
					SendMessage(hWnd, WM_CLOSE, 0, 0);
					break;
				case VK_SPACE:
					break;
			}
			
			return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

VOID ReleaseBuffer() {
	props.buffer -> DeInit();
}

HRESULT CreateBuffer() {
	props.hDC = GetDC(props.hWnd);
	
	return props.buffer -> Init(props.hDC, props.screenWidth, props.screenHeight);
}

VOID ClearBuffer(DWORD color) {
	props.buffer -> Clear(color);
}

HRESULT SwapBuffers() {
	assert(NULL != props.hDC);
	assert(NULL != props.buffer -> GetBufDC());

	GetClientRect(props.hWnd, &props.screenRect);

	if(!BitBlt(
		props.hDC
		, props.screenRect.left
		, props.screenRect.top
		, 1 + props.screenRect.right - props.screenRect.left
		, 1 + props.screenRect.bottom - props.screenRect.top
		, props.buffer -> GetBufDC()
		, 0
		, 0
		, SRCCOPY
	)) {
		MessageBoxW(props.hWnd, L"SwapBuffers - Cannot swap buffers!", L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT SetSize(DWORD width, DWORD height) {
	ReleaseBuffer();

	props.screenHeight = height;
	props.screenWidth = width;
	GetWindowRect(props.hWnd, &props.screenRect);

	if(FAILED(props.buffer -> Init(props.hDC, width, height))) {
		return E_FAIL;
	}

	return S_OK;
}