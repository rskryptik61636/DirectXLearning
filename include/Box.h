// class to describe a box - adopted from Frank Luna's Direct3D tutorials

#ifndef BOX_H
#define BOX_H

#include "d3dUtil.h"

class Box
{
public:
	Box();
	~Box();

	void init(ID3D11Device *device, float scale);
	void draw();

private:
	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D11Device *mD3dDevice;	// d3d device
	ID3D11Buffer *mVb;			// vertex buffer
	ID3D11Buffer *mIb;			// index buffer
};

#endif