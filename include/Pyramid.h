#ifndef PYRAMID_H
#define PYRAMID_H

#include "d3dUtil.h"

class Pyramid
{
public:
	Pyramid();
	~Pyramid();

	void init(ID3D10Device *device, float scale);
	void draw();

private:
	DWORD mNumVertices;
	DWORD mNumFaces;
	DWORD mNumVerticesPerFace;

	ID3D10Device *mD3DDevice;
	ID3D10Buffer *mVertexBuffer;
	ID3D10Buffer *mIndexBuffer;
};

#endif	// PYRAMID_H