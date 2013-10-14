#ifndef SOFTWARERENDERER_BITMAP_H
#define SOFTWARERENDERER_BITMAP_H

#ifndef WIN32

#pragma pack(push, bitmap_data, 1)

struct BITMAPFILEHEADER {
	WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
};

struct BITMAPINFOHEADER {
	DWORD biSize;
	LONG biWidth;
	LONG biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG biXPelsPerMeter;
	LONG biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
};

struct {
	BYTE rgbBlue; 
	BYTE rgbGreen; 
	BYTE rgbRed; 
	BYTE rgbReserved; 
};

#pragma pack(pop, bitmap_data)

#endif

class Bitmap {
public:
	Bitmap();
	Bitmap(const WCHAR *path);
	~Bitmap();

	VOID LoadFromFile(const WCHAR *path);
	VOID Release();

	DWORD *data;
	RGBQUAD *palette;
	DWORD size;
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
private:
	VOID ConvertToRGB8(const BYTE *temp);
	VOID ConvertToRGB24(const BYTE *temp);
};

#endif