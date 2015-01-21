// Source file for the SkyboxEffect class

#include <SkyboxEffect.h>

#include <SamplerStateMaker.h>

#include <RenderStateMaker.h>

// Default ctor
SkyboxEffect::EffectParams::EffectParams() : scale(1.0f), eyePosW(0.0f, 0.0f, 0.0f), viewProj(DXMatrix::Identity()), pSkyboxTexture(NULL)
{}

// Param ctor
SkyboxEffect::SkyboxEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext, const float fSkyboxRadius)
: DXEffect(shaderRoot, pDevice, pDeviceContext), m_fSkyboxRadius(fSkyboxRadius), m_pVSBuffer(NULL), m_nVSBufferSize(0)
{
	// Initialize the skybox effect (RAII)
	init();
}

// Dtor
SkyboxEffect::~SkyboxEffect()
{}

// Causes the effect to be applied with the given params.
void SkyboxEffect::apply(const EffectParams &params)
{
	// Set the skybox vertex shader and its params.
	m_pDeviceContext->VSSetShader(m_vertexShaders["SkyboxVS"], NULL, 0);

	const DXMatrix skyboxWorld(DXMatrix::CreateScale(params.scale) * DXMatrix::CreateTranslation(params.eyePosW));	// world transform = skybox scale centered around the camera
	setConstantBufferVariableDataWithMatrix(skyboxWorld * params.viewProj, "gWVP", m_constantBuffers["SkyboxVS"]);
	populateConstantBufferData(m_constantBuffers["SkyboxVS"]);
	std::array<ID3D11Buffer*, 1> ppConstBuf = { m_constantBuffers["SkyboxVS"].pBuffer.p };
	m_pDeviceContext->VSSetConstantBuffers(m_constantBuffers["SkyboxVS"].bindDesc.BindPoint, ppConstBuf.size(), ppConstBuf.data());

	// Set the skybox pixel shader and its params.
	m_pDeviceContext->PSSetShader(m_pixelShaders["SkyboxPS"], NULL, 0);
	std::array<ID3D11ShaderResourceView*, 1> ppSRV = { params.pSkyboxTexture };
	m_pDeviceContext->PSSetShaderResources(0, ppSRV.size(), ppSRV.data());
	std::array<ID3D11SamplerState*, 1> ppSam = { m_samplers["SkyboxLinear"].pSamplerState.p };
	m_pDeviceContext->PSSetSamplers(0, ppSam.size(), ppSam.data());

	// Retain a backup of the current rasterizer and depth-stencil states as they will be modified for the skybox.
	RasterizerStatePtr pOriginalRS;
	m_pDeviceContext->RSGetState(&pOriginalRS.p);

	DepthStencilStatePtr pOriginalDSS;
	UINT uiOriginalStencilRef;
	m_pDeviceContext->OMGetDepthStencilState(&pOriginalDSS.p, &uiOriginalStencilRef);
	
	// Set the cull_none rasterizer state and less_equal depth-stencil states.
	m_pDeviceContext->RSSetState(m_rasterStates["SkyboxCullNone"].p);
	m_pDeviceContext->OMSetDepthStencilState(m_depthStencilStates["SkyboxLessEqual"].p, 0);

	// Retain a backup of the current input layout and primitive topology.
	InputLayoutPtr pOriginalInputLayout;
	D3D11_PRIMITIVE_TOPOLOGY originalTopology;
	m_pDeviceContext->IAGetInputLayout(&pOriginalInputLayout.p);
	m_pDeviceContext->IAGetPrimitiveTopology(&originalTopology);

	// Set the skybox's input layout and triangle primitive topology.
	m_pDeviceContext->IASetInputLayout(m_pInputLayout.p);
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw the skybox sphere.
	m_pSkyboxSphere->draw();

	// Restore the original input layout and primitive topology.
	m_pDeviceContext->IASetPrimitiveTopology(originalTopology);
	m_pDeviceContext->IASetInputLayout(pOriginalInputLayout.p);

	// Restore the original rasterizer and depth-stencil states.
	m_pDeviceContext->OMSetDepthStencilState(pOriginalDSS.p, uiOriginalStencilRef);
	m_pDeviceContext->RSSetState(pOriginalRS.p);

	// Unbind the shaders, resources and sampler states from the pipeline.
	m_pDeviceContext->VSSetShader(NULL, NULL, 0);
	ppConstBuf[0] = NULL;
	m_pDeviceContext->VSSetConstantBuffers(m_constantBuffers["SkyboxVS"].bindDesc.BindPoint, ppConstBuf.size(), ppConstBuf.data());

	m_pDeviceContext->PSSetShader(NULL, NULL, 0);
	ppSRV[0] = NULL;
	m_pDeviceContext->PSSetShaderResources(0, ppSRV.size(), ppSRV.data());
	ppSam[0] = NULL;
	m_pDeviceContext->PSSetSamplers(0, ppSam.size(), ppSam.data());
}

// Initialization function which creates all the shaders and initializes
// all shader constant buffers, resources and sampler states.
void SkyboxEffect::init()
{
	DXEffect::init();	// Invoke the base class version
}

// Returns a string which contains effect info
std::wstring SkyboxEffect::getEffectInfo()
{
	return L"";
}

// Unbinds all the shaders, resources and samplers that were bound to the pipeline by this effect.
void SkyboxEffect::cleanup()
{}

// All the constant buffers across all shaders are set. (pure virtual, must be implemented by derived class)
void SkyboxEffect::setShaderConstantBuffers()/* =0*/	
{}

// All the shader textures(resources) are set. (pure virtual, must be implemented by derived class)
void SkyboxEffect::setShaderResources()/* =0*/	
{}

// All the shader samplers are set. (pure virtual, must be implemented by derived class)
void SkyboxEffect::setShaderSamplers()/* =0*/
{}

// All the shaders are set. (pure virtual, must be implemented by derived class)
void SkyboxEffect::setShaders()/* =0*/
{}

// Creates all the shaders required for the effect, basically calls to create*Shader. (pure virtual, must be implemented by derived class)
void SkyboxEffect::createShaders()/* =0*/
{
	// Create the skybox vertex shader.
	wpath shaderPath(constructShaderPath(m_shaderRoot, L"skyboxVS.cso"));
	createVertexShader(shaderPath, m_shaderBlobs["SkyboxVS"], m_vertexShaders["SkyboxVS"]);

	// Set the vertex shader buffer pointer and size.
	m_pVSBuffer = m_shaderBlobs["SkyboxVS"]->GetBufferPointer();
	m_nVSBufferSize = m_shaderBlobs["SkyboxVS"]->GetBufferSize();

	// Create the skybox pixel shader.
	shaderPath = constructShaderPath(m_shaderRoot, L"skyboxPS.cso");
	createPixelShader(shaderPath, m_shaderBlobs["SkyboxPS"], m_pixelShaders["SkyboxPS"]);
}

// Initializes the input layout for the effect. (pure virtual, must be implemented by derived class)
void SkyboxEffect::initInputLayout()/* =0*/
{
	// Create the input layout from the skybox vertex shader.
	createInputLayoutFromShaderInfo(m_shaderBlobs["SkyboxVS"], m_pInputLayout);
}

// Creates and initializes the constant buffers of all shaders. (pure virtual, must be implemented by derived class)
void SkyboxEffect::initShaderConstantBuffers()/* =0*/
{
	// Create the cbPerObject constant buffer in the skybox vertex shader.
	createConstantBuffer(m_shaderBlobs["SkyboxVS"], "cbPerObject", m_constantBuffers["SkyboxVS"]);

	// Init the skybox sphere.
	m_pSkyboxSphere.reset(new ComplexSphere(m_pDevice, m_fSkyboxRadius));
}

// Initializes the resources of all shaders. (pure virtual, must be implemented by derived class)
void SkyboxEffect::initShaderResources()/* =0*/
{
	// @TODO: add implementation here
}

// Initializes the sampler states of all shaders. (pure virtual, must be implemented by derived class)
void SkyboxEffect::initShaderSamplers()/* =0*/
{
	// Init the skybox sampler to a trilinear sampler state.
	SamplerStateMaker samplerStateMaker;
	samplerStateMaker.makeTriLinear(m_pDevice, m_samplers["SkyboxLinear"].pSamplerState);

	// Init the skybox rasterizer and depth-stencil states to cull none and less_equal respectively.
	RenderStateMaker renderStateMaker;
	renderStateMaker.createCullNoneRS(m_pDevice, m_rasterStates["SkyboxCullNone"]);
	renderStateMaker.createLessEqualDepthTestingDSS(m_pDevice, m_depthStencilStates["SkyboxLessEqual"]);
}