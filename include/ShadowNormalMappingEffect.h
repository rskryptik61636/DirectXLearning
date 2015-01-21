// Header file for the ShadowNormalMappingEffect class
#ifndef SHADOW_NORMAL_MAPPING_EFFECT_H
#define SHADOW_NORMAL_MAPPING_EFFECT_H

// Base DXEffect class
#include "DXEffect.h"

// Class which implements shadow as well as normal mapping
class ShadowNormalMappingEffect : public DXEffect
{
public:

	// Can be used to specify the current pass to be rendered
	enum EffectPass
	{
		EP_SHADOW_MAP_GENERATION,
		EP_SHADOW_MAP_RENDER
	};

	// Can be used to specify the shadow mapping technique to be used
	enum EffectTechnique
	{
		ET_SIMPLE,
		ET_PCF_1_TAP,
		ET_PCF_16_TAP
	};

	// Param ctor
	explicit ShadowNormalMappingEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext);

	// Dtor
	~ShadowNormalMappingEffect();

	// Causes the effect to be applied. (virtual, may be implemented by derived class)
	void apply();

	// Initialization function which creates all the shaders and initializes
	// all shader constant buffers, resources and sampler states.
	void init();

	// Returns a string which contains effect info
	std::wstring getEffectInfo();

	// Accessor functions for the vertex shader's byte code buffer pointer and size.
	// WARNING: should only be called after the init() function has been invoked.
	LPCVOID getVSBufferPointer()	const	{ return m_pFirstPassVertexByteCode.p ? m_pFirstPassVertexByteCode->GetBufferPointer() : NULL; }
	const SIZE_T getVSBufferSize()	const	{ return m_pFirstPassVertexByteCode.p ? m_pFirstPassVertexByteCode->GetBufferSize() : NULL; }

	// Mutator func to set the current pass
	inline void setCurrentPass(const EffectPass eCurrentPass)	{ m_eCurrentPass = eCurrentPass; }

	// Mutator func to set the current technique
	void setCurrentTechnique(const EffectTechnique eCurrentTechnique);	//{ m_eCurrentTechnique = eCurrentTechnique; }

	// mutator funcs to set the constant buffer members

	// cbPerObject first pass

	// set the first pass' light world view projection matrix (world matrix * light's view projection matrix)
	void setFirstPassLightWVP(const DXMatrix &lightWVP);

	// cbPerObject second pass

	// set the second pass' world view projection matrix
	void setSecondPassWVP(const DXMatrix &wvp);

	// set the second pass' world matrix
	void setSecondPassWorld(const DXMatrix &world);

	// set the second pass' inverse transpose of the world matrix
	void setSecondPassWorldInvTrans(const DXMatrix &worldInvTrans);

	// set the second pass' light space world view projection tangent matrix (world * light's view projection * tangent matrix)
	void setSecondPassLightWorldViewProjTangentMatrix(const std::vector<DXMatrix> &lightWorldViewProjTangent);

	// set the second pass' texture transformation matrix
	void setSecondPassTexMtx(const DXMatrix &texMtx);

	// cbPerFrame

	// set the spot lights
	void setSpotLights(const std::vector<Light> &spotLights);

	// set the world space eye pos
	void setEyePosW(const DXVector3 &eyePosW);

	// set the diffuse texture map
	void setDiffuseMap(const ShaderResourceViewPtr &pDiffuseMap);

	// set the specular light map
	void setSpecularMap(const ShaderResourceViewPtr &pSpecularMap);

	// set the normal map
	void setNormalMap(const ShaderResourceViewPtr &pNormalMap);

	// set the shadow map
	void setShadowMap(const ShaderResourceViewPtr &pShadowMap);

	// set the shadow map dims
	void setShadowMapSize(const UINT nWidth, const UINT nHeight);

	// unbind the shadow map SRV from the pixel shader
	void releaseShadowMap();

	// @TODO: keep the ones that we'll need
#if 0
	

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

	// First pass shadow map generation vertex shader
	VertexShaderPtr m_pFirstPassVertexShader;
	BlobPtr m_pFirstPassVertexByteCode;
	ShaderConstantBuffer m_cbPerObjectFirstPass;

	// First pass shadow map generation pixel shader
	PixelShaderPtr m_pFirstPassDummyPixelShader;
	BlobPtr m_pFirstPassDummyPixelByteCode;

	// Second pass shadow map generation vertex shader
	VertexShaderPtr m_pSecondPassVertexShader;
	BlobPtr m_pSecondPassVertexByteCode;
	ShaderConstantBuffer m_cbPerObjectSecondPass;

	// Second pass simple shadow map generation pixel shader
	PixelShaderPtr m_pSecondPassSimplePixelShader;
	BlobPtr m_pSecondPassSimplePixelByteCode;
	ShaderConstantBuffer m_cbPerFrameSecondPassSimple;
	ShaderResource m_resDiffuseMap, m_resSpecularMap, m_resNormalMap, m_resShadowMap;
	ShaderSamplerState m_samTriLinear, m_samShadow;
	UINT m_iSamplerStartBindPoint;
	std::vector<ID3D11SamplerState*> m_ppSamplers;

	// Second pass PCF based shadow map generation pixel shader
	PixelShaderPtr m_pSecondPassPCFPixelShader;
	BlobPtr m_pSecondPassPCFPixelByteCode;
	ShaderConstantBuffer m_cbPerFrameSecondPassPCF;
	ShaderResource m_resDiffuseMapPCF, m_resSpecularMapPCF, m_resNormalMapPCF, m_resShadowMapPCF;
	ShaderSamplerState m_samTriLinearPCF, m_samShadowPCF;
	UINT m_iSamplerStartBindPointPCF;
	std::vector<ID3D11SamplerState*> m_ppSamplersPCF;

	// Current pass being rendered
	EffectPass m_eCurrentPass;

	// Current filtering method
	EffectTechnique m_eCurrentTechnique;

	// TODO: really need to figure this out
	static const int nMaxSpotLights;
};
typedef std::unique_ptr<ShadowNormalMappingEffect> ShadowNormalMappingEffectPtr;

#endif	// SHADOW_NORMAL_MAPPING_EFFECT_H