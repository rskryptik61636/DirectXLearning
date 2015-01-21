// ColoredCubeApp class - header file

#ifndef COLORED_CUBE_APP_H
#define COLORED_CUBE_APP_H

#include "d3dApp.h"
#include "Box.h"

class ColoredCubeApp : public D3DApp
{
public:
	ColoredCubeApp(HINSTANCE hInstance);
	~ColoredCubeApp();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene();

private:
	void buildFX();
	void buildVertexLayouts();

private:
	Box mBox;

	ID3DX11Effect *mFx;
	ID3DX11EffectTechnique *mTech;
	ID3D11InputLayout *mVertexLayout;
	ID3DX11EffectMatrixVariable *mfxWVPVar;
	ID3DX11EffectScalarVariable *mfxGtimeVar;

	DXMatrix mView;
	DXMatrix mProj;
	DXMatrix mWVP;

	float mTheta;
	float mPhi;
};

#endif	// COLORED_CUBE_APP_H