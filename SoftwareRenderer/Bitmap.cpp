#include "stdafx.h"

Bitmap::Bitmap() {
	data = NULL;
}

Bitmap::Bitmap(const WCHAR *path) {
	data = NULL;
	LoadFromFile(path);
}

Bitmap::~Bitmap() {
	if(NULL != data || NULL != palette) {
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
	
	file = _wfopen(path, L"rb");

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

	data = NULL;
	palette = NULL;
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
		BYTE *ref1;
		const BYTE *ref2;
		for(UINT i = 0; i < size; i += 3) {
			if((padWidth > 0) && ((i + offset) % padWidth == 0)) {
				i += offset;
			}

			ref1 = data + j;
			ref2 = temp + i;
			
			ref1[2] = ref2[0];
			ref1[1] = ref2[1];
			ref1[0] = ref2[2];

			j += 3;
		}
	} else {
		UINT j = size - 3;
		BYTE *ref1;
		const BYTE *ref2;

		for(UINT i = 0; i < size; i += 3) {
			if((padWidth > 0) && ((i + offset) % padWidth == 0)) {
				i += offset;
			}

			ref1 = data + j;
			ref2 = temp + i;

			ref1[2] = ref2[0];
			ref1[1] = ref2[1];
			ref1[0] = ref2[2];
			j -= 3;
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
		BYTE ref2, *ref1;
		for(UINT i = 0; i < size * 3; i += 3) {
			if((padWidth > 0) && ((i + offset) % padWidth == 0)) {
				i += offset;
			}

			ref1 = data + i;
			ref2 = *(temp + j);
			*ref1 = palette[ref2].rgbRed;
			*(ref1 + 1) = palette[ref2].rgbGreen;
			*(ref1 + 2) = palette[ref2].rgbBlue;
			++j;
		}
	} else {
		UINT j = size - 1;
		BYTE ref2, *ref1;
		for(UINT i = 0; i < size * 3; i += 3) {
			if((padWidth > 0) && ((i + offset) % padWidth == 0)) {
				i += offset;
			}
			ref1 = data + i;
			ref2 = *(temp + j);
			*ref1 = palette[ref2].rgbRed;
			*(ref1 + 1) = palette[ref2].rgbGreen;
			*(ref1 + 2) = palette[ref2].rgbBlue;
			--j;
		}
	}
}