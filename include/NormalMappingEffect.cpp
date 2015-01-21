// Source file for the NormalMappingEffect class

#include "NormalMappingEffect.h"
#include "SamplerStateMaker.h"

// shader constant params
const int NormalMappingEffect::m_nMaxParallelLights = 4;
const int NormalMappingEffect::m_nMaxPointLights = 4;
const int NormalMappingEffect::m_nMaxSpotLights = 4;

// Param ctor
NormalMappingEffect::NormalMappingEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext)
: DXEffect(shaderRoot, pDevice, pDeviceContext)
{}

// Dtor
NormalMappingEffect::~NormalMappingEffect()	{}

// Causes the effect to be applied. (virtual, may be implemented by derived class)
void NormalMappingEffect::apply()
{
	DXEffect::apply();
}

// Initialization function which creates all the shaders and initializes
// all shader constant buffers, resources and sampler states.
void NormalMappingEffect::init()
{
	DXEffect::init();
}

// mutator funcs to set the constant buffer members
// @TODO: update the set functions to use the bind points as defined in the varsInfo.varDesc

// cbPerObject

// set the world matrix
void NormalMappingEffect::setWorld(const DXMatrix &world)
{
	setConstantBufferVariableDataWithMatrix(world, "gWorld", m_cbPerObject);
}

// set the inverse transpose of the world matrix
void NormalMappingEffect::setWorldInvTrans(const DXMatrix &worldInvTrans)
{
	setConstantBufferVariableDataWithMatrix(worldInvTrans, "gWorldInvTrans", m_cbPerObject);
}

// set the world view projection matrix
void NormalMappingEffect::setWVP(const DXMatrix &wvp)
{
	setConstantBufferVariableDataWithMatrix(wvp, "gWVP", m_cbPerObject);
}

// set the texture transformation matrix
void NormalMappingEffect::setTexMtx(const DXMatrix &texMtx)
{
	setConstantBufferVariableDataWithMatrix(texMtx, "gTexMtx", m_cbPerObject);
}

// cbPerFrame

// set the parallel lights
void NormalMappingEffect::setParallelLights(const std::vector<Light> &parallelLights)	// no. of lights can be determined from the vector size
{	
	setLights(parallelLights, m_nMaxParallelLights, "gParallelLights", "nParallelLights", m_cbPerFrame);
}

// set the point lights
void NormalMappingEffect::setPointLights(const std::vector<Light> &pointLights)
{	
	setLights(pointLights, m_nMaxPointLights, "gPointLights", "nPointLights", m_cbPerFrame);
}

// set the spot lights
void NormalMappingEffect::setSpotLights(const std::vector<Light> &spotLights)
{	
	setLights(spotLights, m_nMaxSpotLights, "gSpotLights", "nSpotLights", m_cbPerFrame);
}

// set the world space eye pos
void NormalMappingEffect::setEyePosW(const DXVector3 &eyePosW)
{
	setConstantBufferVariableData(reinterpret_cast<const BYTE*>(&eyePosW), sizeof(eyePosW), "gEyePosW", m_cbPerFrame);
}

// set the diffuse texture map
void NormalMappingEffect::setDiffuseMap(const ShaderResourceViewPtr &pDiffuseMap)
{
	m_resDiffuseMap.pResourceView = pDiffuseMap; 
}

// set the normal map
void NormalMappingEffect::setNormalMap(const ShaderResourceViewPtr &pNormalMap)
{
	m_resNormalMap.pResourceView = pNormalMap;
}

// set the specular light map
void NormalMappingEffect::setSpecularMap(const ShaderResourceViewPtr &pSpecularMap)
{
	m_resSpecMap.pResourceView = pSpecularMap;
}

// All the constant buffers across all shaders are set. (pure virtual, must be implemented by derived class)
void NormalMappingEffect::setShaderConstantBuffers()
{
	// populate the cbPerObject constant buffer and set it in the vertex shader
	populateConstantBufferData(m_cbPerObject);
	const std::vector<ID3D11Buffer*> ppVSBuffers(1, m_cbPerObject.pBuffer.p);
	m_pDeviceContext->VSSetConstantBuffers(m_cbPerObject.bindDesc.BindPoint, ppVSBuffers.size(), ppVSBuffers.data());

	// populate the cbPerFrame constant buffer and set it in the pixel shader
	populateConstantBufferData(m_cbPerFrame);
	const std::vector<ID3D11Buffer*> ppPSBuffers(1, m_cbPerFrame.pBuffer.p);
	m_pDeviceContext->PSSetConstantBuffers(m_cbPerFrame.bindDesc.BindPoint, ppPSBuffers.size(), ppPSBuffers.data());
}

// All the shader textures(resources) are set. (pure virtual, must be implemented by derived class)
void NormalMappingEffect::setShaderResources()
{
	// create an array of the shader resource views that need to be set in the pixel shader
	std::vector<ID3D11ShaderResourceView*> ppResourceViews(3);
	ppResourceViews[0] = m_resDiffuseMap.pResourceView.p;
	ppResourceViews[1] = m_resNormalMap.pResourceView.p;
	ppResourceViews[2] = m_resSpecMap.pResourceView.p;
	m_pDeviceContext->PSSetShaderResources(m_resDiffuseMap.bindDesc.BindPoint, ppResourceViews.size(), ppResourceViews.data());
}

// All the shader samplers are set. (pure virtual, must be implemented by derived class)
void NormalMappingEffect::setShaderSamplers()
{
	// set the samTrilinear sampler state in the pixel shader
	const std::vector<ID3D11SamplerState*> ppSamplers(1, m_samTriLinear.pSamplerState.p);
	m_pDeviceContext->PSSetSamplers(m_samTriLinear.bindDesc.BindPoint, ppSamplers.size(), ppSamplers.data());
}

// All the shaders are set. (pure virtual, must be implemented by derived class)
void NormalMappingEffect::setShaders()
{
	// set the vertex and pixel shaders
	m_pDeviceContext->VSSetShader(m_pNormalMappingVertexShader.p, NULL, 0);
	m_pDeviceContext->PSSetShader(m_pNormalMappingPixelShader.p, NULL, 0);
}

// Creates all the shaders required for the effect, basically calls to create*Shader. (pure virtual, must be implemented by derived class)
void NormalMappingEffect::createShaders()
{
	// create the vertex shader
	const wpath vertexShaderPath(constructShaderPath(m_shaderRoot, wpath(L"normalMappingVS.cso")));
	createVertexShader(vertexShaderPath, m_pNormalMappingVertexByteCode, m_pNormalMappingVertexShader);

	// create the tex pixel shader
	const wpath pixelShaderPath(constructShaderPath(m_shaderRoot, wpath(L"normalMappingPS.cso")));
	createPixelShader(pixelShaderPath, m_pNormalMappingPixelByteCode, m_pNormalMappingPixelShader);
}

// Initializes the input layout for the effect. (pure virtual, must be implemented by derived class)
void NormalMappingEffect::initInputLayout()
{
	// determine the input layout from the vertex shader's byte code
	createInputLayoutFromShaderInfo(m_pNormalMappingVertexByteCode, m_pInputLayout);
}

// Creates and initializes the constant buffers of all shaders. (pure virtual, must be implemented by derived class)
void NormalMappingEffect::initShaderConstantBuffers()
{
	// create the cbPerObject constant buffer
	const std::string vsBufName("cbPerObject");
	createConstantBuffer(m_pNormalMappingVertexByteCode, vsBufName, m_cbPerObject);

	// create the cbPerFrame constant buffer
	const std::string psBufName("cbPerFrame");
	createConstantBuffer(m_pNormalMappingPixelByteCode, psBufName, m_cbPerFrame);
}

// Initializes the resources of all shaders. (pure virtual, must be implemented by derived class)
void NormalMappingEffect::initShaderResources()
{
	// get the binding info of all the pixel shader textures
	ShaderReflectionPtr pReflector;
	HR(D3DReflect(m_pNormalMappingPixelByteCode->GetBufferPointer(), m_pNormalMappingPixelByteCode->GetBufferSize(), 
					IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&pReflector.p)));

	m_resDiffuseMap.strName = "gDiffuseMap";
	HR(pReflector->GetResourceBindingDescByName(m_resDiffuseMap.strName.c_str(), &m_resDiffuseMap.bindDesc));

	m_resNormalMap.strName = "gNormalMap";
	HR(pReflector->GetResourceBindingDescByName(m_resNormalMap.strName.c_str(), &m_resNormalMap.bindDesc));

	m_resSpecMap.strName = "gSpecMap";
	HR(pReflector->GetResourceBindingDescByName(m_resSpecMap.strName.c_str(), &m_resSpecMap.bindDesc));

	// close the reflection interface
	pReflector.Release();
}

// Initializes the sampler states of all shaders. (pure virtual, must be implemented by derived class)
void NormalMappingEffect::initShaderSamplers()
{
	// get the binding info about the trilinear sampler state
	ShaderReflectionPtr pReflector;
	HR(D3DReflect(m_pNormalMappingPixelByteCode->GetBufferPointer(), m_pNormalMappingPixelByteCode->GetBufferSize(),
		IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&pReflector.p)));
	m_samTriLinear.strName = "gTriLinearSam";
	HR(pReflector->GetResourceBindingDescByName(m_samTriLinear.strName.c_str(), &m_samTriLinear.bindDesc));
	pReflector.Release();

	// create the trilinear sampler state
	SamplerStateMaker samplerStateMaker;
	samplerStateMaker.makeTriLinear(m_pDevice, m_samTriLinear.pSamplerState);
}