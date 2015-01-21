//=======================================================================================
// PeaksAndValleys.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef PEAKSANDVALLEYS_H
#define PEAKSANDVALLEYS_H

#include "d3dUtil.h"

class PeaksAndValleys
{
public:
	PeaksAndValleys();
	~PeaksAndValleys();

	float getHeight(float x, float z)const;
 
	void init(ID3D11Device* device, DWORD m, DWORD n, float dx);
	void update(float dt);
	void draw();

private:
	DWORD mNumRows;
	DWORD mNumCols;

	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D11Device* md3dDevice;
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;
};

#endif // PEAKSANDVALLEYS_H