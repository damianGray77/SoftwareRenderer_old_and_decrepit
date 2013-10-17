#include "stdafx.h"
#include "../Math/Matrix4x4.h"
#include "../Math/Matrix3x3.h"
#include "Color4f.h"
#include "Color4b.h"
#include "Texture.h"
#include "Vertex2.h"
#include "Vertex3.h"
#include "Vertex3c.h"
#include "Vector3.h"
#include "Polygon3.h"
#include "PVertex3c.h"
#include "Model.h"

Model::Model() { }

Model::~Model() { }

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