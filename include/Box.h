// class to describe a box - adopted from Frank Luna's Direct3D tutorials

#ifndef BOX_H
#define BOX_H

#include "d3dUtil.h"

class Box
{
public:
	Box();
	~Box();

	void init(const DevicePtr &device, float scale);
	void setIndexAndVertexBuffers();
	void draw();

private:
	DWORD mNumVertices;
	DWORD mNumFaces;

	DevicePtr mD3dDevice;	// d3d device
	DeviceContextPtr mD3dDeviceContext;	// d3d device context
	BufferPtr mVb;			// vertex buffer
	BufferPtr mIb;			// index buffer
};
typedef std::unique_ptr<Box> BoxPtr;

#endif