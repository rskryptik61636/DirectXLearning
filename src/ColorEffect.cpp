// Source file for the ColorEffect class

#include "ColorEffect.h"

// Param ctor
ColorEffect::ColorEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext)
: DXEffect(shaderRoot, pDevice, pDeviceContext)	{}

// Dtor
ColorEffect::~ColorEffect()	{}

// Causes the effect to be applied. (virtual, may be implemented by derived class)
void ColorEffect::apply()	{ DXEffect::apply(); }

// Initialization function which creates all the shaders and initializes
// all shader constant buffers, resources and sampler states.
void ColorEffect::init()	{ DXEffect::init(); }

// Mutator func to set the m_cbPerObject's WVP variable
void ColorEffect::setWVP(const DXMatrix &wvp)
{
	setConstantBufferVariableDataWithMatrix(wvp, "gWVP", m_cbPerObject);
}

// All the constant buffers across all shaders are set. (pure virtual, must be implemented by derived class)
void ColorEffect::setShaderConstantBuffers()
{
	// set the m_cbPerObject constant buffer in the vertex shader
	populateConstantBufferData(m_cbPerObject);
	const std::vector<ID3D11Buffer*> ppBuffers(1, m_cbPerObject.pBuffer.p);
	m_pDeviceContext->VSSetConstantBuffers(m_cbPerObject.bindDesc.BindPoint, ppBuffers.size(), ppBuffers.data());
}

// All the shaders are set. (pure virtual, must be implemented by derived class)
void ColorEffect::setShaders()
{
	// set the color vertex and pixel shaders
	m_pDeviceContext->VSSetShader(m_pColorVertexShader.p, NULL, 0);
	m_pDeviceContext->PSSetShader(m_pColorPixelShader.p, NULL, 0);
}

// Creates all the shaders required for the effect, basically calls to create*Shader. (pure virtual, must be implemented by derived class)
void ColorEffect::createShaders()
{
	// create the color vertex shader
	const wpath vertexShaderPath(constructShaderPath(m_shaderRoot, wpath(L"colorVS.cso")));
	createVertexShader(vertexShaderPath, m_pColorVertexByteCode, m_pColorVertexShader);

	// create the color pixel shader
	const wpath pixelShaderPath(constructShaderPath(m_shaderRoot, wpath(L"colorPS.cso")));
	createPixelShader(pixelShaderPath, m_pColorPixelByteCode, m_pColorPixelShader);
}

// Initializes the input layout for the effect. (pure virtual, must be implemented by derived class)
void ColorEffect::initInputLayout()
{
	// init the input layout from the color vertex shader's byte code
	createInputLayoutFromShaderInfo(m_pColorVertexByteCode, m_pInputLayout);
}

// Creates and initializes the constant buffers of all shaders. (pure virtual, must be implemented by derived class)
void ColorEffect::initShaderConstantBuffers()
{
	// init m_cbPerObject
	const std::string vsBufName("cbPerObject");
	createConstantBuffer(m_pColorVertexByteCode, vsBufName, m_cbPerObject);
}