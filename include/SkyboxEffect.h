// Header file for the SkyboxEffect class
#ifndef SKYBOX_EFFECT_H
#define SKYBOX_EFFECT_H

#include <DXEffect.h>
#include <DXShapes.h>

// Can be used to render a skybox.
class SkyboxEffect : public DXEffect
{
public:

	// Params for the skybox effect
	struct EffectParams
	{
		// Default ctor
		EffectParams();

		float scale;	// Skybox scale

		DXVector3 eyePosW;	// Camera eye position

		DXMatrix viewProj;	// Camera view-projection matrix

		ID3D11ShaderResourceView* pSkyboxTexture;	// Skybox texture
	};

	// Param ctor
	explicit SkyboxEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext, const float fSkyboxRadius);

	// Dtor
	/*virtual*/ ~SkyboxEffect();

	// Causes the effect to be applied. (virtual, may be implemented by derived class.) 
	// WARNING: Not implemented. Use the variant which takes effect params.
	/*virtual*/ void apply()	{ throw std::exception("Not implemented!"); }

	// Causes the effect to be applied with the given params.
	void apply(const EffectParams &params);

	// Initialization function which creates all the shaders and initializes
	// all shader constant buffers, resources and sampler states.
	/*virtual*/ void init();

	// Returns a string which contains effect info
	/*virtual*/ std::wstring getEffectInfo();

	// Accessor functions for the vertex shader's byte code buffer pointer and size.
	// WARNING: should only be called after the init() function has been invoked.
	/*virtual*/ LPCVOID getVSBufferPointer() const/* =0*/	{ return m_pVSBuffer; }
	/*virtual*/ const SIZE_T getVSBufferSize() const/* =0*/	{ return m_nVSBufferSize; }

	// Unbinds all the shaders, resources and samplers that were bound to the pipeline by this effect.
	/*virtual*/ void cleanup();

protected:

	// All the constant buffers across all shaders are set. (pure virtual, must be implemented by derived class)
	/*virtual*/ void setShaderConstantBuffers()/* =0*/;

	// All the shader textures(resources) are set. (pure virtual, must be implemented by derived class)
	/*virtual*/ void setShaderResources()/* =0*/;

	// All the shader samplers are set. (pure virtual, must be implemented by derived class)
	/*virtual*/ void setShaderSamplers()/* =0*/;

	// All the shaders are set. (pure virtual, must be implemented by derived class)
	/*virtual*/ void setShaders()/* =0*/;

	// Creates all the shaders required for the effect, basically calls to create*Shader. (pure virtual, must be implemented by derived class)
	/*virtual*/ void createShaders()/* =0*/;

	// Initializes the input layout for the effect. (pure virtual, must be implemented by derived class)
	/*virtual*/ void initInputLayout()/* =0*/;

	// Creates and initializes the constant buffers of all shaders. (pure virtual, must be implemented by derived class)
	/*virtual*/ void initShaderConstantBuffers()/* =0*/;

	// Initializes the resources of all shaders. (pure virtual, must be implemented by derived class)
	/*virtual*/ void initShaderResources()/* =0*/;

	// Initializes the sampler states of all shaders. (pure virtual, must be implemented by derived class)
	/*virtual*/ void initShaderSamplers()/* =0*/;

protected:

	// Skybox sphere
	ComplexSpherePtr m_pSkyboxSphere;
	float m_fSkyboxRadius;

	// Shaders and their associated blobs
	std::map<std::string, VertexShaderPtr> m_vertexShaders;
	std::map<std::string, PixelShaderPtr> m_pixelShaders;
	std::map<std::string, BlobPtr> m_shaderBlobs;

	// Constant buffers
	std::map<std::string, ShaderConstantBuffer> m_constantBuffers;

	// Resources and sampler states.
	std::map<std::string, ShaderResource> m_resources;
	std::map<std::string, ShaderSamplerState> m_samplers;

	// Rasterizer and depth-stencil states.
	std::map<std::string, RasterizerStatePtr> m_rasterStates;
	std::map<std::string, DepthStencilStatePtr> m_depthStencilStates;

	// Vertex shader buffer pointer and size.
	LPCVOID m_pVSBuffer;
	UINT m_nVSBufferSize;

};	// end of class SkyboxEffect
typedef std::unique_ptr<SkyboxEffect> SkyboxEffectPtr;

#endif	// SKYBOX_EFFECT_H