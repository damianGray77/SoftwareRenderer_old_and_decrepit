#ifndef SOFTWARERENDERER_WINBUFFER_H
#define SOFTWARERENDERER_WINBUFFER_H

class WinBuffer {
public:
	WinBuffer();

	HRESULT Init(HDC dc, DWORD w, DWORD h);
	VOID DeInit();
	
	VOID Clear(DWORD c);
	
	HBITMAP GetDIB();
	HDC GetBufDC();

	BOOL IsBuffer();

	~WinBuffer() {}
	
	HBITMAP bufBmp, defBmp;
	HDC bufDC, dibDC;
	VOID *bits;
	DWORD width, height;
	DWORD mWidth, mHeight;
	size_t size;
	
	LONG *yOffsets;
};

#endif