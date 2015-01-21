#ifndef COLORED_PYRAMID_APP_H
#define COLORED_PYRAMID_APP_H

#include "d3dApp.h"
//#include "Pyramid.h"
#include "Cubamid.h"

class ColoredPyramidApp : public D3DApp
{
public:
	ColoredPyramidApp(HINSTANCE hInstance);
	~ColoredPyramidApp();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene();

private:
	void buildFX();
	void buildVertexLayouts();

private:
	//Pyramid mPyramid;
	Cubamid mCubamid;

	ID3D10Effect *mFx;
	ID3D10EffectTechnique *mTech;
	ID3D10InputLayout *mVertexLayout;
	ID3D10EffectMatrixVariable *mfxWVPVar;

	ID3D10RasterizerState *mWireframeRS;
	ID3D10RasterizerState *mSolidRS;

	D3DXMATRIX mWorld;
	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;

	float mTheta;
	float mPhi;
};

#endif	// COLORED_PYRAMID_APP_H