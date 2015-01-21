// Header file for the DeferredShadingEffect class
#ifndef DEFERRED_SHADING_EFFECT_H
#define DEFERRED_SHADING_EFFECT_H

// Base DXEffect class
#include "DXEffect.h"

// GBuffer class
#include "GBuffer.h"

// Light data structure
#include "Light.h"

// DebugTextureEffect class
#include "DebugTextureEffect.h"

// Class which implements deferred shading
class DeferredShadingEffect : public DXEffect
{
public:

	// Can be used to specify the deferred shading technique to be used
	enum EffectTechnique
	{
		ET_CLASSIC,
		ET_TILED
	};

	// Can be used to specify the current pass to be rendered
	enum EffectPass
	{
		EP_G_BUFFER_GENERATION,
		EP_DEFERRED_SHADING
	};

	// Effect params struct
	struct EffectParams
	{
		// Default ctor.
		EffectParams();

		// Effect technique
		EffectTechnique eTech;

		// Effect pass
		EffectPass ePass;

		// World space transform
		DXMatrix *world;

		// Inverse tranpose of world space transform
		DXMatrix *worldInvTrans;

		// World-view-projection transfrom
		DXMatrix *wvp;

		// Texture space transform
		DXMatrix *texMtx;

		// Current frame's view projection matrix
		DXMatrix *currViewProj;

		// Previous frame's view projection matrix
		DXMatrix *prevViewProj;

		// Diffuse colour map
		ShaderResourceViewPtr pDiffuseMap;

		// Specular colour map
		ShaderResourceViewPtr pSpecularMap;

		// Deferred shading position map
		ShaderResourceViewPtr pPositionMap;

		// Deferred shading normal map
		ShaderResourceViewPtr pNormalMap;

		// World space eye position
		DXVector3 eyePosW;

		// Parallel lights
		std::vector<SLight> *parallelLights;

		// Point lights
		std::vector<SLight> *pointLights;

		// Spot lights
		std::vector<SLight> *spotLights;

		// Motion blur no. of samples
		int nMotionBlurSamples;

		// Shadow map dims (width, height)
		DXVector2 shadowMapDims;

		// Deferred shading shadow map
		ShaderResourceViewPtr pShadowMap;

		// Shadow mapping transformation matrices for the spot lights
		std::vector<DXMatrix> *spotLightsShadowTransforms;		

	};	// end of struct ShaderParams

	// Param ctor
	// @param shaderRoot: Absolute path of the shaders root directory.
	// @param pDevice: D3D device object.
	// @param pDeviceContext: D3D immediate device context.
	explicit DeferredShadingEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext);

	// Param ctor
	// @param shaderRoot: Absolute path of the shaders root directory.
	// @param pDevice: D3D device object.
	// @param pDeviceContext: D3D immediate device context.
	// @param nClientWidth: Client window width.
	// @param nClientHeight: Client window height.
	explicit DeferredShadingEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext, 
									const UINT nClientWidth, const UINT nClientHeight);

	// Dtor
	~DeferredShadingEffect();

	// Causes the effect to be applied. (virtual, may be implemented by derived class)
	// WARNING: Do not use. Use the variant which takes in an EffectParams instance.
	void apply()	{}

	// Causes the effect to be applied with the given params.
	void apply(const EffectParams &params);

	// Initialization function which creates all the shaders and initializes
	// all shader constant buffers, resources and sampler states.
	void init();

	// Variant of the init function which also specifies the no. of parallel, point and spot lights in the scene.
	void init(const UINT nParallelLights, const UINT nPointLights, const UINT nSpotLights);

	// Returns a string which contains effect info
	std::wstring getEffectInfo();

	// Accessor functions for the vertex shader's byte code buffer pointer and size.
	// WARNING: should only be called after the init() function has been invoked.
	LPCVOID getVSBufferPointer()	const	{ return m_pVSByteCode.p ? m_pVSByteCode->GetBufferPointer() : NULL; }
	const SIZE_T getVSBufferSize()	const	{ return m_pVSByteCode.p ? m_pVSByteCode->GetBufferSize() : NULL; }

	// Unbinds all the shaders, resources and samplers that were bound to the pipeline by this effect.
	void cleanup();	

	// Toggles the states of the rendering techniques based on keyboard input
	void toggleStates();

	// Displays the current state of the deferred shading effects
	void dispEffectStates(const SpriteBatchPtr &pSpriteBatch, const SpriteFontPtr &pSpriteFont, const UINT uiClientWidth, const UINT uiClientHeight);

	// Accessor method specifying whether the shadow mapping effect is set
	inline bool doingShadowMapping() const	{ return m_eShadingEffect == SE_SHADOW_MAPPING; }

	// Mutator method to resize the tiled output buffer.
	void resizeTiledOutput(const UINT nClientWidth, const UINT nClientHeight);

	// TODO: add shader param mutator funcs here

protected:

	// All the constant buffers across all shaders are set. (pure virtual, must be implemented by derived class)
	void setShaderConstantBuffers() 	{}

	// All the shader textures(resources) are set. (pure virtual, must be implemented by derived class)
	void setShaderResources() 	{}

	// All the shader samplers are set. (pure virtual, must be implemented by derived class)
	void setShaderSamplers() 	{}

	// All the shaders are set. (pure virtual, must be implemented by derived class)
	void setShaders() 	{}

	// Creates all the shaders required for the effect, basically calls to create*Shader. (pure virtual, must be implemented by derived class)
	void createShaders(); 

	// Initializes the input layout for the effect. (pure virtual, must be implemented by derived class)
	void initInputLayout();

	// Creates and initializes the constant buffers of all shaders. (pure virtual, must be implemented by derived class)
	void initShaderConstantBuffers() ;

	// Initializes the resources of all shaders. (pure virtual, must be implemented by derived class)
	void initShaderResources() ;

	// Initializes the sampler states of all shaders. (pure virtual, must be implemented by derived class)
	void initShaderSamplers() ;

protected:

	// Can be used to specify the current type of GBuffer effect that affects how the GBuffer textures are being created.
	enum GBufferEffect
	{
		GE_STANDARD,
		GE_NORMAL_MAPPING,
		GE_MAX_EFFECTS	// should always be the last one
	};
	GBufferEffect m_eGBufferEffect;

	// Can be used to specify the current type of final shading effect that takes place.
	enum ShadingEffect
	{
		SE_STANDARD,
		SE_MOTION_BLUR,
		SE_SHADOW_MAPPING,
		SE_MAX_EFFECTS	// should always be the last one
	};
	ShadingEffect m_eShadingEffect;

	// Vertex shader, byte code and params
	VertexShaderPtr m_pVS;
	BlobPtr m_pVSByteCode;
	ShaderConstantBuffer m_VSCBPerObject;

	// GBuffer generation pixel shader, byte code and params
	PixelShaderPtr m_pGBufferPS;
	BlobPtr m_pGBufferPSByteCode;
	//ShaderResource m_resGBufferPSDiffuseMap, m_resGBufferPSSpecularMap;
	ShaderSamplerState m_samGBufferPS;

	// GBuffer generation normal mapping pixel shader, byte code and params
	PixelShaderPtr m_pGBufferPS2;
	BlobPtr m_pGBufferPS2ByteCode;
	ShaderSamplerState m_samGBufferPS2;
	ToggleStatePtr m_pNormalMappingToggler;

	// Deferred texture mapping pixel shader, byte code and params
	PixelShaderPtr m_pTexPS;
	BlobPtr m_pTexPSByteCode;
	ToggleStatePtr m_pTextureMappingToggler;
	ShaderSamplerState m_samTexPS;
	ShaderConstantBuffer m_cbPerFrameTexPS;

	// Common shader params that can be reused
	ShaderStructuredBuffer<SLight> m_sbParallelLights, m_sbPointLights, m_sbSpotLights;
	UINT m_nParallelLights, m_nPointLights, m_nSpotLights;	
	ShaderSamplerState m_samStandardShading;

	// Deferred motion blur pixel shader, byte code and params
	PixelShaderPtr m_pMotionBlurPS;
	BlobPtr m_pMotionBlurPSByteCode;
	ShaderSamplerState m_samMotionBlurPS;
	ShaderConstantBuffer m_cbPerFrameMotionBlurPS;
	ToggleStatePtr m_pMotionBlurToggler;

	// Deferred shadow mapping pixel shader, byte code and params
	PixelShaderPtr m_pSimpleShadowMappingPS, m_pPCFShadowMappingPS;
	BlobPtr m_pSimpleShadowMappingPSByteCode, m_pPCFShadowMappingPSByteCode;

	// Common shadow mapping params
	ShaderSamplerState m_samShadowMappingPS;
	ShaderConstantBuffer m_cbPerFrameShadowMappingPS;
	//ToggleStatePtr m_pShadowMappingToggler;
	ShaderStructuredBuffer<DXMatrix> m_sbSpotLightsShadowTransforms;

	// Output buffer for tiled deferred shading and its dims.
	CTexture2DPtr m_pTiledOutput;
	UINT m_nTileSize, m_nTileDims;	// Tile size = width/height. Tile dims = Tile size * Tile size.
									// NOTE: Must reflect the same value as defined in the the tiled deferred shader.
	BOOL m_bShowLightsPerTile;		// Specifies whether the lights per tile should be displayed in the tiled buffer output or not.
									// NOTE: Taking advantage of the fact that BOOL is actually a typedef UINT with FALSE = 0 and TRUE = 1.

	// Current effect technique.
	EffectTechnique m_eCurrTech;

	// DebugTextureEffect instance.
	DebugTextureEffectPtr m_pDebugTextureEffect;

	// Shaders and their associated params.
	// @TODO: Update all the shaders, buffers, resources, toggle states to use this.
	std::map<std::string, VertexShaderPtr> m_vertexShaders;
	std::map<std::string, PixelShaderPtr> m_pixelShaders;
	std::map<std::string, ComputeShaderPtr> m_computeShaders;
	std::map<std::string, ShaderConstantBuffer> m_constantBuffers;
	std::map<std::string, BlobPtr> m_shaderBlobs;
	std::map<std::string, ShaderSamplerState> m_samplerStates;
	std::map<std::string, ToggleStatePtr> m_toggleStates;

	// PCF shadow mapping tap size.
	UINT m_nTapSize, m_nMaxTapSize;
};
typedef std::unique_ptr<DeferredShadingEffect> DeferredShadingEffectPtr;

#endif	// DEFERRED_SHADING_EFFECT_H