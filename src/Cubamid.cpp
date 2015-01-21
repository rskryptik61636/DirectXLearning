#include "Cubamid.h"
#include "Vertex.h"

Cubamid::Cubamid() : Object()
{}

Cubamid::~Cubamid()
{
};

void Cubamid::initVertexBuffer()
{
	// using SimpleVertex
	Object::vertexType = SIMPLE_VERTEX;//false;

	// initialize vertices
	mNumCubeVertices = 8;
	mNumPyramidVertices = 5;
	Object::mNumVertices = mNumCubeVertices + mNumPyramidVertices;	// 8 for box, 5 for pyramid
	Object::mSimpleVertexList.resize(mNumVertices);

	// define cube and pyramid vertex offsets
	mCubeFirstVertexPos = 0;
	mPyramidFirstVertexPos = mCubeFirstVertexPos + mNumCubeVertices;

	// define box vertices - front half
	mSimpleVertexList[0].position = D3DXVECTOR3(-1.0f,-1.0f,-1.0f);		// bottom-left
	mSimpleVertexList[0].color = CYAN;

	mSimpleVertexList[1].position = D3DXVECTOR3(-1.0f,+1.0f,-1.0f);		// top-left
	mSimpleVertexList[1].color = CYAN;

	mSimpleVertexList[2].position = D3DXVECTOR3(+1.0f,+1.0f,-1.0f);		// top-right
	mSimpleVertexList[2].color = CYAN;

	mSimpleVertexList[3].position = D3DXVECTOR3(+1.0f,-1.0f,-1.0f);		// bottom-right
	mSimpleVertexList[3].color = CYAN;

	// define box vertices - back half
	mSimpleVertexList[4].position = D3DXVECTOR3(-1.0f,-1.0f,+1.0f);		// bottom-left
	mSimpleVertexList[4].color = GREEN;

	mSimpleVertexList[5].position = D3DXVECTOR3(-1.0f,+1.0f,+1.0f);		// top-left
	mSimpleVertexList[5].color = GREEN;

	mSimpleVertexList[6].position = D3DXVECTOR3(+1.0f,+1.0f,+1.0f);		// top-right
	mSimpleVertexList[6].color = GREEN;

	mSimpleVertexList[7].position = D3DXVECTOR3(+1.0f,-1.0f,+1.0f);		// bottom-right
	mSimpleVertexList[7].color = GREEN;

	// define pyramid vertices
	//mPyramidFirstVertexPos = 8;
	mSimpleVertexList[8].position = D3DXVECTOR3(-1.0f,-1.0f,-1.0f);		// bottom-left
	mSimpleVertexList[8].color = WHITE;

	mSimpleVertexList[9].position = D3DXVECTOR3(-1.0f,-1.0f,+1.0f);		// top-left
	mSimpleVertexList[9].color = WHITE;

	mSimpleVertexList[10].position = D3DXVECTOR3(+1.0f,-1.0f,+1.0f);	// top-right
	mSimpleVertexList[10].color = WHITE;

	mSimpleVertexList[11].position = D3DXVECTOR3(+1.0f,-1.0f,-1.0f);	// bottom-right
	mSimpleVertexList[11].color = WHITE;

	mSimpleVertexList[12].position = D3DXVECTOR3(0.0f,+1.0f,0.0f);		// apex
	mSimpleVertexList[12].color = RED;
}

void Cubamid::initIndexBuffer()
{
	// initialize indices
	Object::mNumVerticesPerFace = 3;
	mNumCubeFaces = 12;
	mNumPyramidFaces = 6;
	Object::mNumFaces = mNumCubeFaces + mNumPyramidFaces;		// 12 for cube, 6 for pyramid

	// define the index list
	mCubeFirstIndexPos = 0;
	mPyramidFirstIndexPos = mCubeFirstIndexPos + (mNumCubeFaces*mNumVerticesPerFace);
	DWORD indices[] = {
		// cube - front face
		0, 1, 2,
		0, 2, 3,

		// cube - back face 
		4, 6, 5,
		4, 7, 6,

		// cube - left face
		4, 5, 1,
		4, 1, 0,

		// cube - right face
		3, 2, 6,
		3, 6, 7,

		// cube - top face
		1, 5, 6,
		1, 6, 2,

		// cube - bottom face
		4, 0, 3, 
		4, 3, 7,

		// pyramid - base
		0, 3, 1,
		3, 2, 1,

		// pyramid - front face
		0, 4, 3,

		// pyramid - left face
		0, 1, 4,

		// pyramid - back face
		1, 2, 4,

		// pyramid - right face
		3, 4, 2
	};

	// fill the index list
	Object::mIndexList.resize(mNumFaces * mNumVerticesPerFace);
	for(size_t i = 0; i < mIndexList.size(); ++i)
		mIndexList[i] = indices[i];
}

void Cubamid::draw()
{
	UINT stride = sizeof(SimpleVertex);		// since we're using SimpleVertex
	UINT offset = 0;
	mD3dDevice->IASetVertexBuffers(0,1,&mVb,&stride,&offset);
	mD3dDevice->IASetIndexBuffer(mIb,DXGI_FORMAT_R32_UINT,0);	
}

void Cubamid::drawCube()
{
	draw();
	mD3dDevice->DrawIndexed(mNumCubeFaces * mNumVerticesPerFace,mCubeFirstIndexPos,mCubeFirstVertexPos);	// draw the cube
}

void Cubamid::drawPyramid()
{
	draw();
	mD3dDevice->DrawIndexed(mNumPyramidFaces * mNumVerticesPerFace,mPyramidFirstIndexPos,mPyramidFirstVertexPos);	// draw the pyramid
}