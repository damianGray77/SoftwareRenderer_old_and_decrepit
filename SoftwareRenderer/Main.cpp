#include "stdafx.h"

VOID InitWindow() {
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, MsgProc, 0L, 0L, GetModuleHandle(NULL), LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW + 1), NULL, props.className, NULL };

    RegisterClassEx(&wc);

	if(MessageBoxW(NULL, L"Click Yes to go to full screen (Recommended)", L"Options", MB_YESNO | MB_ICONQUESTION) == IDNO) {
		props.fullScreen = FALSE;
	}

	DWORD winStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN; 
	DWORD winStyleEx = WS_EX_CLIENTEDGE;
	if(props.fullScreen) {
		winStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		winStyleEx = 0;
		ChangeToFullScreen();
		ShowCursor(FALSE);
	}

	props.screenRect.bottom = props.screenHeight;
	props.screenRect.right = props.screenWidth;
	AdjustWindowRectEx(&props.screenRect, winStyle, FALSE, winStyleEx);

	props.hWnd = CreateWindowEx(winStyleEx, props.className, props.windowName, winStyle, CW_USEDEFAULT, CW_USEDEFAULT, props.screenRect.right - props.screenRect.left, props.screenRect.bottom - props.screenRect.top, NULL, NULL, wc.hInstance, NULL);
}

VOID DeInitWindow() {
	if(props.fullScreen) {
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(TRUE);
	}

	UnregisterClass(props.className, GetModuleHandle(NULL));
}

VOID ChangeToFullScreen() {
	DEVMODE dmSettings;
	memset(&dmSettings, 0, sizeof(dmSettings));

	if(!EnumDisplaySettingsW(NULL, ENUM_CURRENT_SETTINGS, &dmSettings)) {
		MessageBoxW(NULL, L"Could Not Enum Display Settings", L"Error", MB_OK);
		
		return;
	}

	dmSettings.dmPelsWidth	= props.screenWidth;
	dmSettings.dmPelsHeight	= props.screenHeight;
	
	int result = ChangeDisplaySettingsW(&dmSettings, CDS_FULLSCREEN);	

	if(result != DISP_CHANGE_SUCCESSFUL) {
		MessageBoxW(NULL, L"Display Mode Not Compatible", L"Error", MB_OK);
		PostQuitMessage(0);
	}
}

VOID InitSinCos() {
	DOUBLE val;
	
	DOUBLE valMultiplier = PI / (SINCOSMAX * 0.5f);
	
	for(INT i = 0; i < SINCOSMAX; ++i) {
		val = i * valMultiplier;
		
		sinTbl[i] = (FLOAT)sin(val);
		invSinTbl[i] = 1.0f / sinTbl[i];
		
		cosTbl[i] = (FLOAT)cos(val);
		invCosTbl[i] = 1.0f / cosTbl[i];
	}
}

VOID CalculateFrameRate() {
	static FLOAT fps = 0.0f;
    static FLOAT lastTime = 0.0f;
	static WCHAR strFrameRate[50] = {0};

    FLOAT currentTime = GetTickCount() * 0.001f;				

    ++fps;

    if(currentTime - lastTime > 1.0f) {
	    lastTime = currentTime;

		swprintf(strFrameRate, L"Current Frames Per Second: %d", int(fps));
		SetWindowTextW(props.hWnd, strFrameRate);
        fps = 0;
    }
}

BOOL ClampFrameRate(INT desiredFrameRate) {
	static FLOAT lastTime = GetTickCount() * 0.001f;
	static FLOAT elapsedTime = 0.0f;

	FLOAT currentTime = GetTickCount() * 0.001f;
	FLOAT deltaTime = currentTime - lastTime;
	FLOAT desiredFPS = 1.0f / desiredFrameRate;

	elapsedTime += deltaTime;
	lastTime = currentTime;

	if(elapsedTime > desiredFPS) {
		props.delta = desiredFPS;
		elapsedTime -= desiredFPS;

		return TRUE;
	}

	return FALSE;
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
			if(TRUE == paused) {
				continue;
			}
			
			//if(ClampFrameRate(60)) {
				g_Camera.Update();
				if(FAILED(g_3D -> Render())) {
					PostQuitMessage(0);
				}

				CalculateFrameRate();
			//} else {
			//	Sleep(1);
			//}
        } 
	}

	return msg.wParam;
}

INT WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR cmdLine, INT cmdshow) {
	WPARAM par = 0;

	InitWindow();
	InitSinCos();
	
	BOOL didFailInit =
		NULL == props.hWnd
		|| FAILED(g_3D -> Init())
		|| FAILED(g_3D -> SetupGeometry())
		|| FAILED(g_3D -> SetupTextures())
	;

	if(didFailInit) {
		DeInitWindow();

		return par;
	}
	
	ShowWindow(props.hWnd, SW_SHOWNORMAL);
	UpdateWindow(props.hWnd);

	SetFocus(props.hWnd);
	if(!props.fullScreen) {
		SetForegroundWindow(props.hWnd);
	}

	//ShowCursor(FALSE);

	g_Camera.PositionCamera(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	par = MainLoop();

	DeInitWindow();

	return par;
}

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
		case WM_SIZE:
			if(!props.fullScreen) {
				UINT width = (LOWORD(lParam) < 64 ? 64 : LOWORD(lParam)) + (LOWORD(lParam) % 4);
				UINT height = (HIWORD(lParam) < 64 ? 64 : HIWORD(lParam)) + (HIWORD(lParam) % 4);
				
				g_3D -> SetSize(width, height);
			}
			
			return 0;
        case WM_DESTROY:
            g_3D -> CleanUp();
            PostQuitMessage(0);
            
            return 0;
		//case WM_PAINT:
            //g_3D -> Render();
            //ValidateRect(hWnd, NULL);

          //return 0;
		case WM_KEYDOWN:
			switch(wParam) {
				case VK_ESCAPE:
					SendMessage(hWnd, WM_CLOSE, 0, 0);
					break;
				case VK_SPACE:
					paused = !paused;
					break;
			}
			
			return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}