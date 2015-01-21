// source file for the LitSphere class

#include "LitSphere.h"

#include <numeric>
#include <functional>
#include <algorithm>

// default ctor
LitSphere::LitSphere() : ObjectV2(12, 20, 3)	{}

// dtor
LitSphere::~LitSphere()	{}

// init vertex buffer func
void LitSphere::initVertexBuffer()
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
	{
		ObjectV2::mVertexList[i].normal = DXVector3(0.0f, 0.0f, 0.0f);	// to be computed later
		ObjectV2::mVertexList[i].diffuse = BLUE;
		ObjectV2::mVertexList[i].spec = WHITE;
		//ObjectV2::mVertexList[i].color = BLUE;
	}
}

// init index buffer func
void LitSphere::initIndexBuffer()
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

#if 0
	// compute the normals of each vertex
	computeNormals();
#endif	// 0
}

// compute normals func
void LitSphere::computeNormals()
{
	// compute normals as if each vertex was on the surface of a sphere
	DXVector3 sphereCenter(0.0f, 0.0f, 0.0f);
	for(size_t i = 0; i < ObjectV2::mVertexList.size(); ++i)
	{
		mVertexList[i].normal = DirectX::XMVector3Normalize(DXVector3(mVertexList[i].position - sphereCenter));
		//D3DXVec3Normalize(&mVertexList[i].normal, &(mVertexList[i].position - sphereCenter));
	}

#if 0

	// for each vertex in the vertex list, compute the weighted average of all normals
	// -based on the average of all the triangle areas of which the vertex is a part
	std::vector<DXVector3> normals;
	std::vector<float> areas;
	//std::vector<DXVector3> crossProducts;
	for(size_t i = 0; i < /*ObjectV2::*/mVertexList.size(); ++i)
	{
		for(size_t j = 0; j < /*ObjectV2::*/mIndexList.size(); j+=3)
		{
			DWORD a(/*ObjectV2::*/mIndexList[j]), b(/*ObjectV2::*/mIndexList[j+1]), c(/*ObjectV2::*/mIndexList[j+2]);

			// check if the i'th vertex is a member of the j'th triangle
			if(a == i || b == i || c == i)
			{
				DXVector3 e1(/*ObjectV2::*/mVertexList[b].position - /*ObjectV2::*/mVertexList[a].position);	// e1 = b - a
				DXVector3 e2(/*ObjectV2::*/mVertexList[c].position - /*ObjectV2::*/mVertexList[a].position);	// e2 = c - a

				// compute the cross product and push it into the normals list
				DXVector3 cross;
				D3DXVec3Cross(&cross, &e1, &e2);
				normals.push_back(*D3DXVec3Normalize(&cross,&cross));
				areas.push_back(D3DXVec3Length(&cross) / 2.0f);	// apparently length(tri_normal)/2 = tri_area
			}
		}

		// compute the weighted area of the i'th vertex
		float totalArea = std::accumulate(areas.begin(), areas.end(), 0.0f);	// total area of all triangles sharing the i'th vertex
		std::transform(areas.begin(), areas.end(), areas.begin(), std::bind2nd(std::divides<float>(), totalArea));

		DXVector3 currNormal(0.0f, 0.0f, 0.0f);
		for(size_t k = 0; k < areas.size(); ++k)
		{
			currNormal += areas[k] * normals[k];
		}
		D3DXVec3Normalize(&/*ObjectV2::*/mVertexList[i].normal, &currNormal);

		// clear out normals and areas
		normals.clear();
		areas.clear();
	}

#endif	// 0
}

// init function
void LitSphere::init(ID3D11Device *device, float scale)
{
	vertexScale = scale;

	initVertexBuffer();	// initialize the vertex list
	initIndexBuffer();	// initialize the index list

	computeNormals();				// compute the vertex normals

	mD3dDevice = device;

	// NOTE: following makes an assumption that Vertex type VertexT has a member called position
	// this should not be the case (UPDATE: renamed all Vertex members named 'pos' to 'position')
#if 1
	// scale the object
	for(size_t i = 0; i < mVertexList.size(); ++i)
		mVertexList[i].position *= scale;
#endif	// 1
		
	// set the vertex buffer description
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(LightVertex) * mNumVertices;
		
	//vbd.ByteWidth = vertexType ? (sizeof(Vertex)) : (sizeof(SimpleVertex)) * mNumVertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	// create subresource data for the vertices
	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem = reinterpret_cast<void*>(&mVertexList[0]);
	//vInitData.pSysMem = vertexType ? (void*)&mVertexList[0] : (void*)&mSimpleVertexList[0];
	
	// create the vertex buffer
	HR(mD3dDevice->CreateBuffer(&vbd,&vInitData,&mVb));	

	// Create the index buffer
	// set the index buffer description
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * mNumFaces * mNumVerticesPerFace;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	// create subresource data for the index buffer
	D3D11_SUBRESOURCE_DATA iInitData;
	iInitData.pSysMem = &mIndexList[0];
	//iInitData.pSysMem = indices;

	// create the index buffer
	HR(mD3dDevice->CreateBuffer(&ibd,&iInitData,&mIb));
}