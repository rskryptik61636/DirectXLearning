// Header file for the DebugTextureEffect class
#ifndef DEBUG_TEXTURE_EFFECT
#define DEBUG_TEXTURE_EFFECT

// Base DXEffect class
#include "DXEffect.h"

// Class which implements shadow as well as normal mapping
class DebugTextureEffect : public DXEffect
{
public:

	// Can be used to specify the texture channel to be rendered
	enum DebugChannel
	{
		DC_RED = 0,
		DC_GREEN = 1,
		DC_BLUE = 2,
		DC_ALL = 3
	};

	// Param ctor
	explicit DebugTextureEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext);

	// Dtor
	~DebugTextureEffect();

	// Causes the effect to be applied. (virtual, may be implemented by derived class)
	void apply();

	// Initialization function which creates all the shaders and initializes
	// all shader constant buffers, resources and sampler states.
	void init();

	// mutator funcs to set the constant buffer members

	// cbPerObject

	// set the world view projection matrix
	void setWVP(const DXMatrix &wvp);

	// set the texture transformation matrix
	void setTexMtx(const DXMatrix &texMtx);

	// cbPerFrame

	// Mutator func to set the texture channel to be debugged
	void setTextureIndex(const DebugChannel eTextureIndex);

	// Mutator func to set the texture to be debugged
	void setDebugTexture(const ShaderResourceViewPtr &pDebugTexture);

	// Mutator func to set the additive term
	void setAdditiveTerm(const float fAdditive);

	// Mutator func to set the multiplier term
	void setMultiplierTerm(const float fMuliplier);

	// Mutator func to release the texture being debugged from the pixel shader resources
	// Added in the event that the depth buffer texture is bound to the pixel shader resources
	// which would prevent it from being reused in the next frame if it is not unbound
	void releaseDebugTexture();

	// Accessor functions for the vertex shader's byte code buffer pointer and size.
	// WARNING: should only be called after the init() function has been invoked.
	LPCVOID getVSBufferPointer()	const	{ return m_pVertexByteCode.p ? m_pVertexByteCode->GetBufferPointer() : NULL; }
	const SIZE_T getVSBufferSize()	const	{ return m_pVertexByteCode.p ? m_pVertexByteCode->GetBufferSize() : NULL; }

	// Unbinds all the shaders, resources and samplers that were bound to the pipeline by this effect.
	void cleanup();

	// @TODO: keep the ones that we'll need
#if 0
	// set the world matrix
	void setWorld(const DXMatrix &world);

	// set the inverse transpose of the world matrix
	void setWorldInvTrans(const DXMatrix &worldInvTrans);

	// cbPerFrame

	// set the parallel lights
	void setParallelLights(const std::vector<Light> &parallelLights);	// no. of lights can be determined from the vector size

	// set the point lights
	void setPointLights(const std::vector<Light> &pointLights);

	// set the spot lights
	void setSpotLights(const std::vector<Light> &spotLights);

	// set the world space eye pos
	void setEyePosW(const DXVector3 &eyePosW);

	// set the diffuse texture map
	void setDiffuseMap(const ShaderResourceViewPtr &pDiffuseMap);

	// set the normal map
	void setNormalMap(const ShaderResourceViewPtr &pNormalMap);

	// set the specular light map
	void setSpecularMap(const ShaderResourceViewPtr &pSpecularMap);
#endif	// 0

	// TODO: add shader param mutator funcs here

protected:

	// All the constant buffers across all shaders are set. (pure virtual, must be implemented by derived class)
	/*virtual*/ void setShaderConstantBuffers() /*=0*/;

	// All the shader textures(resources) are set. (pure virtual, must be implemented by derived class)
	/*virtual*/ void setShaderResources() /*=0*/;

	// All the shader samplers are set. (pure virtual, must be implemented by derived class)
	/*virtual*/ void setShaderSamplers() /*=0*/;

	// All the shaders are set. (pure virtual, must be implemented by derived class)
	/*virtual*/ void setShaders() /*=0*/;

	// Creates all the shaders required for the effect, basically calls to create*Shader. (pure virtual, must be implemented by derived class)
	/*virtual*/ void createShaders() /*=0*/;

	// Initializes the input layout for the effect. (pure virtual, must be implemented by derived class)
	/*virtual*/ void initInputLayout() /*= 0*/;

	// Creates and initializes the constant buffers of all shaders. (pure virtual, must be implemented by derived class)
	/*virtual*/ void initShaderConstantBuffers() /*=0*/;

	// Initializes the resources of all shaders. (pure virtual, must be implemented by derived class)
	/*virtual*/ void initShaderResources() /*=0*/;

	// Initializes the sampler states of all shaders. (pure virtual, must be implemented by derived class)
	/*virtual*/ void initShaderSamplers() /*=0*/;

protected:

	// Vertex shader variables
	VertexShaderPtr m_pVertexShader;
	BlobPtr m_pVertexByteCode;
	ShaderConstantBuffer m_cbPerObject;

	// Pixel shader variables
	PixelShaderPtr m_pPixelShader;
	BlobPtr m_pPixelByteCode;
	ShaderConstantBuffer m_cbPerFrame;
	ShaderResource m_debugTexture;
	ShaderSamplerState m_sampler;

	// Additive and multiplier terms for the sampled texture.
	float m_fAdditive, m_fMultiplier;

};
typedef std::unique_ptr<DebugTextureEffect> DebugTextureEffectPtr;

#endif	// DEBUG_TEXTURE_EFFECT