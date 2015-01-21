// Test class to play around with loading models using Assimp - header file
#ifndef MODELS_TEST_APP_H
#define MODELS_TEST_APP_H

#include <DXApp.h>
#include <DXModel.h>
#include <TexEffect.h>
#include <NormalMappingEffect.h>
#include <DeferredShadingEffect.h>
#include <ComplexQuad.h>
#include <DebugTextureEffect.h>
#include <ShadowMap.h>
#include <ParticleSystemEffect.h>
#include <SkyboxEffect.h>

#include <AssimpInclude.h>

class ModelsTestApp : public DXApp
{
public:

	// Parameterized ctor, creates a DirectX app given the hInstance and the path to a scene description file
	// @param: hInstance = handle to the Windows app instance
	// @param: strSceneFilePath = absolute path to the scene description file (TODO: decide on the scene description file format)
	ModelsTestApp(HINSTANCE hInstance, const std::string strSceneFilePath);

	// Destructor
	~ModelsTestApp();

	// Initializes the DirectX application's objects and resources
	void initApp();

	// Called when the client window is resized
	void onResize();

	// Called when Windows events have to be processed and there is something that the application might have to take care off
	// @param: dt = delta time interval of the mGameTimer member
	void updateScene(float dt);

	// Renders the scene
	void drawScene();

	// not really useful at this point
#if 0
	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y);	// {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y); // {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y); // {}
#endif	 // 0

	// @TODO: add public methods here

public:
	// @TODO: add public members here

protected:

	// Define resources such as textures, blend/rasterizer/depth-stencil states and so on
	void createResources();

	// Define scene objects which need to be rendered
	void createObjects();

	// Meat of the drawScene method, this is where the scene rendering is implemented
	void drawObjects();

	// Define to build the shaders which will be used by the application
	void buildShaders();

	// Define to build the vertex layout which will be bound to the input assembly stage
	void buildVertexLayouts();

	// Recursive function which traverses the model's node tree and draws the meshes at each node using the TexEffect.
	void drawModelTex(const DXModelNode &currNode, const DXMatrix &viewProj, std::vector<DXMatrix> &transformationStack);

	// Renders the scene to the deferred shading effect's GBuffer
	void renderGBuffer();

	// Recursive function which traverses the model's node tree and renders the geometry of all the meshes at each node
	// into the deferred shading effect's GBuffer.
	void renderModelGBuffer(const DXModelNode &currNode, const BasicModelPtr &pModel, const DXMatrix &viewProj,
		std::vector<DXMatrix> &transformationStack, DeferredShadingEffect::EffectParams &eParams);

	// NOTE: These functions were taken from Andrew Lauritzen's 2010 Deferred Shading demo: http://software.intel.com/en-us/articles/deferred-rendering-for-current-and-future-rendering-pipelines/
	// Appending the license notice for reference.
	// @TODO: Move these to SceneBuilder as the pattern should rightly be set from parameters specified in the XML file.

	// Copyright 2010 Intel Corporation
	// All Rights Reserved
	//
	// Permission is granted to use, copy, distribute and prepare derivative works of this
	// software for any purpose and without fee, provided, that the above copyright notice
	// and this statement appear in all copies.  Intel makes no representations about the
	// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
	// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
	// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
	// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
	// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
	// assume any responsibility for any errors which may appear in this software nor any
	// responsibility to update it.

	// The original function is called InitializeLightParameters and was adapted for our purposes.
	void createPointLightsPattern(const UINT nMaxLights, const float fMaxRadius, const PointLightParams pointLightParams, std::vector<SLight> &pointLights);

	// Simple function for getting bright colors...
	// Hue in [0, 1)
	DXVector3 HueToRGB(const float hue);

	// The original function is called Move and was adapted for our purposes.
	void movePointLightsPattern(const float elapsedTime, const float fMaxRadius, const std::size_t nStart, const std::size_t nEnd, std::vector<SLight> &pointLights);

protected:

	// Spider model
	BasicModelPtr m_pSpiderModel;

	// Basic model directory
	BasicModelDirectory m_basicModels;

	// Full screen quad
	ComplexQuadPtr m_pFullscreenQuad;

	// TexEffect instance
	TexEffectPtr m_pTexEffect;

	// NormalMappingEffect instance and associated toggle state
	NormalMappingEffectPtr m_pNormalMappingEffect;
	ToggleStatePtr m_pNormalMappingEffectToggle;

	// DeferredShadingEffect instance
	DeferredShadingEffectPtr m_pDeferredShadingEffect;

	// DebugTextureEffect instance
	DebugTextureEffectPtr m_pDebugTextureEffect;

	// Particle system directory and the rain effect's toggle state
	ParticleSystemDirectory m_particleSystems;
	ToggleStatePtr m_pRainToggler;

	// Skybox effect instance
	SkyboxEffectPtr m_pSkybox;

	// Additive blend state for the deferred shading effect
	BlendStatePtr m_pAdditiveBS;

	// GBuffer instance
	GBufferPtr m_pGBuffer;

	// Specular map texture
	ShaderResourceViewPtr m_pSpecularMap;

	// Texture resources map.
	std::map<std::string, ShaderResource> m_textures;

	// Scene lights
	std::vector<SLight> m_parallelLights, m_pointLights, m_spotLights, m_movingPointLights, m_stationaryPointLights;
	float m_fPointLightsPatternRadius;

	// Previous frame's camera view projection matrix
	DXMatrix m_prevViewProj;

	// Scale at which the primary scene model should be drawn
	float m_fSceneScale;

	// Toggle state to switch between deferred shading output and the GBuffer textures
	ToggleStatePtr m_pGBufferOutput;

	// ShadowMap instance
	ShadowMapPtr m_pShadowMap;
	UINT m_nShadowMapWidth, m_nShadowMapHeight;
	std::vector<DXMatrix> m_spotLightsShadowTransforms;

	// Normal mapping flag (disabled by default) TODO: remove when done testing
	//bool m_bDoNormalMapping, m_bNormalMappingKeyPressed;

};	// end of class TestModelsApp

#endif	// MODELS_TEST_APP_H