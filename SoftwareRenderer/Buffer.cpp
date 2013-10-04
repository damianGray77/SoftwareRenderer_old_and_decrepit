#include "stdafx.h"

Buffer::Buffer() {
	buf = NULL;
}

BOOL Buffer::Init(DWORD w, DWORD h) {
	buf = new FLOAT[w * h];
	if(NULL == buf) {
		return FALSE;
	}
	
	bits = buf;
	
	width = w;
	height = h;
	mWidth = w / 2;
	mHeight = h / 2;
	
	Clear(0);
	
	return TRUE;
}

VOID Buffer::DeInit() {
	if(NULL != buf) {
		delete[] buf;
	}
	
	buf = NULL;
	bits = NULL;

	width = 0;
	height = 0;
}

VOID* Buffer::GetBits() {
	return bits;
}

VOID Buffer::Clear(FLOAT c) {
	std::fill((FLOAT*)bits, (FLOAT*)bits + width * height, c);
	//memset(bits, c, width * height * sizeof(FLOAT));
}

DWORD Buffer::GetWidth() {
	return width;
}

DWORD Buffer::GetHeight() {
	return height;
}

DWORD Buffer::GetMidWidth() {
	return mWidth;
}

DWORD Buffer::GetMidHeight() {
	return mHeight;
}

BOOL Buffer::IsBuffer() {
	return (buf != NULL);
}