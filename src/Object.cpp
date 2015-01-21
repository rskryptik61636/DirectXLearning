// source file for generic Object class

#include "Object.h"
//#include "Vertex.h"

Object::Object() : mNumVertices(0), mNumFaces(0), mNumVerticesPerFace(0), mD3dDevice(0), mVb(0), mIb(0)
{}

Object::~Object()
{
	ReleaseCOM(mVb);
	ReleaseCOM(mIb);
}

void Object::init(ID3D11Device *device, float scale)
{
	initVertexBuffer();	// initialize the vertex list
	initIndexBuffer();	// initialize the index list

	mD3dDevice = device;

	//mNumVertices = 8;
	//mNumFaces = 12;

	//// set of vertices to be rendered
	//Vertex vertices[] = 
	//{
	//	{DXVector3(-1.0f,-1.0f,-1.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector2(0.0f,0.0f),WHITE},		// bottom face
	//	{DXVector3(-1.0f,+1.0f,-1.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector2(0.0f,0.0f),BLACK},
	//	{DXVector3(+1.0f,+1.0f,-1.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector2(0.0f,0.0f),RED},
	//	{DXVector3(+1.0f,-1.0f,-1.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector2(0.0f,0.0f),GREEN},
	//	
	//	{DXVector3(-1.0f,-1.0f,+1.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector2(0.0f,0.0f),BLUE},		// top face
	//	{DXVector3(-1.0f,+1.0f,+1.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector2(0.0f,0.0f),YELLOW},
	//	{DXVector3(+1.0f,+1.0f,+1.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector2(0.0f,0.0f),CYAN},
	//	{DXVector3(+1.0f,-1.0f,+1.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector3(0.0f,0.0f,0.0f),DXVector2(0.0f,0.0f),MAGENTA}
	//};


	// scale the object
	switch(vertexType)
	{

	case SIMPLE_VERTEX:
	for(size_t i = 0; i < mSimpleVertexList.size(); ++i)
		mSimpleVertexList[i].position *= scale;
	break;

	case COMPLEX_VERTEX:
		for(size_t i = 0; i < mVertexList.size(); ++i)
			mVertexList[i].position *= scale;
		break;
	}

	// set the vertex buffer description
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	switch(vertexType)
	{
	case SIMPLE_VERTEX:
		vbd.ByteWidth = sizeof(SimpleVertex) * mNumVertices;
		break;

	case COMPLEX_VERTEX:
		vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
		break;
	}
	//vbd.ByteWidth = vertexType ? (sizeof(Vertex)) : (sizeof(SimpleVertex)) * mNumVertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	// create subresource data for the vertices
	D3D11_SUBRESOURCE_DATA vInitData;
	switch(vertexType)
	{
	case SIMPLE_VERTEX:
		vInitData.pSysMem = reinterpret_cast<void*>(&mSimpleVertexList[0]);
		break;

	case COMPLEX_VERTEX:
		vInitData.pSysMem = reinterpret_cast<void*>(&mVertexList[0]);
	}
	//vInitData.pSysMem = vertexType ? (void*)&mVertexList[0] : (void*)&mSimpleVertexList[0];
	
	// create the vertex buffer
	HR(mD3dDevice->CreateBuffer(&vbd,&vInitData,&mVb));	

	// Create the index buffer

	//DWORD indices[] = {
	//	// front face
	//	0, 1, 2,
	//	0, 2, 3,

	//	// back face
	//	4, 6, 5,
	//	4, 7, 6,

	//	// left face
	//	4, 5, 1,
	//	4, 1, 0,

	//	// right face
	//	3, 2, 6,
	//	3, 6, 7,

	//	// top face
	//	1, 5, 6,
	//	1, 6, 2,

	//	// bottom face
	//	4, 0, 3, 
	//	4, 3, 7
	//};

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

void Object::draw()
{
	UINT stride;// = vertexType ? sizeof(Vertex) : sizeof(SimpleVertex);
	switch(vertexType)
	{
	case SIMPLE_VERTEX:
		stride = sizeof(SimpleVertex);
		break;

	case COMPLEX_VERTEX:
		stride = sizeof(Vertex);
		break;
	}
	UINT offset = 0;

	// get the rendering context of the device
	ID3D11DeviceContext *mD3dDeviceContext;
	mD3dDevice->GetImmediateContext(&mD3dDeviceContext);
	mD3dDeviceContext->IASetVertexBuffers(0, 1, &mVb, &stride, &offset);	// set the vertex buffer for position
	//mD3dDevice->IASetVertexBuffers(1,1,&mVb,&stride,&offset);	// set the vertex buffer for color
	mD3dDeviceContext->IASetIndexBuffer(mIb, DXGI_FORMAT_R32_UINT, 0);	// set the index buffer
	mD3dDeviceContext->DrawIndexed(mNumFaces*mNumVerticesPerFace, 0, 0);					// draw the box
	mD3dDeviceContext->Release();	// release the resource since a deep copy of the immediate context is made into it
}