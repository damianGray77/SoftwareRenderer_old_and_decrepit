#ifndef SOFTWARERENDERER_POLYGON3CUV_H
#define SOFTWARERENDERER_POLYGON3CUV_H

#include "Color4f.h"
#include "Vertex2.h"

class Polygon3cuv {
public:
	INT i1, i2, i3;
	INT tId;
	Vertex2 uv1, uv2, uv3;
	Color4f c1, c2, c3;
};

#endif