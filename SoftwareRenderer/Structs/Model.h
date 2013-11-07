#ifndef SOFTWARERENDERER_MODEL_H
#define SOFTWARERENDERER_MODEL_H

#include "Texture.h"
#include "Vertex3.h"
#include "Vertex3c.h"
#include "Vector3.h"
#include "Polygon3.h"
#include "Light.h"
#include "PVertex3c.h"

class Model {
public:
	VOID CalculateNormals(BOOL normalize);
	INT numVerts, numFaces, numLights;
	PVertex3c *vertices;
	PVertex3c *lights;
	Polygon3 *polygons;
	INT numVisible;
	INT *visible;
	INT	numTexts;
	Texture *textures;
	Vector3 rotation;
	Vertex3 position;
	Matrix4x4 projection;

	Model();
	~Model();
};

#endif