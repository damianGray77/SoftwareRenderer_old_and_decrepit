#ifndef SOFTWARERENDERER_MODEL_H
#define SOFTWARERENDERER_MODEL_H

class Model {
public:
	VOID CalculateNormals(BOOL normalize);
	INT numVerts, numFaces;
	PVertex3c *vertices;
	Polygon3 *polygons;
	INT numVisible;
	INT *visible;
	INT	numMats;
	Texture *tex;

	Model();
	~Model();
};

#endif