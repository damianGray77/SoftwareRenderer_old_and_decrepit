#ifndef SOFTWARERENDERER_PVERTEX3_H
#define SOFTWARERENDERER_PVERTEX3_H

class PVertex3 {
public:
	Vertex3 position;
	Vertex3 posTrans;
	Vector3 normal;
    Vertex2 uvMap;
};

#endif