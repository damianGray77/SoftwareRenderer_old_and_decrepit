#ifndef SOFTWARERENDERER_TEXTURE_H
#define SOFTWARERENDERER_TEXTURE_H

struct Texture {
	CHAR name[64];
	Color4b ambient, diffuse, specular;
	FLOAT specLevel;
};

#endif