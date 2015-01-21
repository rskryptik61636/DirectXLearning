// Header file for sandbox app for experimenting with different effects
#ifndef DX_SANDBOX_APP_H
#define DX_SANDBOX_APP_H

// header file for the DXApp base class
#include "DXApp.h"

// header file for the NormalMappingEffect class
#include "NormalMappingEffect.h"

// header file for the ColorEffect class
#include "ColorEffect.h"

// header file for the DebugTextureEffect class
#include "DebugTextureEffect.h"

// header file for the ShadowNormalMappingEffect class
#include "ShadowNormalMappingEffect.h"

// header file for the ParticleSystemEffect class
#include "ParticleSystemEffect.h"

// header file for the TangentBox class
#include "TangentBox.h"

// header file for the SimpleBox class
#include "SimpleBox.h"

// header file for the TangentQuad class
#include "TangentQuad.h"

// header file for the ShadowMap class
#include "ShadowMap.h"

// header file for the RenderStateMaker class
#include "RenderStateMaker.h"

// Sandbox app class
class DXSandboxApp : public DXApp
{
public:

	// Parameterized ctor, initializes the DXSandboxApp
	DXSandboxApp(HINSTANCE hInstance, const std::string strSceneFilePath);

	// Dtor
	~DXSandboxApp();

	// Renders the scene
	void drawScene();

	// Called when Windows events have to be processed and there is something that the application might have to take care off
	// @param: dt = delta time interval of the mGameTimer member
	void updateScene(float dt);

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

#if 0
	// Initializes the camera using the given params (TODO: deprecate when we implement the scene file functionality)
	void initCamera();
#endif	// 0

	// Initializes the scene lights
	void initLights();

	// Draws the boxes that indicate the positions of each of the scene lights
	void drawLightBoxes(const DXMatrix &viewProj);

	// Draws the shadow map to a debug window
	void drawShadowMap();
		
protected:

	// NormalMappingEffect instance
	NormalMappingEffectPtr m_pNormalMappingEffect;

	// ColorEffect instance
	ColorEffectPtr m_pColorEffect;

	// DebugTextureEffect instance
	DebugTextureEffectPtr m_pDebugTextureEffect;

	// ShadowNormalMappingEffect instance
	ShadowNormalMappingEffectPtr m_pShadowNormalMappingEffect;

	// @TODO: remove when done testing
	// ParticleSystemEffect instance
	ParticleSystemEffectPtr m_pParticleSystemEffect;
	UINT m_nMaxParticles;	// Max no. of particles in the particle system

	// Particle system directory which contains all the particle system instances
	ParticleSystemDirectory m_particleSystems;
	ToggleStatePtr m_pRainToggler;

	// brick texture and normal map
	ShaderResourceViewPtr m_pBrickTexture;
	ShaderResourceViewPtr m_pBrickNormalMap;

	// floor texture and normal map
	ShaderResourceViewPtr m_pFloorTexture;
	ShaderResourceViewPtr m_pFloorNormalMap;

	// specular light map
	ShaderResourceViewPtr m_pSpecularMap;

	// raindrop particle texture
	ShaderResource m_raindropTexture;

	// box with TangentVertex vertices which will serve as the room for the demo scenes
	TangentBoxPtr m_pRoom;

	// ordinary box with TangentVertex vertices
	TangentBoxPtr m_pBox1;

	// tangent quad used to display the shadow map
	TangentQuadPtr m_pShadowMapQuad;

	// shadow map helper class instance
	ShadowMapPtr m_pShadowMap;

	// scene lights
	std::vector<Light> m_parallelLights;
	std::vector<Light> m_pointLights;
	std::vector<Light> m_spotLights;

	// boxes to indicate the positions of the scene lights (parallel lights don't have a position)
	std::vector<SimpleBoxPtr> m_pointLightBoxes;
	std::vector<SimpleBoxPtr> m_spotLightBoxes;

	// render states
	RasterizerStatePtr m_pCounterClockwiseFrontFacing, m_pShadowMapRS;
	DepthStencilStatePtr m_pDepthTestingDSS;

};	// end of class DXSandboxApp

#endif	// DX_SANDBOX_APP_H