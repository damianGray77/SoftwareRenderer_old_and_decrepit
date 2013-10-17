#ifndef SOFTWARERENDERER_TEXTURE_H
#define SOFTWARERENDERER_TEXTURE_H

struct Texture {
	INT textureId;
	Color4b ambient, diffuse, specular;
	FLOAT specLevel;
};

#endif