#ifndef TEXTURE_ANISTROPHIZER_MAIN_H
#define TEXTURE_ANISTROPHIZER_MAIN_H

#include "Bitmap.h"

VOID InitWindow();
VOID DeInitWindow();
WPARAM MainLoop();
INT WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR cmdLine, INT cmdshow);
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

VOID DrawBitmap();
VOID ReleaseBuffer();
HRESULT CreateBuffer();
VOID ClearBuffer(DWORD color);
HRESULT SwapBuffers();
HRESULT SetSize(DWORD width, DWORD height);

#endif