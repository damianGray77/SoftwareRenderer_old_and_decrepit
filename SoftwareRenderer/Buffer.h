#ifndef SOFTWARERENDERER_BUFFER_H
#define SOFTWARERENDERER_BUFFER_H

class Buffer {
	VOID *bits;
	FLOAT *buf;
	DWORD width, height;
	DWORD mWidth, mHeight;
public:
	Buffer();
	
	BOOL Init(DWORD w, DWORD h);
	VOID DeInit();
	
	VOID* GetBits();
	
	VOID Clear(FLOAT c);

	DWORD GetWidth();
	DWORD GetHeight();
	DWORD GetMidWidth();
	DWORD GetMidHeight();
	
	BOOL IsBuffer();
	
	~Buffer() {}
};

#endif