// header file for TexturedWavesV1 which extends ObjectV2

#ifndef TEXTURED_WAVES_V1_H
#define TEXTURED_WAVES_V1_H

//#include "d3dUtil.h"
#include "ObjectV2.h"

class TexturedWavesV1 : public ObjectV2<TextureVertex>
{

public:
	TexturedWavesV1();
	virtual ~TexturedWavesV1();

	// overriding just to make sure the base class version doesn't get invoked
	virtual void init(ID3D11Device *device, float scale)	{}

	// actual init function
	virtual void init(ID3D11Device *device, DWORD m, DWORD n, float dx, float dt, float speed, float damping);
	
	// define for namesake as we don't actually use these functions
	virtual void initVertexBuffer()	{}
	virtual void initIndexBuffer()	{}

	// custom functions
	void update(float dt);
	void disturb(DWORD i, DWORD j, float magnitude);
	
private:
	DWORD mNumRows, mNumCols;

	// simulation constants
	float mK1, mK2, mK3;
	float mTimeStep, mSpatialStep;

	DXVector3 *mPrevSolution;
	DXVector3 *mCurrSolution;
	DXVector3 *mNormals;
};

#endif	// TEXTURED_WAVES_V1_H