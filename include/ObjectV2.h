#ifndef OBJECT_V2_H
#define OBJECT_V2_H

#include <vector>
#include "d3dUtil.h"
#include "Vertex.h"

template<typename VertexT>
class ObjectV2
{
public:
	ObjectV2() : 
		mNumVertices(0), mNumFaces(0), mNumVerticesPerFace(0), 
		mD3dDevice(0), mVb(0), mIb(0)	{}

	ObjectV2(DWORD nVerts, DWORD nFaces, DWORD nVertsPerFace)	:
		mNumVertices(nVerts), mNumFaces(nFaces), mNumVerticesPerFace(nVertsPerFace),
		mD3dDevice(0), mVb(0), mIb(0)	{}

	virtual ~ObjectV2()
	{
		ReleaseCOM(mVb);
		ReleaseCOM(mIb);
		mD3dDeviceContext.Release();
	}

	virtual void initVertexBuffer() = 0;		// pure virtual func to define vertex list
	virtual void initIndexBuffer() = 0;			// pure virtual func to define index list

	// NOTE: The scale param is actually useless and unnecessary as we really shouldn't
	//		 be scaling the positions of the vertices in the buffer itself.
	//
	//		 Specifying a default scale value of 1.0f just so that clients don't have to
	//		 specify it if they don't want to.
	virtual void init(ID3D11Device *device, float scale = 1.0f);	
	
//protected:
	virtual void draw();
	
	virtual D3D11_INPUT_ELEMENT_DESC* GetVertexElementDesc()
	{
		return VertexT::GetElementDesc();
	}

	// Sets the vertex and index buffers of the model in the IA stage of the rendering pipeline.
	virtual void setVertexAndIndexBuffers();

protected:

	// Helper function which creates the vertex and index buffers.
	virtual void createBuffers();
	
protected:
	DWORD mNumVertices;
	DWORD mNumFaces;
	DWORD mNumVerticesPerFace;

	//bool vertexType;			// 0 = SimpleVertex, 1 = Vertex
	//VertexType vertexType;
	std::vector<VertexT> mVertexList;
	/*std::vector<Vertex> mVertexList;
	std::vector<SimpleVertex> mSimpleVertexList;*/
	std::vector<DWORD> mIndexList;

	ID3D11Device *mD3dDevice;	// d3d device
	DeviceContextPtr mD3dDeviceContext;	// d3d device context
	ID3D11Buffer *mVb;			// vertex buffer
	ID3D11Buffer *mIb;			// index buffer

	float vertexScale;
};

template<typename VertexT>
void ObjectV2<VertexT>::init(ID3D11Device *device, float scale /*= 1.0f*/)
{
	vertexScale = scale;

	initVertexBuffer();	// initialize the vertex list
	initIndexBuffer();	// initialize the index list

	mD3dDevice = device;
	mD3dDevice->GetImmediateContext(&mD3dDeviceContext.p);

	// NOTE: shouldn't be scaling the vertex positions in the buffer
	// as it will be handled as the modelling transformation on this object.
#if 0
	// scale the object
	for(size_t i = 0; i < mVertexList.size(); ++i)
		mVertexList[i].position *= scale;
#endif	// 0
	
	// Create the vertex and index buffers
	createBuffers();

	// @TODO: remove when done testing
#if 0
	// set the vertex buffer description
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexT) * mNumVertices;
		
	//vbd.ByteWidth = vertexType ? (sizeof(Vertex)) : (sizeof(SimpleVertex)) * mNumVertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	// create subresource data for the vertices
	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem = reinterpret_cast<void*>(mVertexList.data());
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
	iInitData.pSysMem = mIndexList.data();
	//iInitData.pSysMem = indices;

	// create the index buffer
	HR(mD3dDevice->CreateBuffer(&ibd,&iInitData,&mIb));
#endif	// 0

}

// Sets the vertex and index buffers of the model in the IA stage of the rendering pipeline.
template<typename VertexT>
void ObjectV2<VertexT>::setVertexAndIndexBuffers()
{
	UINT stride = sizeof(VertexT);// = vertexType ? sizeof(Vertex) : sizeof(SimpleVertex);
	UINT offset = 0;

	// get the immediate rendering context of the device
	mD3dDeviceContext->IASetVertexBuffers(0, 1, &mVb, &stride, &offset);	// set the vertex buffer for position
	//mD3dDevice->IASetVertexBuffers(1,1,&mVb,&stride,&offset);	// set the vertex buffer for color
	mD3dDeviceContext->IASetIndexBuffer(mIb, DXGI_FORMAT_R32_UINT, 0);	// set the index buffer
}

template<typename VertexT>
void ObjectV2<VertexT>::draw()
{
	setVertexAndIndexBuffers();
	mD3dDeviceContext->DrawIndexed(mIndexList.size(), 0, 0);					// draw the box
	//mD3dDeviceContext->DrawIndexed(mNumFaces*mNumVerticesPerFace, 0, 0);					// draw the box	
}

// Helper function which creates the vertex and index buffers.
template<typename VertexT>
void ObjectV2<VertexT>::createBuffers()
{
	// set the vertex buffer description
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexT)* mVertexList.size();
	//vbd.ByteWidth = sizeof(VertexT)* mNumVertices;	// @TODO: remove when done testing

	//vbd.ByteWidth = vertexType ? (sizeof(Vertex)) : (sizeof(SimpleVertex)) * mNumVertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	// create subresource data for the vertices
	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem = reinterpret_cast<void*>(mVertexList.data());
	//vInitData.pSysMem = vertexType ? (void*)&mVertexList[0] : (void*)&mSimpleVertexList[0];

	// create the vertex buffer
	HR(mD3dDevice->CreateBuffer(&vbd, &vInitData, &mVb));

	// Create the index buffer
	// set the index buffer description
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD)* mIndexList.size();
	//ibd.ByteWidth = sizeof(DWORD)* mNumFaces * mNumVerticesPerFace;	// @TODO: remove when done testing
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	// create subresource data for the index buffer
	D3D11_SUBRESOURCE_DATA iInitData;
	iInitData.pSysMem = mIndexList.data();
	//iInitData.pSysMem = indices;

	// create the index buffer
	HR(mD3dDevice->CreateBuffer(&ibd, &iInitData, &mIb));
}

#endif	// OBJECT_V2_H