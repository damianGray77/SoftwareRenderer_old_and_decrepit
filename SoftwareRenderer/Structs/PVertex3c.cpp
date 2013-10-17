#include "stdafx.h"
#include "../Math/Matrix4x4.h"
#include "../Math/Matrix3x3.h"
#include "Color4f.h"
#include "Vertex2.h"
#include "Vertex3.h"
#include "Vertex3c.h"
#include "Vector3.h"
#include "PVertex3c.h"

const VOID PVertex3c::Copy(PVertex3c &v1, const PVertex3c &v2) {
	PVertex3c res = {
		{
			v2.position.x
			, v2.position.y
			, v2.position.z
		}, {
			v2.posTrans.x
			, v2.posTrans.y
			, v2.posTrans.z
		}, {
			v2.posProj.x
			, v2.posProj.y
			, v2.posProj.z
		}, {
			v2.uvMap.x
			, v2.uvMap.y
		}, {
			v2.normal.x
			, v2.normal.y
			, v2.normal.z
		}
		, {
			v2.color.r
			, v2.color.g
			, v2.color.b
			, v2.color.a
		}
	};
	v1 = res;
}

const VOID PVertex3c::Copy(PVertex3c &v1, const Vertex3c &v2) {
	PVertex3c res = {
		{ v2.x, v2.y, v2.z, }
		, { 0.0f, 0.0f, 0.0f }
		, { 0.0f, 0.0f, 0.0f }
		, { v2.u, v2.v }
		, { 0.0f, 0.0f, 0.0f }
		, { v2.c.r, v2.c.g, v2.c.b, v2.c.a }
	};
	v1 = res;
}