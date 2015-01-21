//=======================================================================================
// Waves.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef WAVES_H
#define WAVES_H

#include "d3dUtil.h"

class Waves
{
public:
	Waves();
	~Waves();

 
	void init(ID3D11Device* device, DWORD m, DWORD n, float dx, float dt, float speed, float damping);
	void update(float dt);
	void disturb(DWORD i, DWORD j, float magnitude);
	void draw();

private:
	DWORD mNumRows;
	DWORD mNumCols;

	DWORD mNumVertices;
	DWORD mNumFaces;

	// Simulation constants we can precompute.
	float mK1;
	float mK2;
	float mK3;

	float mTimeStep;
	float mSpatialStep;

	DXVector3* mPrevSolution;
	DXVector3* mCurrSolution;
	DXVector3* mNormals;

	ID3D11Device* md3dDevice;
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;
};

#endif // WAVES_H