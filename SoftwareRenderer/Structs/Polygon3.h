#ifndef SOFTWARERENDERER_POLYGON3_H
#define SOFTWARERENDERER_POLYGON3_H

struct Polygon3 {
	INT verts[3];
	Vector3	normal;
	Vertex2 uvs[3];
	Color4f cols[3];
	FLOAT cenZ;
	INT texID;
};

#endif