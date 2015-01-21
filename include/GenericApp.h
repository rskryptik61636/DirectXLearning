// header file for Generic App class

#ifndef GENERIC_APP_H
#define GENERIC_APP_H

#include <string>
#include <memory>
#include <atlbase.h>

#include "d3dApp.h"
#include "Light.h"
#include "Vertex.h"

#include "DXCamera.h"

class GenericApp : public D3DApp
{
public:
	GenericApp(HINSTANCE hInstance);
	GenericApp(HINSTANCE hInstance, std::wstring shaderFilename);
	virtual ~GenericApp();

	virtual void initApp();
	virtual void onResize();
	virtual void updateScene(float dt);
	virtual void drawScene();
	
protected:
	virtual void createResources() = 0;
	virtual void createObjects() = 0;
	virtual void drawObjects() = 0;

	virtual void setShaderVariables() = 0;
	virtual void getShaderVariables() = 0;

	virtual void buildFX();
	virtual void buildVertexLayouts();

protected:
	// define objects variables here
	
	// define lights variables here

	// common variables
	EffectPtr mFX;
	EffectTechniquePtr mTech;
	InputLayoutPtr mVertexLayout;
	/*ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3D11InputLayout* mVertexLayout;*/
	
	// define shader variables and shader resource variables here

	// define world matrix here

	// common pipeline matrices
	DXMatrix mView;
	DXMatrix mProj;
	DXMatrix mWVP;

	// other misc common variables
	DXVector3 mEyePos;
	float mRadius;
	float mTheta;
	float mPhi;
	std::wstring mShaderFilename;
	VertexType mPrimaryVertexType;

	// camera class object
	DXCameraPtr mCamera;
};

#endif	// GENERIC_APP_H