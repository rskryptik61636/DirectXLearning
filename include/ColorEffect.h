// header file for the ColorEffect class
#ifndef COLOR_EFFECT_H
#define COLOR_EFFECT_H

// include the DXEffect base class header
#include "DXEffect.h"

class ColorEffect : public DXEffect
{
public:

	// Param ctor
	explicit ColorEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext);

	// Dtor
	~ColorEffect();

	// Causes the effect to be applied. (virtual, may be implemented by derived class)
	void apply();

	// Initialization function which creates all the shaders and initializes
	// all shader constant buffers, resources and sampler states.
	void init();

	// Mutator func to set the m_cbPerObject's WVP variable
	void setWVP(const DXMatrix &wvp);

	// Accessor functions for the vertex shader's byte code buffer pointer and size.
	// WARNING: should only be called after the init() function has been invoked.
	LPCVOID getVSBufferPointer()	const	{ return m_pColorVertexByteCode.p ? m_pColorVertexByteCode->GetBufferPointer() : NULL; }
	const SIZE_T getVSBufferSize()	const	{ return m_pColorVertexByteCode.p ? m_pColorVertexByteCode->GetBufferSize() : NULL; }

protected:

	// All the constant buffers across all shaders are set. (pure virtual, must be implemented by derived class)
	/*virtual*/ void setShaderConstantBuffers() /*=0*/;

	// All the shader textures(resources) are set. (pure virtual, must be implemented by derived class)
	/*virtual*/ void setShaderResources() /*=0*/	{}

	// All the shader samplers are set. (pure virtual, must be implemented by derived class)
	/*virtual*/ void setShaderSamplers() /*=0*/	{}

	// All the shaders are set. (pure virtual, must be implemented by derived class)
	/*virtual*/ void setShaders() /*=0*/;

	// Creates all the shaders required for the effect, basically calls to create*Shader. (pure virtual, must be implemented by derived class)
	/*virtual*/ void createShaders() /*=0*/;

	// Initializes the input layout for the effect. (pure virtual, must be implemented by derived class)
	/*virtual*/ void initInputLayout() /*= 0*/;

	// Creates and initializes the constant buffers of all shaders. (pure virtual, must be implemented by derived class)
	/*virtual*/ void initShaderConstantBuffers() /*=0*/;

	// Initializes the resources of all shaders. (pure virtual, must be implemented by derived class)
	/*virtual*/ void initShaderResources() /*=0*/	{}

	// Initializes the sampler states of all shaders. (pure virtual, must be implemented by derived class)
	/*virtual*/ void initShaderSamplers() /*=0*/	{}

protected:

	// vertex shader vars
	VertexShaderPtr m_pColorVertexShader;
	BlobPtr m_pColorVertexByteCode;

	// vertex shader constant buffer
	ShaderConstantBuffer m_cbPerObject;

	// pixel shader vars
	PixelShaderPtr m_pColorPixelShader;
	BlobPtr m_pColorPixelByteCode;

};	// end of class ColorEffect
typedef std::unique_ptr<ColorEffect> ColorEffectPtr;

#endif	// COLOR_EFFECT_H