// header file for the MirrorApp demo which experiments with the stencil buffer

#ifndef MIRROR_APP_H
#define MIRROR_APP_H

// base class of MirrorApp
#include "DXApp.h"

// scene object classes
#include "RoomV1.h"
#include "TextureBox.h"
#include "SimpleQuad.h"

// TexEffect class
#include "TexEffect.h"

// ColorEffect class
#include "ColorEffect.h"

// MirrorApp demo: implements a mirror reflection using the depth-stencil buffer
class MirrorApp : public DXApp
{
public:

	// Parameterized ctor, initializes the MirrorApp demo
	MirrorApp(HINSTANCE hInstance, const std::string strSceneFilePath);

	// Dtor
	~MirrorApp();

	// @TODO: add public methods here

public:

	// @TODO: add public members here

protected:

	// Define resources such as textures, blend/rasterizer/depth-stencil states and so on
	virtual void createResources();

	// Define scene objects which need to be rendered
	virtual void createObjects();

	// Meat of the drawScene method, this is where the scene rendering is implemented
	virtual void drawObjects();

	// Define to build the shaders which will be used by the application
	virtual void buildShaders();

	// Define to build the vertex layout which will be bound to the input assembly stage
	virtual void buildVertexLayouts();

	// TODO: remove when done testing
#if 0
	// Define to set the constant buffer in the vertex shader during the scene render
	void setVSConstBuf(const DXMatrix &texMtx, const DXMatrix &worldMtx, const DXMatrix &wvpMtx);

	// Define to set the constant buffer in the pixel shader during the scene render
	void setPSConstBuf(const Light &light, const DXVector3 &eyePosW, const float gCurrTime);

	// Updates the vertex shader's constant buffer and its resources
	void updateVertexShader(const DXMatrix &texMtx, const DXMatrix &worldMtx, const DXMatrix &wvpMtx);

	// Updates the pixel shader's constant buffer and its resources
	void updatePixelShader(const Light &light, const DXVector3 &eyePosW, const float gCurrTime,
		const UINT diffuseMapBindPoint, const ShaderResourceViewPtr &pDiffuseMapRV,
		const UINT specMapBindPoint, const ShaderResourceViewPtr &pSpecMapRV,
		const UINT samplerStateBindPoint, const SamplerStatePtr &pSamplerState);

	// Define to set the constant buffer in the color vertex shader during the scene render
	void setColorVSConstBuf(const DXMatrix &wvpMtx);
#endif	// 0

	// resource creation and draw objects functions for each scene to be rendered

	// default scene resource creation
	void createResourcesDefaultScene(const bool bCCWFront = true, const bool bRefCrateStencil = true);

	// default scene draw objects
	void drawObjectsDefaultScene();

	// experiment 1 resource creation
	void createResourcesExpOne();

	// experiment 1 draw objects
	void drawObjectsExpOne();

	// experiment 4 resource creation
	void createResourcesExpFour();

	// experiment 4 draw objects
	void drawObjectsExpFour();

	// helper function to draw a full screen quad of the given colour
	void drawFullScreenQuad(const DXColor color);

	// @TODO: add protected methods here

protected:

	// scene objects
	TextureBoxPtr m_pCrate;
	RoomV1Ptr m_pRoom;

	// scene light
	Light m_parallelLight;

	// rasterizer, blend and depth-stencil states
	RasterizerStatePtr m_pCullCWRS;
	BlendStatePtr m_pReflectBS;
	DepthStencilStatePtr m_pDrawMirrorDSS;
	DepthStencilStatePtr m_pDrawReflectionDSS;
	
	// texture shader resource views
	ShaderResourceViewPtr m_pCrateRV, m_pWallRV, m_pFloorRV, m_pMirrorRV, m_pSpecRV;	

	// TexEffect instance
	TexEffectPtr m_pTexEffect;

	// ColorEffect ptr
	ColorEffectPtr m_pColorEffect;

	// input layout for the color shader
	InputLayoutPtr m_pColorVertexLayout;

	// simple quad to represent depth complexity
	SimpleQuadPtr m_pDepthQuad;
	
	// enum to specify the scene to be rendered
	enum Scenes
	{
		SCENE_DEFAULT,	// default scene
		SCENE_EXP1,		// wall-crate occlusion test
		SCENE_EXP2,		// front facing CCW disabled on reflected crate
		SCENE_EXP3,		// reflected crate stencil test disabled
		SCENE_EXP4,		// depth complexity computation
	};
	Scenes m_eCurrScene;

	// boolean flag used in experiment one to toggle the depth test for the wall
	bool m_bEnableWallDepth;
	
	// @TODO: add protected members here

};	// end of class MirrorApp

#endif	// MIRROR_APP_H