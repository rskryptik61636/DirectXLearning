#include "Pyramid.h"
#include "Vertex.h"

Pyramid::Pyramid() :	mNumVertices(0), mNumFaces(0), mNumVerticesPerFace(0), 
						mD3DDevice(0), mVertexBuffer(0), mIndexBuffer(0)
{}

Pyramid::~Pyramid()
{
	ReleaseCOM(mVertexBuffer);
	ReleaseCOM(mIndexBuffer);
}

void Pyramid::init(ID3D10Device *device, float scale)
{
	mD3DDevice = device;

	// set the no. of vertices and faces
	mNumVertices = 5;
	mNumFaces = 6;
	mNumVerticesPerFace = 3;

	// define the vertex list
	SimpleVertex vertices[] = 
	{
		{D3DXVECTOR3(-1.0f,-1.0f,-1.0f),GREEN},		// base bottom-left
		{D3DXVECTOR3(-1.0f,-1.0f,+1.0f),GREEN},		// base top-left
		{D3DXVECTOR3(+1.0f,-1.0f,+1.0f),GREEN},		// base top-right
		{D3DXVECTOR3(+1.0f,-1.0f,-1.0f),GREEN},		// base bottom-right

		{D3DXVECTOR3(0.0f,+1.0f,0.0f),RED}			// peak
	};

	// scale the pyramid vertices
	for(int i = 0; i < mNumVertices; ++i)
		vertices[i].position *= scale;

	// vertex buffer description
	D3D10_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D10_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(SimpleVertex) * mNumVertices;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	// create vertex subresource data
	D3D10_SUBRESOURCE_DATA vertexInitData;
	vertexInitData.pSysMem = vertices;

	// create the vertex buffer
	HR(mD3DDevice->CreateBuffer(&vertexBufferDesc,&vertexInitData,&mVertexBuffer));

	// define the triangle indices
	DWORD indices[] = 
	{
		// base
		0, 3, 1,
		3, 2, 1,

		// front face
		0, 4, 3,

		// left face
		0, 1, 4,

		// back face
		1, 2, 4,

		// right face
		3, 4, 2
	};

	// set the index buffer desc
	D3D10_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D10_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = (mNumFaces*mNumVerticesPerFace) * sizeof(DWORD);
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// create the index buffer subresource data
	D3D10_SUBRESOURCE_DATA indexInitData;
	indexInitData.pSysMem = indices;

	// create the index buffer
	HR(mD3DDevice->CreateBuffer(&indexBufferDesc,&indexInitData,&mIndexBuffer));
}

void Pyramid::draw()
{
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	mD3DDevice->IASetVertexBuffers(0,1,&mVertexBuffer,&stride,&offset);		// set the vertex buffer
	mD3DDevice->IASetIndexBuffer(mIndexBuffer,DXGI_FORMAT_R32_UINT,0);		// set the index buffer
	mD3DDevice->DrawIndexed(mNumFaces*mNumVerticesPerFace,0,0);				// draw the pyramid
}