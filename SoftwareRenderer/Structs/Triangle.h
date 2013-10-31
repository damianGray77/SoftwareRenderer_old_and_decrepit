#ifndef SOFTWARERENDERER_TRIANGLE_H
#define SOFTWARERENDERER_TRIANGLE_H

#include "Vertex3c.h"

class Triangle {
public:
	Vertex3c v1;
	Vertex3c v2;
	Vertex3c v3;
	INT texId;
};

#endif