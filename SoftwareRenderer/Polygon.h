#ifndef SOFTWARERENDERER_POLYGON_H
#define SOFTWARERENDERER_POLYGON_H

struct Polygon3 {
	int verts[3];
	Vector3	normal;
	float cenZ;
	int texID;
};

#endif