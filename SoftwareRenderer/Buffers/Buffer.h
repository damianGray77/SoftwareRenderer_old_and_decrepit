#ifndef SOFTWARERENDERER_BUFFER_H
#define SOFTWARERENDERER_BUFFER_H

class Buffer {
public:
	Buffer();
	
	BOOL Init(DWORD w, DWORD h);
	VOID DeInit();
	
	VOID Clear(FLOAT c);
	
	BOOL IsBuffer();
	
	~Buffer() {}
	
	VOID *bits;
	FLOAT *buf;
	DWORD width, height;
	DWORD mWidth, mHeight;
	
	LONG *yOffsets;
};

#endif