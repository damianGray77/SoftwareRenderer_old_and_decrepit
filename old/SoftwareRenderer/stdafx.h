#include "targetver.h"

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#pragma comment(lib, "winmm.lib")

#include <algorithm>
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define kSpeed 0.3f

/*#include "WinBuffer.h"
#include "Buffer.h"

#include "Globals.h"
#include "Matrix.h"
#include "Vector.h"
#include "Color.h"
#include "Texture.h"
#include "Polygon.h"
#include "Triangle.h"
#include "Model.h"
#include "Camera.h"

#include "Resource.h"

#include "Bitmap.h"

#include "SoftwareRenderer.h"
#include "Main.h"*/