#ifndef SOFTWARERENDERER_PVERTEX3C_H
#define SOFTWARERENDERER_PVERTEX3C_H

class PVertex3c {
public:
	Vertex3 position;
	Vertex3 posTrans;
	Vertex3c posProj;
	Vertex2 uvMap;
	Vector3 normal;
	Color4f color;

	static const VOID Copy(PVertex3c &v1, const PVertex3c &v2);
	static const VOID Copy(PVertex3c &v1, const Vertex3c &v2);
};

#endif