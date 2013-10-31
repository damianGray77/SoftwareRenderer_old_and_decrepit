#include "stdafx.h"
#include "Model.h"

Model::Model() { }

Model::~Model() { }

VOID Model::CalculateNormals(BOOL normalize) {
	INT *tblVertRefs = new INT[numVerts];

	for(INT i = 0; i < numFaces; ++i) {
		INT a = polygons[i].verts[0];
		INT b = polygons[i].verts[1];
		INT c = polygons[i].verts[2];
		
		Vertex3 v1 = vertices[b].position - vertices[a].position;
		Vertex3 v2 = vertices[c].position - vertices[a].position;

		Vector3 norm = Vector3::Cross(v2, v1);

		polygons[i].normal = normalize
			? Vector3::Normal(norm)
			: norm
		;

		vertices[a].normal += polygons[i].normal;
		vertices[b].normal += polygons[i].normal;
		vertices[c].normal += polygons[i].normal;

		++tblVertRefs[a];
		++tblVertRefs[b];
		++tblVertRefs[c];
	}

	for(int i = 0; i < numVerts; ++i) {
		vertices[i].normal /= (FLOAT)tblVertRefs[i];
		if(normalize) {
			vertices[i].normal = Vector3::Normal(vertices[i].normal);
		}
	}

	delete[] tblVertRefs;
}