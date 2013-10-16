#include "stdafx.h"

Bitmap::Bitmap() {
	data = NULL;
}

Bitmap::Bitmap(const WCHAR *path) {
	data = NULL;
	xOffsets = NULL;
	yOffsets = NULL;
	
	LoadFromFile(path);
}

Bitmap::~Bitmap() {
	if(NULL != data || NULL != palette || NULL != xOffsets || NULL != yOffsets) {
		Release();
	}
}

VOID Bitmap::LoadFromFile(const WCHAR *path) {
	FILE *file;

	if(NULL != data) {
		Release();
	}

	if(NULL == path) {
		MessageBox(props.hWnd, L"Could not find the file!", L"Error", MB_OK | MB_ICONERROR);
		
		return;
	}
	
	_wfopen_s(&file, path, L"rb");

	if(NULL == file) {
		MessageBox(props.hWnd, L"Could not find the file.", L"Error", MB_OK | MB_ICONERROR);
	
		return;
	}

	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, file);
	if(fileHeader.bfType != 0x4D42) {
		fclose(file);

		MessageBox(props.hWnd, L"Unsupported format.", L"Error", MB_OK | MB_ICONERROR);
	
		return;
	}

	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);

	USHORT numColours = 1 << infoHeader.biBitCount;
	if(infoHeader.biBitCount <= 8) {
		palette = new RGBQUAD[numColours];
		fread(palette, sizeof(RGBQUAD), numColours, file);
	}

	fseek(file, fileHeader.bfOffBits, SEEK_SET);
	
	size = fileHeader.bfSize - fileHeader.bfOffBits;
	//size = infoHeader.biWidth * infoHeader.biHeight * (infoHeader.biBitCount / 8);
	
	xOffsets = new LONG[infoHeader.biWidth];
	for(INT i = 0; i < infoHeader.biWidth; ++i) {
		xOffsets[i] = i * 3;
	}
	
	yOffsets = new LONG[infoHeader.biHeight];
	for(INT i = 0; i < infoHeader.biHeight; ++i) {
		yOffsets[i] = infoHeader.biWidth * 3 * i;
	}

	BYTE *temp = new BYTE[size];
	if(NULL == temp) {
		delete[](temp);
		fclose(file);

		MessageBox(props.hWnd, L"Could not allocate memory.", L"Error", MB_OK | MB_ICONERROR);

		return;
	}

	fread(temp, sizeof(BYTE), size, file);
	if(NULL == temp) {
		delete[](temp);
		fclose(file);

		MessageBox(props.hWnd, L"Could not read file.", L"Error", MB_OK | MB_ICONERROR);

		return;
	}

	fclose(file);

	switch(infoHeader.biBitCount) {
		case 24:
			ConvertToRGB24(temp);
			break;
		default:
			ConvertToRGB8(temp);
			break;
	}

	delete[](temp);
	temp = NULL;
}

VOID Bitmap::Release() {
	if(NULL != data) {
		delete[](data);
	}

	if(NULL != palette) {
		delete[](palette);
	}
	
	if(NULL != xOffsets) {
		delete[](xOffsets);
	}
	
	if(NULL != yOffsets) {
		delete[](yOffsets);
	}

	data = NULL;
	palette = NULL;
	xOffsets = NULL;
	yOffsets = NULL;
}

VOID Bitmap::ConvertToRGB24(const BYTE *temp) {
	LONG byteWidth, padWidth;
	byteWidth = padWidth = infoHeader.biWidth * (infoHeader.biBitCount / 8);
	while(padWidth % 4 != 0) {
	   ++padWidth;
	}

	data = new BYTE[size];
	if(NULL == data) {
		MessageBox(props.hWnd, L"Could not allocate memory.", L"Error", MB_OK | MB_ICONERROR);

		delete[](data);

		return;
	}
	
	INT offset = padWidth - byteWidth;
	if(infoHeader.biHeight <= 0) {
		UINT j = 0;
		CONST BYTE *tempX = temp;
		for(UINT i = 0; i < size; i += 3) {
			if((padWidth > 0) && ((i + offset) % padWidth == 0)) {
				i += offset;
			}
			
			tempX = temp + i;
			
			BYTE r = tempX[2];
			BYTE g = tempX[1];
			BYTE b = tempX[0];
			
			data[j++] = r;
			data[j++] = g;
			data[b++] = b;
			
			//data[j++] = RGBAb(r, g, b, 255);
		}
	} else {
		UINT j = size - 1;
		CONST BYTE *tempX = temp;
		for(UINT i = 0; i < size; i += 3) {
			if((padWidth > 0) && ((i + offset) % padWidth == 0)) {
				i += offset;
			}
			
			tempX = temp + i;
			
			BYTE r = tempX[2];
			BYTE g = tempX[1];
			BYTE b = tempX[0];
			
			data[j--] = r;
			data[j--] = g;
			data[j--] = b;
			
			//data[j--] = RGBAb(r, g, b, 255);
		}
	}
}

VOID Bitmap::ConvertToRGB8(const BYTE *temp) {
	LONG byteWidth, padWidth;
	byteWidth = padWidth = infoHeader.biWidth * (infoHeader.biBitCount / 8);
	while(padWidth % 4 != 0) {
	   ++padWidth;
	}

	data = new BYTE[size * 3];
	if(NULL == data) {
		MessageBox(props.hWnd, L"Could not allocate memory.", L"Error", MB_OK | MB_ICONERROR);

		delete[](data);

		return;
	}

	INT offset = padWidth - byteWidth;
	if(infoHeader.biHeight > 0) {
		UINT j = 0;
		BYTE ref2;
		for(UINT i = 0; i < size * 3; i += 3) {
			if((padWidth > 0) && ((i + offset) % padWidth == 0)) {
				i += offset;
			}
			
			ref2 = *(temp + j);
			
			BYTE r = palette[ref2].rgbRed;
			BYTE g = palette[ref2].rgbGreen;
			BYTE b = palette[ref2].rgbBlue;
			
			data[j++] = r;
			data[j++] = g;
			data[j++] = b;
			
			//data[j++] = RGBAb(r, g, b, 255);
		}
	} else {
		UINT j = (size * 3) - 1;
		BYTE ref2;
		for(UINT i = 0; i < size * 3; i += 3) {
			if((padWidth > 0) && ((i + offset) % padWidth == 0)) {
				i += offset;
			}
			
			ref2 = *(temp + j);
			
			BYTE r = palette[ref2].rgbRed;
			BYTE g = palette[ref2].rgbGreen;
			BYTE b = palette[ref2].rgbBlue;
			
			data[j--] = r;
			data[j--] = g;
			data[j--] = b;
			
			//data[j--] = RGBAb(r, g, b, 255);
		}
	}
}