#ifndef SOFTWARERENDERER_PVERTEX3C_H
#define SOFTWARERENDERER_PVERTEX3C_H

#include "Color4f.h"
#include "Vertex2.h"
#include "Vertex3.h"
#include "Vertex3c.h"
#include "Vector3.h"
#include "Light.h"

class PVertex3c {
public:
	Vertex3 position;
	Vertex3 posWorld;
	Vertex3 posScreen;
	Vertex2 uvMap;
	Vector3 normal;
	Color4f color;

	static const VOID Copy(PVertex3c &v1, const PVertex3c &v2);
	static const VOID Copy(PVertex3c &v1, const Light &l);
	static const VOID Copy(PVertex3c &v1, const Vertex3c &v2);
};

#endif