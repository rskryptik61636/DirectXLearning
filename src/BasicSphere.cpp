// source file for the basic sphere class

#include "BasicSphere.h"

// basic ctor
BasicSphere::BasicSphere()	: ObjectV2(12, 20, 3) //, mNumIcosahedronVertices(12), mNumIcosahedronFaces(20)	
{
	//ObjectV2::mNumVerticesPerFace = 3;
}

// dtor
BasicSphere::~BasicSphere()	{}

// initialize vertex buffer func
void BasicSphere::initVertexBuffer()
{
	// resize the vertex list
	ObjectV2::mVertexList.resize(ObjectV2::mNumVertices);

	// set the golden ratio and define the icosahedron vertices
	float a = 2.0f / (1.0f + sqrtf(5.0f));
	ObjectV2::mVertexList[0].position = DXVector3(0, a, -1);
	ObjectV2::mVertexList[1].position = DXVector3(-a, 1, 0);
	ObjectV2::mVertexList[2].position = DXVector3(a, 1, 0);
	ObjectV2::mVertexList[3].position = DXVector3(0, a, 1);
	ObjectV2::mVertexList[4].position = DXVector3(-1, 0, a);
	ObjectV2::mVertexList[5].position = DXVector3(0, -a, 1);
	ObjectV2::mVertexList[6].position = DXVector3(1, 0, a);
	ObjectV2::mVertexList[7].position = DXVector3(1, 0, -a);
	ObjectV2::mVertexList[8].position = DXVector3(0, -a, -1);
	ObjectV2::mVertexList[9].position = DXVector3(-1, 0, -a);
	ObjectV2::mVertexList[10].position = DXVector3(-a, -1, 0);
	ObjectV2::mVertexList[11].position = DXVector3(a, -1, 0);
	
	// set all vertices' colour to blue
	for(size_t i = 0; i < ObjectV2::mVertexList.size(); ++i)
		ObjectV2::mVertexList[i].color = BLUE;

	//// init the vertex list with the icosahedron vertices
	//for(size_t i = 0; i < mNumIcosahedronVertices; ++i)
	//{
	//	mVertexList[i].position = icosahedronVertices[i].position;
	//	mVertexList[i].color = icosahedronVertices[i].color;
	//}
}

// initialize index buffer func
void BasicSphere::initIndexBuffer()
{
	// resize the indices list
	ObjectV2::mIndexList.resize(ObjectV2::mNumFaces * ObjectV2::mNumVerticesPerFace);

	// define the icosahedron triangles
	ObjectV2::mIndexList[0] = 0, ObjectV2::mIndexList[1] = 1, ObjectV2::mIndexList[2] = 2;
	ObjectV2::mIndexList[3] = 3, ObjectV2::mIndexList[4] = 2, ObjectV2::mIndexList[5] = 1;
	ObjectV2::mIndexList[6] =	3, ObjectV2::mIndexList[7] = 4, ObjectV2::mIndexList[8] = 5;
	ObjectV2::mIndexList[9] = 3, ObjectV2::mIndexList[10] = 5, ObjectV2::mIndexList[11] = 6;
	ObjectV2::mIndexList[12] = 0, ObjectV2::mIndexList[13] = 7, ObjectV2::mIndexList[14] = 8;
	ObjectV2::mIndexList[15] = 0, ObjectV2::mIndexList[16] = 8, ObjectV2::mIndexList[17] = 9;
	ObjectV2::mIndexList[18] = 5, ObjectV2::mIndexList[19] = 10, ObjectV2::mIndexList[20] = 11;
	ObjectV2::mIndexList[21] = 8, ObjectV2::mIndexList[22] = 11, ObjectV2::mIndexList[23] = 10;
	ObjectV2::mIndexList[24] = 1, ObjectV2::mIndexList[25] = 9, ObjectV2::mIndexList[26] = 4;
	ObjectV2::mIndexList[27] = 10, ObjectV2::mIndexList[28] = 4, ObjectV2::mIndexList[29] = 9;
	ObjectV2::mIndexList[30] = 2, ObjectV2::mIndexList[31] = 6, ObjectV2::mIndexList[32] = 7;
	ObjectV2::mIndexList[33] = 11, ObjectV2::mIndexList[34] = 7, ObjectV2::mIndexList[35] = 6;
	ObjectV2::mIndexList[36] = 3, ObjectV2::mIndexList[37] = 1, ObjectV2::mIndexList[38] = 4;
	ObjectV2::mIndexList[39] = 3, ObjectV2::mIndexList[40] = 6, ObjectV2::mIndexList[41] = 2;
	ObjectV2::mIndexList[42] = 0, ObjectV2::mIndexList[43] = 9, ObjectV2::mIndexList[44] = 1;
	ObjectV2::mIndexList[45] = 0, ObjectV2::mIndexList[46] = 2, ObjectV2::mIndexList[47] = 7;
	ObjectV2::mIndexList[48] = 8, ObjectV2::mIndexList[49] = 10, ObjectV2::mIndexList[50] = 9;
	ObjectV2::mIndexList[51] = 8, ObjectV2::mIndexList[52] = 7, ObjectV2::mIndexList[53] = 11;
	ObjectV2::mIndexList[54] = 5, ObjectV2::mIndexList[55] = 4, ObjectV2::mIndexList[56] = 10;
	ObjectV2::mIndexList[57] = 5, ObjectV2::mIndexList[58] = 11, ObjectV2::mIndexList[59] = 6;
}

#if 0
// define icosahedron func
void BasicSphere::defineIcosahedron()
{
	// resize the icosahedron vertices list
	icosahedronVertices.resize(mNumIcosahedronVertices);

	// set the golden ratio and define the icosahedron vertices
	float a = 2.0f / (1.0f + sqrtf(5.0f));
	icosahedronVertices[0].position = DXVector3(0, a, -1);
	icosahedronVertices[1].position = DXVector3(-a, 1, 0);
	icosahedronVertices[2].position = DXVector3(a, 1, 0);
	icosahedronVertices[3].position = DXVector3(0, a, 1);
	icosahedronVertices[4].position = DXVector3(-1, 0, a);
	icosahedronVertices[5].position = DXVector3(0, -a, 1);
	icosahedronVertices[6].position = DXVector3(1, 0, a);
	icosahedronVertices[7].position = DXVector3(1, 0, -a);
	icosahedronVertices[8].position = DXVector3(0, -a, -1);
	icosahedronVertices[9].position = DXVector3(-1, 0, -a);
	icosahedronVertices[10].position = DXVector3(-a, -1, 0);
	icosahedronVertices[11].position = DXVector3(a, -1, 0);
	
	// set all vertices' colour to blue
	for(size_t i = 0; i < icosahedronVertices.size(); ++i)
		icosahedronVertices[i].color = BLUE;

	// resize the icosahedron indices list
	ObjectV2::mIndexList.resize(mNumIcosahedronFaces * ObjectV2::mNumVerticesPerFace);

	// define the icosahedron triangles
	ObjectV2::mIndexList[0] = 0, ObjectV2::mIndexList[1] = 1, ObjectV2::mIndexList[2] = 2;
	ObjectV2::mIndexList[3] = 3, ObjectV2::mIndexList[4] = 2, ObjectV2::mIndexList[5] = 1;
	ObjectV2::mIndexList[6] =	3, ObjectV2::mIndexList[7] = 4, ObjectV2::mIndexList[8] = 5;
	ObjectV2::mIndexList[9] = 3, ObjectV2::mIndexList[10] = 5, ObjectV2::mIndexList[11] = 6;
	ObjectV2::mIndexList[12] = 0, ObjectV2::mIndexList[13] = 7, ObjectV2::mIndexList[14] = 8;
	ObjectV2::mIndexList[15] = 0, ObjectV2::mIndexList[16] = 8, ObjectV2::mIndexList[17] = 9;
	ObjectV2::mIndexList[18] = 5, ObjectV2::mIndexList[19] = 10, ObjectV2::mIndexList[20] = 11;
	ObjectV2::mIndexList[21] = 8, ObjectV2::mIndexList[22] = 11, ObjectV2::mIndexList[23] = 10;
	ObjectV2::mIndexList[24] = 1, ObjectV2::mIndexList[25] = 9, ObjectV2::mIndexList[26] = 4;
	ObjectV2::mIndexList[27] = 10, ObjectV2::mIndexList[28] = 4, ObjectV2::mIndexList[29] = 9;
	ObjectV2::mIndexList[30] = 2, ObjectV2::mIndexList[31] = 6, ObjectV2::mIndexList[32] = 7;
	ObjectV2::mIndexList[33] = 11, ObjectV2::mIndexList[34] = 7, ObjectV2::mIndexList[35] = 6;
	ObjectV2::mIndexList[36] = 3, ObjectV2::mIndexList[37] = 1, ObjectV2::mIndexList[38] = 4;
	ObjectV2::mIndexList[39] = 3, ObjectV2::mIndexList[40] = 6, ObjectV2::mIndexList[41] = 2;
	ObjectV2::mIndexList[42] = 0, ObjectV2::mIndexList[43] = 9, ObjectV2::mIndexList[44] = 1;
	ObjectV2::mIndexList[45] = 0, ObjectV2::mIndexList[46] = 2, ObjectV2::mIndexList[47] = 7;
	ObjectV2::mIndexList[48] = 8, ObjectV2::mIndexList[49] = 10, ObjectV2::mIndexList[50] = 9;
	ObjectV2::mIndexList[51] = 8, ObjectV2::mIndexList[52] = 7, ObjectV2::mIndexList[53] = 11;
	ObjectV2::mIndexList[54] = 5, ObjectV2::mIndexList[55] = 4, ObjectV2::mIndexList[56] = 10;
	ObjectV2::mIndexList[57] = 5, ObjectV2::mIndexList[58] = 11, ObjectV2::mIndexList[59] = 6;
}
#endif	// 0