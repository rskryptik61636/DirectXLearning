// header file for the NormalMappingEffect class
#ifndef NORMAL_MAPPING_EFFECT_H
#define NORMAL_MAPPING_EFFECT_H

// include the base DXEffect class header
#include "DXEffect.h"

// include the Light class
#include "Light.h"

// Class which implements the normal mapping effect
class NormalMappingEffect : public DXEffect
{
public:

	// Param ctor
	explicit NormalMappingEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext);

	// Dtor
	~NormalMappingEffect();

	// Causes the effect to be applied. (virtual, may be implemented by derived class)
	void apply();

	// Initialization function which creates all the shaders and initializes
	// all shader constant buffers, resources and sampler states.
	void init();

	// Accessor functions for the vertex shader's byte code buffer pointer and size.
	// WARNING: should only be called after the init() function has been invoked.
	LPCVOID getVSBufferPointer()	const	{ return m_pNormalMappingVertexByteCode.p ? m_pNormalMappingVertexByteCode->GetBufferPointer() : NULL; }
	const SIZE_T getVSBufferSize()	const	{ return m_pNormalMappingVertexByteCode.p ? m_pNormalMappingVertexByteCode->GetBufferSize() : NULL; }

	// mutator funcs to set the constant buffer members

	// cbPerObject

	// set the world matrix
	void setWorld(const DXMatrix &world);
	
	// set the inverse transpose of the world matrix
	void setWorldInvTrans(const DXMatrix &worldInvTrans);
	
	// set the world view projection matrix
	void setWVP(const DXMatrix &wvp);
	
	// set the texture transformation matrix
	void setTexMtx(const DXMatrix &texMtx);

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

	// vertex shader variables
	VertexShaderPtr m_pNormalMappingVertexShader;
	BlobPtr m_pNormalMappingVertexByteCode;
	ShaderConstantBuffer m_cbPerObject;

	// pixel shader variables
	PixelShaderPtr m_pNormalMappingPixelShader;
	BlobPtr m_pNormalMappingPixelByteCode;
	ShaderConstantBuffer m_cbPerFrame;
	ShaderResource m_resDiffuseMap, m_resNormalMap, m_resSpecMap;
	ShaderSamplerState m_samTriLinear;
	int m_nParallelLights, m_nPointLights, m_nSpotLights;

	// shader constant params
	static const int m_nMaxParallelLights;
	static const int m_nMaxPointLights;
	static const int m_nMaxSpotLights;

};	// end of class NormalMappingEffect
typedef std::unique_ptr<NormalMappingEffect> NormalMappingEffectPtr;

#endif	// NORMAL_MAPPING_EFFECT_H