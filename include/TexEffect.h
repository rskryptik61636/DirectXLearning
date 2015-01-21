// header file for the TexEffect class

#ifndef TEX_EFFECT_H
#define TEX_EFFECT_H

#include "DXEffect.h"

#include "Light.h"

class TexEffect : public DXEffect
{
public:

	// Param ctor
	explicit TexEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext);

	// Dtor
	~TexEffect();

	// Causes the effect to be applied. (virtual, may be implemented by derived class)
	void apply();

	// Initialization function which creates all the shaders and initializes
	// all shader constant buffers, resources and sampler states.
	void init();

	// mutator funcs to set the constant buffer members
	void setWorld(const DXMatrix &world);
	void setWVP(const DXMatrix &wvp);
	void setTexMtx(const DXMatrix &texMtx);

	//void setLight(const Light &light);	// @TODO: remove when done testing

	// set the parallel lights
	void setParallelLights(const std::vector<Light> &parallelLights);	// no. of lights can be determined from the vector size

	// set the point lights
	void setPointLights(const std::vector<Light> &pointLights);

	// set the spot lights
	void setSpotLights(const std::vector<Light> &spotLights);

	void setEyePosW(const DXVector3 &eyePosW);
	void setCurrTime(const float fCurrTime);

	// mutator funcs to set the shader resources
	void setDiffuseMap(const ShaderResourceViewPtr &pDiffuseMap);
	void setSpecMap(const ShaderResourceViewPtr &pSpecMap);

	// Accessor functions for the vertex shader's byte code buffer pointer and size.
	// WARNING: should only be called after the init() function has been invoked.
	LPCVOID getVSBufferPointer()	const	{ return m_pTexVertexByteCode.p ? m_pTexVertexByteCode->GetBufferPointer() : NULL; }
	const SIZE_T getVSBufferSize()	const	{ return m_pTexVertexByteCode.p ? m_pTexVertexByteCode->GetBufferSize() : NULL; }

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

	// tex vertex shader
	VertexShaderPtr m_pTexVertexShader;
	BlobPtr m_pTexVertexByteCode;

	// vertex shader constant buffer
	ShaderConstantBuffer m_cbPerObject;

	// vertex shader constant buffer members
	DXMatrix m_world;
	DXMatrix m_wvp;
	DXMatrix m_texMtx;

	// tex pixel shader
	PixelShaderPtr m_pTexPixelShader;
	BlobPtr m_pTexPixelByteCode;

	// pixel shader constant buffer
	ShaderConstantBuffer m_cbPerFrame;

	// pixel shader constant buffer members
	Light m_light;
	DXVector3 m_eyePosW;
	float m_fCurrTime;

	// pixel shader resources
	ShaderResource m_resDiffuseMap, m_resSpecMap;

	// pixel shader sampler state
	ShaderSamplerState m_samTrilinear;

	// shader constant params
	static const int m_nMaxParallelLights;
	static const int m_nMaxPointLights;
	static const int m_nMaxSpotLights;

};	// end of class TexEffect
typedef std::unique_ptr<TexEffect> TexEffectPtr;

#endif	// TEX_EFFECT_H