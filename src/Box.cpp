// Box class source file

#include "Box.h"
#include "Vertex.h"

Box::Box() : mNumVertices(0), mNumFaces(0), mD3dDevice(0), mVb(0), mIb(0)
{}

Box::~Box()
{
	ReleaseCOM(mVb);
	ReleaseCOM(mIb);
}

void Box::init(ID3D11Device *device, float scale)
{
	mD3dDevice = device;

	mNumVertices = 8;
	mNumFaces = 12;

	// set of vertices to be rendered
	Vertex vertices[] = 
	{
		{DXVector3(-1.0f,-1.0f,-1.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector2(0.0f,0.0f),WHITE},		// bottom face
		{DXVector3(-1.0f,+1.0f,-1.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector2(0.0f,0.0f),BLACK},
		{DXVector3(+1.0f,+1.0f,-1.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector2(0.0f,0.0f),RED},
		{DXVector3(+1.0f,-1.0f,-1.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector2(0.0f,0.0f),GREEN},
		
		{DXVector3(-1.0f,-1.0f,+1.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector2(0.0f,0.0f),BLUE},		// top face
		{DXVector3(-1.0f,+1.0f,+1.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector2(0.0f,0.0f),YELLOW},
		{DXVector3(+1.0f,+1.0f,+1.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector2(0.0f,0.0f),CYAN},
		{DXVector3(+1.0f,-1.0f,+1.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector2(0.0f,0.0f),MAGENTA}
	};

	// scale the box
	for(int i = 0; i < mNumVertices; ++i)
		vertices[i].position *= scale;

	// set the vertex buffer description
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	// create subresource data for the vertices
	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem = vertices;

	// create the vertex buffer
	HR(mD3dDevice->CreateBuffer(&vbd,&vInitData,&mVb));	

	// Create the index buffer

	DWORD indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3, 
		4, 3, 7
	};

	// set the index buffer description
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * mNumFaces * 3;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	// create subresource data for the index buffer
	D3D11_SUBRESOURCE_DATA iInitData;
	iInitData.pSysMem = indices;

	// create the index buffer
	HR(mD3dDevice->CreateBuffer(&ibd,&iInitData,&mIb));
}

void Box::draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11DeviceContext *pDeviceContext;
	mD3dDevice->GetImmediateContext(&pDeviceContext);
	pDeviceContext->IASetVertexBuffers(0, 1, &mVb, &stride, &offset);	// set the vertex buffer for position
	pDeviceContext->IASetVertexBuffers(1, 1, &mVb, &stride, &offset);	// set the vertex buffer for color
	pDeviceContext->IASetIndexBuffer(mIb, DXGI_FORMAT_R32_UINT, 0);	// set the index buffer
	pDeviceContext->DrawIndexed(mNumFaces * 3, 0, 0);					// draw the box
	pDeviceContext->Release();	// release as GetImmediateContext does a deep copy of the rendering context
}