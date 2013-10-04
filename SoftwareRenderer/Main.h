#ifndef SOFTWARERENDERER_MAIN_H
#define SOFTWARERENDERER_MAIN_H

VOID InitWindow();
VOID DeInitWindow();
VOID ChangeToFullScreen();
VOID InitSinCos();
VOID CalculateFrameRate();
BOOL ClampFrameRate(INT desiredFrameRate);
VOID CheckForMovement();
WPARAM MainLoop();
INT WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR cmdLine, INT cmdshow);
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif