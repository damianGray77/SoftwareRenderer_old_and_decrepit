#ifndef SOFTWARERENDERER_WINBUFFER_H
#define SOFTWARERENDERER_WINBUFFER_H

class WinBuffer {
	HBITMAP bufBmp, defBmp;
	HDC bufDC, dibDC;
	VOID *bits;
	DWORD width, height;
	DWORD mWidth, mHeight;
public:
	WinBuffer();

	HRESULT Init(HDC dc, DWORD w, DWORD h);
	VOID DeInit();

	VOID* GetBits();
	
	VOID Clear(DWORD c);

	DWORD GetWidth();
	DWORD GetHeight();
	DWORD GetMidWidth();
	DWORD GetMidHeight();
	
	HBITMAP GetDIB();
	HDC GetBufDC();

	BOOL IsBuffer();

	~WinBuffer() {}
	
	LONG *yOffsets;
};

#endif