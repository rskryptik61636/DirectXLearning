// source file of the TexEffect class

#include "TexEffect.h"

#include "SamplerStateMaker.h"

// shader constant params
const int TexEffect::m_nMaxParallelLights = 500;
const int TexEffect::m_nMaxPointLights = 500;
const int TexEffect::m_nMaxSpotLights = 500;

// Param ctor
TexEffect::TexEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext)
: DXEffect(shaderRoot, pDevice, pDeviceContext)
{}

// Dtor
TexEffect::~TexEffect()	{}

// Causes the effect to be applied. (virtual, may be implemented by derived class)
void TexEffect::apply() 
{
	DXEffect::apply();	// invoke the base class version, nothing else is necessary
}

// Initialization function which creates all the shaders and initializes
// all shader constant buffers, resources and sampler states.
void TexEffect::init()	{ DXEffect::init(); }

// mutator funcs to set the constant buffer members
void TexEffect::setWorld(const DXMatrix &world)	
{ 
	setConstantBufferVariableDataWithMatrix(world, "gWorld", m_cbPerObject);
}
void TexEffect::setWVP(const DXMatrix &wvp)
{
	setConstantBufferVariableDataWithMatrix(wvp, "gWVP", m_cbPerObject);
}
void TexEffect::setTexMtx(const DXMatrix &texMtx)
{
	setConstantBufferVariableDataWithMatrix(texMtx, "gTexMtx", m_cbPerObject);
}

// @TODO: remove when done testing
//void TexEffect::setLight(const Light &light)
//{
//	setConstantBufferVariableData(reinterpret_cast<const BYTE*>(&light), sizeof(light), "gLight", m_cbPerFrame);
//}

// set the parallel lights
void TexEffect::setParallelLights(const std::vector<Light> &parallelLights)	// no. of lights can be determined from the vector size
{
	setLights(parallelLights, m_nMaxParallelLights, "gParallelLights", "nParallelLights", m_cbPerFrame);
}

// set the point lights
void TexEffect::setPointLights(const std::vector<Light> &pointLights)
{
	setLights(pointLights, m_nMaxPointLights, "gPointLights", "nPointLights", m_cbPerFrame);
}

// set the spot lights
void TexEffect::setSpotLights(const std::vector<Light> &spotLights)
{
	setLights(spotLights, m_nMaxSpotLights, "gSpotLights", "nSpotLights", m_cbPerFrame);
}

void TexEffect::setEyePosW(const DXVector3 &eyePosW)	
{
	setConstantBufferVariableData(reinterpret_cast<const BYTE*>(&eyePosW), sizeof(eyePosW), "gEyePosW", m_cbPerFrame);
}
void TexEffect::setCurrTime(const float fCurrTime)	
{ 
	setConstantBufferVariableData(reinterpret_cast<const BYTE*>(&fCurrTime), sizeof(fCurrTime), "gCurrTime", m_cbPerFrame);
}

// mutator funcs to set the shader resources
void TexEffect::setDiffuseMap(const ShaderResourceViewPtr &pDiffuseMap)	{ m_resDiffuseMap.pResourceView = pDiffuseMap; }
void TexEffect::setSpecMap(const ShaderResourceViewPtr &pSpecMap)	{ m_resSpecMap.pResourceView = pSpecMap; }

// All the constant buffers across all shaders are set. (pure virtual, must be implemented by derived class)
void TexEffect::setShaderConstantBuffers() 
{
	// populate the m_cbPerObject constant buffer's data buffer
	populateConstantBufferData(m_cbPerObject);
	
	const std::vector<ID3D11Buffer*> ppVSBuffers(1, m_cbPerObject.pBuffer.p);
	m_pDeviceContext->VSSetConstantBuffers(m_cbPerObject.bindDesc.BindPoint, ppVSBuffers.size(), ppVSBuffers.data());

	// populate the m_cbPerFrame constant buffer and set it in the pixel shader
	populateConstantBufferData(m_cbPerFrame);
	const std::vector<ID3D11Buffer*> ppPSBuffers(1, m_cbPerFrame.pBuffer.p);
	m_pDeviceContext->PSSetConstantBuffers(m_cbPerFrame.bindDesc.BindPoint, ppPSBuffers.size(), ppPSBuffers.data());

	// @TODO: add implementation here
}

// All the shader textures(resources) are set. (pure virtual, must be implemented by derived class)
void TexEffect::setShaderResources() 
{
	// set the diffuse map and spec map shader resources
	// TODO: look into sorting the vector automatically based on the bind point
	std::vector<ID3D11ShaderResourceView*> ppResources(2);
	ppResources[0] = m_resDiffuseMap.pResourceView.p;
	ppResources[1] = m_resSpecMap.pResourceView.p;
	m_pDeviceContext->PSSetShaderResources(m_resDiffuseMap.bindDesc.BindPoint, ppResources.size(), ppResources.data());
}

// All the shader samplers are set. (pure virtual, must be implemented by derived class)
void TexEffect::setShaderSamplers() 
{
	// set the trilinear sampler state
	const std::vector<ID3D11SamplerState*> ppSamplers(1, m_samTrilinear.pSamplerState.p);
	m_pDeviceContext->PSSetSamplers(m_samTrilinear.bindDesc.BindPoint, ppSamplers.size(), ppSamplers.data());
}

// All the shaders are set. (pure virtual, must be implemented by derived class)
void TexEffect::setShaders() 
{
	// set the tex pixel and vertex shaders
	m_pDeviceContext->VSSetShader(m_pTexVertexShader.p, NULL, 0);
	m_pDeviceContext->PSSetShader(m_pTexPixelShader.p, NULL, 0);
}

// Creates all the shaders required for the effect, basically calls to create*Shader. (pure virtual, must be implemented by derived class)
void TexEffect::createShaders() 
{
	// create the tex vertex shader
	const wpath texVertexShaderPath(constructShaderPath(m_shaderRoot, wpath(L"texVS.cso")));
	createVertexShader(texVertexShaderPath, m_pTexVertexByteCode, m_pTexVertexShader);

	// create the tex pixel shader
	const wpath texPixelShaderPath(constructShaderPath(m_shaderRoot, wpath(L"texPS.cso")));
	createPixelShader(texPixelShaderPath, m_pTexPixelByteCode, m_pTexPixelShader);	
}

// Initializes the input layout for the effect. (pure virtual, must be implemented by derived class)
void TexEffect::initInputLayout()
{
	// compute the input layout using the vertex shader's byte code
	createInputLayoutFromShaderInfo(m_pTexVertexByteCode, m_pInputLayout);
}

// Creates and initializes the constant buffers of all shaders. (pure virtual, must be implemented by derived class)
void TexEffect::initShaderConstantBuffers() 
{
	// get info about the constant buffer 'cbPerObject' in the vertex shader
	const std::string vsBufName("cbPerObject");
	createConstantBuffer(m_pTexVertexByteCode, vsBufName, m_cbPerObject);

	// get info about the constant buffer 'cbPerFrame' in the pixel shader
	const std::string psBufName("cbPerFrame");
	createConstantBuffer(m_pTexPixelByteCode, psBufName, m_cbPerFrame);
}

// Initializes the resources of all shaders. (pure virtual, must be implemented by derived class)
void TexEffect::initShaderResources() 
{
	// get the binding descriptions of all the shader texture resources
	ShaderReflectionPtr pReflector;
	HR(D3DReflect(m_pTexPixelByteCode->GetBufferPointer(), m_pTexPixelByteCode->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&pReflector.p)));

	m_resDiffuseMap.strName = "gDiffuseMap";
	HR(pReflector->GetResourceBindingDescByName(m_resDiffuseMap.strName.c_str(), &m_resDiffuseMap.bindDesc));

	m_resSpecMap.strName = "gSpecMap";
	HR(pReflector->GetResourceBindingDescByName(m_resSpecMap.strName.c_str(), &m_resSpecMap.bindDesc));

	// close the reflection interface
	pReflector.Release();

	// @TODO: add implementation here
}

// Initializes the sampler states of all shaders. (pure virtual, must be implemented by derived class)
void TexEffect::initShaderSamplers()
{
	// get the binding description of the gTriLinearSam sampler state
	ShaderReflectionPtr pReflector;
	HR(D3DReflect(m_pTexPixelByteCode->GetBufferPointer(), m_pTexPixelByteCode->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&pReflector.p)));
	
	m_samTrilinear.strName = "gTriLinearSam";
	HR(pReflector->GetResourceBindingDescByName(m_samTrilinear.strName.c_str(), &m_samTrilinear.bindDesc));
	
	pReflector.Release();

	// define the sampler state
	SamplerStateMaker samplerStateMaker;
	samplerStateMaker.makeTriLinear(m_pDevice, m_samTrilinear.pSamplerState);

	// @TODO: add implementation here
}