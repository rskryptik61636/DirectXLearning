// source file for the SimpleQuad class
#include "SimpleQuad.h"

// param ctor 1, takes in the 4 quad points in CW order as well as the color of each point
SimpleQuad::SimpleQuad(const std::vector<SimpleVertex> pts) : m_pts(pts), ObjectV2<SimpleVertex>(pts.size(), 2, 3)
{}

// define vertex buffer
void SimpleQuad::initVertexBuffer()
{
	// make sure we have exactly 4 points
	_ASSERTE(m_pts.size() == 4);

	// copy m_pts to mVertexList
	mVertexList = m_pts;	
}

// define index buffer
void SimpleQuad::initIndexBuffer()
{
	// the quad will be composed of 2 triangles
	// 1) verts 0, 1, 2
	// 2) verts 0, 2, 3
	mIndexList.resize(mNumFaces * mNumVerticesPerFace);
	mIndexList[0] = 0;	mIndexList[1] = 1;	mIndexList[2] = 2;
	mIndexList[3] = 0;	mIndexList[4] = 2;	mIndexList[5] = 3;
}

// override the init method to make the vertex buffer dynamic
void SimpleQuad::init(ID3D11Device *device, float scale)
{
	vertexScale = scale;

	initVertexBuffer();	// initialize the vertex list
	initIndexBuffer();	// initialize the index list

	mD3dDevice = device;
	mD3dDevice->GetImmediateContext(&mD3dDeviceContext.p);

	// NOTE: following makes an assumption that Vertex type VertexT has a member called position
	// this should not be the case (UPDATE: renamed all Vertex members named 'pos' to 'position')
#if 1
	// scale the object iff the scale is not equal to 1
	if (scale != 1)
	{
		for (size_t i = 0; i < mVertexList.size(); ++i)
			mVertexList[i].position *= scale;
	}	
#endif	// 1

	// set the vertex buffer description (make it dynamic, so that the color can be updated at runtime)
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(SimpleVertex)* mNumVertices;

	//vbd.ByteWidth = vertexType ? (sizeof(Vertex)) : (sizeof(SimpleVertex)) * mNumVertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;

	// create subresource data for the vertices
	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem = reinterpret_cast<void*>(&mVertexList[0]);
	//vInitData.pSysMem = vertexType ? (void*)&mVertexList[0] : (void*)&mSimpleVertexList[0];

	// create the vertex buffer
	HR(mD3dDevice->CreateBuffer(&vbd, &vInitData, &mVb));

	// Create the index buffer
	// set the index buffer description
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD)* mNumFaces * mNumVerticesPerFace;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	// create subresource data for the index buffer
	D3D11_SUBRESOURCE_DATA iInitData;
	iInitData.pSysMem = &mIndexList[0];
	//iInitData.pSysMem = indices;

	// create the index buffer
	HR(mD3dDevice->CreateBuffer(&ibd, &iInitData, &mIb));
}

// updates the color of all the points with the given color
void SimpleQuad::updateColor(const DXColor color)
{
	// map the vertex buffer to process space
	D3D11_MAPPED_SUBRESOURCE verts;
	HR(mD3dDeviceContext->Map(mVb, 0, D3D11_MAP_WRITE_DISCARD, 0, &verts));

	// update the color of all the vertices
	SimpleVertex *pVert = reinterpret_cast<SimpleVertex*>(verts.pData);
	for (std::size_t i = 0; i < mVertexList.size(); ++i, ++pVert)
	{
		pVert->position = mVertexList[i].position;
		pVert->color = color;
	}
		
		//mVertexList[i].color = color;

	// unmap the vertex buffer
	mD3dDeviceContext->Unmap(mVb, 0);
}