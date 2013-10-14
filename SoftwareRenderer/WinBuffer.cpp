#include "stdafx.h"

WinBuffer::WinBuffer() {
	bufBmp = NULL;
	defBmp = NULL;

	bufDC = NULL;
	dibDC = NULL;
}

HRESULT WinBuffer::Init(HDC dc, DWORD w, DWORD h) {
	assert(bufBmp == NULL);	
	assert(dc != NULL);

	BITMAPINFO bmpInfo;
	ZeroMemory(&bmpInfo, sizeof(bmpInfo));

	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -((LONG)h);
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage = w * h * 4;

	bufBmp = CreateDIBSection(dc, &bmpInfo, DIB_RGB_COLORS, &bits, NULL, 0x0);
	if(NULL == bufBmp) {
		MessageBoxW(props.hWnd, L"Buffer::Init - Cannot create DIB section!", L"Error", MB_OK|MB_ICONERROR);
		return E_FAIL;
	}

	bufDC = CreateCompatibleDC(dc);
	if(NULL == bufDC) {
		MessageBoxW(props.hWnd, L"Buffer::Init - Cannot create storage DC!", L"Error", MB_OK|MB_ICONERROR);
		return E_FAIL;
	}

	defBmp = (HBITMAP)SelectObject(bufDC,bufBmp);
	if(NULL == defBmp) {
		MessageBoxW(props.hWnd, L"Buffer::Init - Cannot select DIB on storage DC!", L"Error", MB_OK|MB_ICONERROR);
		return E_FAIL;
	}

	dibDC = dc;

	width = w;
	mWidth = w * 0.5f;
	height = h;
	mHeight = h * 0.5f;

	return S_OK;
}

VOID WinBuffer::DeInit() {
	if(NULL != bufBmp) {
		if(NULL != bufDC) {
			if(NULL != defBmp) {
				SelectObject(bufDC, defBmp);
				defBmp = NULL;
			}

			DeleteDC(bufDC);
			bufDC = NULL;
		}

		DeleteObject(bufBmp);
		bufBmp = NULL;
	}

	bits = NULL;

	width = 0;
	height = 0;
}

VOID* WinBuffer::GetBits() {
	return bits;
}

VOID WinBuffer::Clear(DWORD c) {
	memset(bits, c, sizeof(DWORD) * width * height);
}

DWORD WinBuffer::GetWidth() {
	return width;
}

DWORD WinBuffer::GetHeight() {
	return height;
}

DWORD WinBuffer::GetMidWidth() {
	return mWidth;
}

DWORD WinBuffer::GetMidHeight() {
	return mHeight;
}

HBITMAP WinBuffer::GetDIB() {
	return bufBmp;
}

HDC WinBuffer::GetBufDC() {
	return bufDC;
}

BOOL WinBuffer::IsBuffer() {
	return (bufBmp != NULL);
}