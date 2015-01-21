// header file for generic Object class

#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include "d3dUtil.h"
#include "Vertex.h"

struct SimpleVertex;
struct Vertex;

class Object
{
public:
	Object();
	virtual ~Object();

	virtual void initVertexBuffer() = 0;		// pure virtual func to define vertex list
	virtual void initIndexBuffer() = 0;			// pure virtual func to define index list
	virtual void init(ID3D11Device *device, float scale);

//protected:
	virtual void draw();
	
protected:
	DWORD mNumVertices;
	DWORD mNumFaces;
	DWORD mNumVerticesPerFace;

	//bool vertexType;			// 0 = SimpleVertex, 1 = Vertex
	VertexType vertexType;
	std::vector<Vertex> mVertexList;
	std::vector<SimpleVertex> mSimpleVertexList;
	std::vector<DWORD> mIndexList;

	ID3D11Device *mD3dDevice;	// d3d device
	ID3D11Buffer *mVb;			// vertex buffer
	ID3D11Buffer *mIb;			// index buffer
};

#endif	// OBJECT_H