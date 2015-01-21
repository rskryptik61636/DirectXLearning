// Source file for the DebugTextureEffect class

#include "DebugTextureEffect.h"

#include "SamplerStateMaker.h"

// Param ctor
DebugTextureEffect::DebugTextureEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext)
: DXEffect(shaderRoot, pDevice, pDeviceContext), m_fAdditive(0.0f), m_fMultiplier(1.0f)	{}

// Dtor
DebugTextureEffect::~DebugTextureEffect()	{}

// Causes the effect to be applied. (virtual, may be implemented by derived class)
void DebugTextureEffect::apply()	{ DXEffect::apply(); }

// Initialization function which creates all the shaders and initializes
// all shader constant buffers, resources and sampler states.
void DebugTextureEffect::init()	{ DXEffect::init(); }

// mutator funcs to set the constant buffer members

// cbPerObject

// set the world view projection matrix
void DebugTextureEffect::setWVP(const DXMatrix &wvp)
{
	setConstantBufferVariableDataWithMatrix(wvp, "gWVP", m_cbPerObject);
}

// set the texture transformation matrix
void DebugTextureEffect::setTexMtx(const DXMatrix &texMtx)
{
	setConstantBufferVariableDataWithMatrix(texMtx, "gTexMtx", m_cbPerObject);
}

// cbPerFrame

// Mutator func to set the texture channel to be debugged
void DebugTextureEffect::setTextureIndex(const DebugChannel eTextureIndex)
{
	const int nTextureIndex(eTextureIndex);
	const BYTE *pTextureIndex = reinterpret_cast<const BYTE*>(&nTextureIndex);
	const std::size_t nBytes = sizeof(nTextureIndex);
	setConstantBufferVariableData(pTextureIndex, nBytes, "nIndex", m_cbPerFrame);
}

// Mutator func to set the texture to be debugged
void DebugTextureEffect::setDebugTexture(const ShaderResourceViewPtr &pDebugTexture)
{
	m_debugTexture.pResourceView = pDebugTexture;
}

// Mutator func to set the additive term
void DebugTextureEffect::setAdditiveTerm(const float fAdditive)	{ m_fAdditive = fAdditive; }

// Mutator func to set the multiplier term
void DebugTextureEffect::setMultiplierTerm(const float fMuliplier)	{ m_fMultiplier = fMuliplier; }

// All the constant buffers across all shaders are set. (pure virtual, must be implemented by derived class)
void DebugTextureEffect::setShaderConstantBuffers()
{
	// populate and set the cbPerObject constant buffer in the vertex shader
	populateConstantBufferData(m_cbPerObject);
	const std::vector<ID3D11Buffer*> ppVSBuffers(1, m_cbPerObject.pBuffer.p);
	m_pDeviceContext->VSSetConstantBuffers(m_cbPerObject.bindDesc.BindPoint, ppVSBuffers.size(), ppVSBuffers.data());

	// populate and set the cbPerFrame constant buffer in the pixel shader
	setConstantBufferVariableTypedDatum<float>(m_fAdditive, "additive", m_cbPerFrame);
	setConstantBufferVariableTypedDatum<float>(m_fMultiplier, "multiplier", m_cbPerFrame);
	populateConstantBufferData(m_cbPerFrame);
	const std::vector<ID3D11Buffer*> ppPSBuffers(1, m_cbPerFrame.pBuffer.p);
	m_pDeviceContext->PSSetConstantBuffers(m_cbPerFrame.bindDesc.BindPoint, ppPSBuffers.size(), ppPSBuffers.data());
}

// All the shader textures(resources) are set. (pure virtual, must be implemented by derived class)
void DebugTextureEffect::setShaderResources()
{
	// set the debug texture in the pixel shader
	const std::vector<ID3D11ShaderResourceView*> ppPSResources(1, m_debugTexture.pResourceView.p);
	m_pDeviceContext->PSSetShaderResources(m_debugTexture.bindDesc.BindPoint, ppPSResources.size(), ppPSResources.data());
}

// Mutator func to release the texture being debugged from the pixel shader resources
// Added in the event that the depth buffer texture is bound to the pixel shader resources
// which would prevent it from being reused in the next frame if it is not unbound
void DebugTextureEffect::releaseDebugTexture()
{
	ID3D11ShaderResourceView* ppEmpty = { NULL };
	m_pDeviceContext->PSSetShaderResources(m_debugTexture.bindDesc.BindPoint, 1, &ppEmpty);
}

// All the shader samplers are set. (pure virtual, must be implemented by derived class)
void DebugTextureEffect::setShaderSamplers()
{
	// set the sampler state in the pixel shader
	const std::vector<ID3D11SamplerState*> ppPSSamplers(1, m_sampler.pSamplerState.p);
	m_pDeviceContext->PSSetSamplers(m_sampler.bindDesc.BindPoint, ppPSSamplers.size(), ppPSSamplers.data());
}

// All the shaders are set. (pure virtual, must be implemented by derived class)
void DebugTextureEffect::setShaders()
{
	// set the debug texture vertex shader
	m_pDeviceContext->VSSetShader(m_pVertexShader.p, NULL, 0);

	// set the debug texture pixel shader
	m_pDeviceContext->PSSetShader(m_pPixelShader.p, NULL, 0);
}

// Unbinds all the shaders, resources and samplers that were bound to the pipeline by this effect.
void DebugTextureEffect::cleanup()
{
	// unbind the debug texture vertex and pixel shader
	m_pDeviceContext->VSSetShader(NULL, NULL, 0);
	m_pDeviceContext->PSSetShader(NULL, NULL, 0);

	// Unbind the constant buffers in the vertex and pixel shaders.
	const std::vector<ID3D11Buffer*> ppVSBuffers(1, NULL);
	m_pDeviceContext->VSSetConstantBuffers(m_cbPerObject.bindDesc.BindPoint, ppVSBuffers.size(), ppVSBuffers.data());

	const std::vector<ID3D11Buffer*> ppPSBuffers(1, NULL);
	m_pDeviceContext->PSSetConstantBuffers(m_cbPerFrame.bindDesc.BindPoint, ppPSBuffers.size(), ppPSBuffers.data());

	// Unbind the pixel shader resources.
	const std::vector<ID3D11ShaderResourceView*> ppPSResources(1, NULL);
	m_pDeviceContext->PSSetShaderResources(m_debugTexture.bindDesc.BindPoint, ppPSResources.size(), ppPSResources.data());

	// Unbind the sampler state in the pixel shader.
	const std::vector<ID3D11SamplerState*> ppPSSamplers(1, NULL);
	m_pDeviceContext->PSSetSamplers(m_sampler.bindDesc.BindPoint, ppPSSamplers.size(), ppPSSamplers.data());
}

// Creates all the shaders required for the effect, basically calls to create*Shader. (pure virtual, must be implemented by derived class)
void DebugTextureEffect::createShaders()
{
	// Create the debug texture vertex shader
	const wpath vertexShaderPath(constructShaderPath(m_shaderRoot, wpath(L"debugTextureVS.cso")));
	createVertexShader(vertexShaderPath, m_pVertexByteCode, m_pVertexShader);

	// Create the debug texture pixel shader
	const wpath pixelShaderPath(constructShaderPath(m_shaderRoot, wpath(L"debugTexturePS1.cso")));
	createPixelShader(pixelShaderPath, m_pPixelByteCode, m_pPixelShader);
}

// Initializes the input layout for the effect. (pure virtual, must be implemented by derived class)
void DebugTextureEffect::initInputLayout()
{
	// Determine the input layout from the vertex shader
	createInputLayoutFromShaderInfo(m_pVertexByteCode, m_pInputLayout);
}

// Creates and initializes the constant buffers of all shaders. (pure virtual, must be implemented by derived class)
void DebugTextureEffect::initShaderConstantBuffers()
{
	// Initialize the cbPerObject constant buffer
	std::string bufName("cbPerObject");
	//m_cbPerObject.varsInfo.resize(2);
	//m_cbPerObject.varsInfo[0].strVarName = "gWVP";
	//m_cbPerObject.varsInfo[1].strVarName = "gTexMtx";
	createConstantBuffer(m_pVertexByteCode, bufName, m_cbPerObject);

	// Initialize the cbPerFrame constant buffer
	bufName = "cbPerFrame";
	/*m_cbPerFrame.varsInfo.resize(1);
	m_cbPerFrame.varsInfo[0].strVarName = "nIndex";*/
	createConstantBuffer(m_pPixelByteCode, bufName, m_cbPerFrame);
}

// Initializes the resources of all shaders. (pure virtual, must be implemented by derived class)
void DebugTextureEffect::initShaderResources()
{
	// get the binding info of all the pixel shader textures
	ShaderReflectionPtr pReflector;
	HR(D3DReflect(m_pPixelByteCode->GetBufferPointer(), m_pPixelByteCode->GetBufferSize(),
		IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&pReflector.p)));

	m_debugTexture.strName = "gDebugTexture";
	HR(pReflector->GetResourceBindingDescByName(m_debugTexture.strName.c_str(), &m_debugTexture.bindDesc));

	// close the reflection interface
	pReflector.Release();
}

// Initializes the sampler states of all shaders. (pure virtual, must be implemented by derived class)
void DebugTextureEffect::initShaderSamplers()
{
	// Create a tri linear sampler state
	SamplerStateMaker samplerStateMaker;
	samplerStateMaker.makeTriPoint(m_pDevice, m_sampler.pSamplerState);
	//samplerStateMaker.makeTriLinear(m_pDevice, m_sampler.pSamplerState);	// @TODO: remove when done testing
}