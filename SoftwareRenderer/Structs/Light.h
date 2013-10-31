#ifndef SOFTWARERENDERER_LIGHT_H
#define SOFTWARERENDERER_LIGHT_H

#include "Vertex3.h"
#include "Color4f.h"

class Light {
public:
	Vertex3 position;
	Color4f color;
};

#endif