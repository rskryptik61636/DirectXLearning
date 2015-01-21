#ifndef CUBAMID_H
#define CUBAMID_H

#include "Object.h"

class Cubamid : public Object
{
public:
	Cubamid();
	~Cubamid();

	void initVertexBuffer();
	void initIndexBuffer();
	//void init(ID3D10Device *device, float scale);
	void draw();
	void drawCube();
	void drawPyramid();

protected:
	// no. of cube and pyramid indices
	int mNumCubeVertices;
	int mNumPyramidVertices;
	int mNumCubeFaces;
	int mNumPyramidFaces;
	
	// first index positions of cube and pyramid
	int mCubeFirstIndexPos;
	int mPyramidFirstIndexPos;
	
	// first vertex positions of cube and pyramid
	int mCubeFirstVertexPos;
	int mPyramidFirstVertexPos;
};

#endif	// CUBAMID_H
