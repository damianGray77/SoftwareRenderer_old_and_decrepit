#ifndef SOFTWARERENDERER_TEXTURE_H
#define SOFTWARERENDERER_TEXTURE_H

#include "Color4b.h"

class Texture {
public:
	INT textureId;
	Color4b ambient, diffuse, specular;
	FLOAT specLevel;
};

#endif