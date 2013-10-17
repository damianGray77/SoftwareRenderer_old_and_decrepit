#ifndef SOFTWARERENDERER_TEXTURE_H
#define SOFTWARERENDERER_TEXTURE_H

class Texture {
public:
	INT textureId;
	Color4b ambient, diffuse, specular;
	FLOAT specLevel;
};

#endif