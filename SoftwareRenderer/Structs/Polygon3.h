#ifndef SOFTWARERENDERER_POLYGON3_H
#define SOFTWARERENDERER_POLYGON3_H

#include "Vector3.h"
#include "Vertex2.h"
#include "Color4f.h"

struct Polygon3 {
	INT verts[3];
	Vector3	normal;
	Vertex2 uvs[3];
	Color4f cols[3];
	FLOAT cenZ;
	INT texID;
};

#endif