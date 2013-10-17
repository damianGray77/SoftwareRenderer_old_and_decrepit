#include "stdafx.h"
#include "Model.h"
#include "Vector.h"
#include "Vertex.h"
#include "Polygon.h"
#include "Globals.h"

Model::Model() {

}

Model::~Model() {

}

VOID Model::CalculateNormals(BOOL normalize) {
	INT *tblVertRefs, a, b, c;	
	Vector3 norm;
	Vertex3 v1, v2;

	tblVertRefs = new INT[numVerts];
	memset(tblVertRefs, 0, sizeof(INT) * numVerts);
	
	for(INT i = 0; i < numFaces; ++i) {
		a = polygons[i].verts[0];
		b = polygons[i].verts[1];
		c = polygons[i].verts[2];
		
		v1 = vertices[b].position - vertices[a].position;
		v2 = vertices[c].position - vertices[a].position;

		norm = Vector3::Cross(v2, v1);

		polygons[i].normal = norm;
		if(normalize) {
			polygons[i].normal = Vector3::Normal(polygons[i].normal);
		}

		vertices[a].normal += norm;
		vertices[b].normal += norm;
		vertices[c].normal += norm;

		++tblVertRefs[a];
		++tblVertRefs[b];
		++tblVertRefs[c];
	}

	for(int i = 0; i < numVerts; ++i) {
		vertices[i].normal /= (FLOAT)tblVertRefs[i];
		vertices[i].normal = Vector3::Normal(vertices[i].normal);
	}

	delete[] tblVertRefs;
}