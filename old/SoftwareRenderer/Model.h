#ifndef SOFTWARERENDERER_MODEL_H
#define SOFTWARERENDERER_MODEL_H

#include "Vertex.h"
#include "Polygon.h"
#include "Texture.h"

class Model {
public:
	VOID CalculateNormals(BOOL normalize);
	INT numVerts, numFaces;
	PVertex3c *vertices;
	Polygon3 *polygons;
	INT numVisible;
	INT *visible;
	INT	numTexts;
	Texture *textures;

	Model();
	~Model();
};

#endif