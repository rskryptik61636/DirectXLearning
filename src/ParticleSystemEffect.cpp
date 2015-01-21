// Source file of the ParticleSystemEffect class

#include <ParticleSystemEffect.h>

#include <SamplerStateMaker.h>

// EffectParams Default ctor
ParticleSystemEffect::EffectParams::EffectParams()
: eTech(ParticleSystemEffect::ET_UNKNOWN), eyePosW(0.0f, 0.0f, 0.0f), viewProj(DXMatrix::Identity()),
initialAcceleration(0.0f, 0.0f, 0.0f), initialVelocity(0.0f, 0.0f, 0.0f),
lifetime(0.0f), emitterMinPosW(0.0f, 0.0f, 0.0f), emitterMaxPosW(0.0f, 0.0f, 0.0f),
size(0.0f, 0.0f, 0.0f), pParticleTexture(NULL), pGBufferPositionMap(NULL), pGBufferNormalMap(NULL),
pGBufferDiffuseMap(NULL), pGBufferSpecularMap(NULL), eRenderTech(ParticleSystemEffect::RT_FORWARD)
{}

// Param ctor
ParticleSystemEffect::ParticleSystemEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext,
	const UINT nMaxParticles, const EffectParams &initParams) : DXEffect(shaderRoot, pDevice, pDeviceContext), m_nMaxParticles(nMaxParticles),
	m_initParams(initParams), m_nSpawnParticles(8), m_nUpdateParticles(512), m_fTimeStep(0.0f), m_particlesColour(BEACH_SAND),
	m_fSpawnTime(m_initParams.lifetime * (float)m_nSpawnParticles / (float)m_nMaxParticles), m_fSpawnStep(m_fSpawnTime),
	m_pVSBuffer(NULL), m_nVSBufferSize(NULL), m_pCurrentSimParticles(NULL), m_pNewSimParticles(NULL)
{}

// Dtor
ParticleSystemEffect::~ParticleSystemEffect()
{}

// Causes the effect to be applied w/ the given params.
void ParticleSystemEffect::apply(const EffectParams &params)
{
	// Reset the spawn step if it exceeds the spawn time and spawn new particles.
	if (m_fSpawnStep >= m_fSpawnTime)
	{
		//::OutputDebugString(L"Spawned particles!!!\n\n");

		m_fSpawnStep = 0.0f;

		// Bind the particle system insert CS to the pipeline.
		m_pDeviceContext->CSSetShader(m_computeShaders["ParticleSystemInsertCS"], NULL, 0);

		// Populate the contents of the cbPerObject constant buffer in the particle system insert compute shader.
		setConstantBufferVariableTypedDatum<DXColor>(m_particlesColour, "gColour", m_constantBuffers["ParticleSystemInsertCS"]);
		setConstantBufferVariableTypedDatum<float>(m_fTimeStep, "gTimeStep", m_constantBuffers["ParticleSystemInsertCS"]);
		setConstantBufferVariableTypedDatum<DXVector3>(m_initParams.size, "gSize", m_constantBuffers["ParticleSystemInsertCS"]);
		setConstantBufferVariableTypedDatum<DXVector3>(params.emitterMinPosW, "gEmitterPosMinW", m_constantBuffers["ParticleSystemInsertCS"]);
		setConstantBufferVariableTypedDatum<DXVector3>(params.emitterMaxPosW, "gEmitterPosMaxW", m_constantBuffers["ParticleSystemInsertCS"]);		
		populateConstantBufferData(m_constantBuffers["ParticleSystemInsertCS"]);
		std::array<ID3D11Buffer* /*const*/, 1> ppConstBuf = { m_constantBuffers["ParticleSystemInsertCS"].pBuffer.p };
		m_pDeviceContext->CSSetConstantBuffers(m_constantBuffers["ParticleSystemInsertCS"].bindDesc.BindPoint, ppConstBuf.size(), ppConstBuf.data());

		// Set the particle buffer's UAV in the particle system update compute shader.
		/*const*/ std::array<ID3D11UnorderedAccessView* /*const*/, 1> ppUAV = { m_pCurrentSimParticles->pBuffer->GetUnorderedAccess() };
		std::array<UINT, 1> pUAVInit = { -1 };
		m_pDeviceContext->CSSetUnorderedAccessViews(m_pCurrentSimParticles->bindDesc.BindPoint, ppUAV.size(), ppUAV.data(), pUAVInit.data());

		// Set the random number texture in the particle system update compute shader.
		/*const*/ std::array<ID3D11ShaderResourceView* /*const*/, 1> ppSRV = { m_resources["RandomNumber"].pResourceView.p };
		m_pDeviceContext->CSSetShaderResources(0, ppSRV.size(), ppSRV.data());

		// Set the random number texture sampler state to a point sampler in the forward rendering vertex shader.
		std::array<ID3D11SamplerState*, 1> ppSam = { m_samplerStates["PointSampler"].p };
		m_pDeviceContext->CSSetSamplers(0, ppSam.size(), ppSam.data());

		// Dispatch one thread group along (X,Y,Z) to spawn particles
		m_pDeviceContext->Dispatch(1, 1, 1);

		// Unbind all the resources from the pipeline
		m_pDeviceContext->CSSetShader(NULL, NULL, 0);

		ppConstBuf[0] = NULL;
		m_pDeviceContext->CSSetConstantBuffers(m_constantBuffers["ParticleSystemInsertCS"].bindDesc.BindPoint, ppConstBuf.size(), ppConstBuf.data());

		ppUAV[0] = NULL;
		m_pDeviceContext->CSSetUnorderedAccessViews(m_pCurrentSimParticles->bindDesc.BindPoint, ppUAV.size(), ppUAV.data(), pUAVInit.data());

		ppSRV[0] = NULL;
		m_pDeviceContext->CSSetShaderResources(0, ppSRV.size(), ppSRV.data());

		ppSam[0] = NULL;
		m_pDeviceContext->CSSetSamplers(0, ppSam.size(), ppSam.data());
	}

	// Display the current counts of the current and new simulation states.
	const UINT nCurrParticles(getStructureCount(m_pCurrentSimParticles->pBuffer->GetUnorderedAccess()));
	//displaySimStates();

	// Update the particles in the current simulation state and add them to the new simulation state.
	m_pDeviceContext->CSSetShader(m_computeShaders["ParticleSystemUpdateCS"], NULL, 0);

	// Populate the contents of the cbPerObject constant buffer in the particle system insert compute shader.
	setConstantBufferVariableTypedDatum<UINT>(nCurrParticles, "gNumParticles", m_constantBuffers["ParticleSystemUpdateCS"]);
	setConstantBufferVariableTypedDatum<float>(m_fTimeStep, "gTimeStep", m_constantBuffers["ParticleSystemUpdateCS"]);
	setConstantBufferVariableTypedDatum<DXVector3>(params.initialAcceleration, "gAccel", m_constantBuffers["ParticleSystemUpdateCS"]);
	setConstantBufferVariableTypedDatum<float>(params.lifetime, "gLifetime", m_constantBuffers["ParticleSystemUpdateCS"]);
	populateConstantBufferData(m_constantBuffers["ParticleSystemUpdateCS"]);
	std::array<ID3D11Buffer* /*const*/, 1> ppConstBuf = { m_constantBuffers["ParticleSystemUpdateCS"].pBuffer.p };
	m_pDeviceContext->CSSetConstantBuffers(m_constantBuffers["ParticleSystemUpdateCS"].bindDesc.BindPoint, ppConstBuf.size(), ppConstBuf.data());

	// Bind the current and new simulation states' UAVs to the pipeline.
	std::array<ID3D11UnorderedAccessView*, 2> ppUAV = { m_pCurrentSimParticles->pBuffer->GetUnorderedAccess(), 
														m_pNewSimParticles->pBuffer->GetUnorderedAccess() };
	const std::array<UINT, 2> pUAVInit = { -1, -1 };
	m_pDeviceContext->CSSetUnorderedAccessViews(0, ppUAV.size(), ppUAV.data(), pUAVInit.data());

	// Dispatch as many thread groups as required to update the particles.
	m_pDeviceContext->Dispatch(m_nMaxParticles / m_nUpdateParticles, 1, 1);

	// Unbind the resources from the pipeline.
	m_pDeviceContext->CSSetShader(NULL, NULL, 0);

	ppConstBuf[0] = NULL;
	m_pDeviceContext->CSSetConstantBuffers(m_constantBuffers["ParticleSystemUpdateCS"].bindDesc.BindPoint, ppConstBuf.size(), ppConstBuf.data());

	ppUAV[0] = NULL;	ppUAV[1] = NULL;
	m_pDeviceContext->CSSetUnorderedAccessViews(0, ppUAV.size(), ppUAV.data(), pUAVInit.data());

	/*if (m_fTimeStep > params.lifetime)
		m_fTimeStep = 0.0f;*/

	// Display the current counts of the current and new simulation states.
	//displaySimStates();

	// Bind the shaders to the pipeline
	m_pDeviceContext->VSSetShader(m_vertexShaders["ForwardVS"], NULL, 0);
	m_pDeviceContext->GSSetShader(m_geometryShaders["ForwardRainGS"], NULL, 0);
	//m_pDeviceContext->PSSetShader(m_pixelShaders["ForwardBasicPS"], NULL, 0);	// @TODO: remove when done testing

	// Set the particle system SRV in the forward rendering vertex shader.
	/*const*/ std::array<ID3D11ShaderResourceView* /*const*/, 1> ppSRV2 = { m_pNewSimParticles->pBuffer->GetShaderResource() };
	m_pDeviceContext->VSSetShaderResources(0, ppSRV2.size(), ppSRV2.data());

	// Populate the contents of the cbPerObject constant buffer in the forward rendering vertex shader.
	setConstantBufferVariableDataWithMatrix(params.viewProj, "gViewProj", m_constantBuffers["ForwardRainGS"]);
	setConstantBufferVariableTypedDatum<DXVector3>(params.size, "gSize", m_constantBuffers["ForwardRainGS"]);
	populateConstantBufferData(m_constantBuffers["ForwardRainGS"]);
	ppConstBuf[0] = m_constantBuffers["ForwardRainGS"].pBuffer.p;
	m_pDeviceContext->GSSetConstantBuffers(m_constantBuffers["ForwardRainGS"].bindDesc.BindPoint, ppConstBuf.size(), ppConstBuf.data());

	// Set the pixel shader and its params based on the given rendering technique.
	std::array<ID3D11SamplerState*, 1> ppSam;
	switch (params.eRenderTech)
	{
		case RT_FORWARD:
		{
			// Set the forward rendering pixel shader in the pipeline.
			m_pDeviceContext->PSSetShader(m_pixelShaders["ForwardBasicPS"], NULL, 0);

			// Set the particle texture in the forward rendering pixel shader.
			ppSRV2[0] = params.pParticleTexture;
			m_pDeviceContext->PSSetShaderResources(0, ppSRV2.size(), ppSRV2.data());

			// Set the tri linear sampler state in the forward rendering pixel shader.
			ppSam[0] = m_samplerStates["LinearSampler"];
			m_pDeviceContext->PSSetSamplers(0, ppSam.size(), ppSam.data());
		}
		break;

		case RT_DEFERRED:
		{
			// Set the deferred rendering pixel shader in the pipeline.
			m_pDeviceContext->PSSetShader(m_pixelShaders["DeferredBasicPS"], NULL, 0);

			// Populate and set the cbPerObject constant buffer.
			setConstantBufferVariableTypedDatum<DXVector3>(params.eyePosW, "gEyePosW", m_constantBuffers["DeferredBasicPS"]);
			populateConstantBufferData(m_constantBuffers["DeferredBasicPS"]);
			ppConstBuf[0] = m_constantBuffers["DeferredBasicPS"].pBuffer.p;
			m_pDeviceContext->PSSetConstantBuffers(0, ppConstBuf.size(), ppConstBuf.data());

			// Set the particle texture in the forward rendering pixel shader.
			ppSRV2[0] = params.pParticleTexture;
			m_pDeviceContext->PSSetShaderResources(0, ppSRV2.size(), ppSRV2.data());

			// Set the tri linear sampler state in the forward rendering pixel shader.
			ppSam[0] = m_samplerStates["LinearSampler"];
			m_pDeviceContext->PSSetSamplers(0, ppSam.size(), ppSam.data());
		}
		break;
	}

	// @TODO: remove when done testing
	//// Set the particle texture in the forward rendering pixel shader.
	//ppSRV2[0] = params.pParticleTexture;
	//m_pDeviceContext->PSSetShaderResources(0, ppSRV2.size(), ppSRV2.data());

	//// Set the tri linear sampler state in the forward rendering pixel shader.
	//std::array<ID3D11SamplerState*, 1> ppSam = { m_samplerStates["LinearSampler"] };
	//m_pDeviceContext->PSSetSamplers(0, ppSam.size(), ppSam.data());

	// Draw the particle system based on the current type of effect
	switch (params.eTech)
	{
	case ET_RAIN:
	{
					// Set the primitive topology to a point list, bind no vertex buffer and draw the max no. of particles
					m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
					m_pDeviceContext->IASetInputLayout(NULL);
					m_pDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);

					// setting one of the supported types for index buffer format as it *shouldn't* matter since no index buffer is being bound
					//m_pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R16_UINT, 0);	

					m_pDeviceContext->Draw(/*5*/ nCurrParticles, 0);
	}
		break;

	case ET_FIRE:
	{
					// @TODO: add implementation here
	}
		break;
	}

	// Unbind the resources from the pipeline.
	m_pDeviceContext->VSSetShader(NULL, NULL, 0);
	m_pDeviceContext->GSSetShader(NULL, NULL, 0);
	m_pDeviceContext->PSSetShader(NULL, NULL, 0);
	//m_pDeviceContext->CSSetShader(m_computeShaders["ParticleSystemUpdateCS"], NULL, 0);

	// Populate the contents of the cbPerObject constant buffer in the forward rendering geometry shader.
	ppConstBuf[0] = NULL;
	m_pDeviceContext->GSSetConstantBuffers(m_constantBuffers["ForwardRainGS"].bindDesc.BindPoint, ppConstBuf.size(), ppConstBuf.data());

	// Set the particle system SRV in the forward rendering vertex shader.
	ppSRV2[0] = NULL;
	m_pDeviceContext->VSSetShaderResources(0, ppSRV2.size(), ppSRV2.data());

	// Set the particle texture in the forward rendering pixel shader.
	ppSRV2[0] = NULL;
	m_pDeviceContext->PSSetShaderResources(0, ppSRV2.size(), ppSRV2.data());

	// Set the tri linear sampler state in the forward rendering pixel shader.
	ppSam[0] = NULL;
	m_pDeviceContext->PSSetSamplers(0, ppSam.size(), ppSam.data());

	// Swap the simulation states for the next iteration.
	swapSimStates();

	// @TODO: remove when done testing
#if 0
	// Populate the contents of the cbPerObject constant buffer in the particle system update compute shader.
	setConstantBufferVariableTypedDatum<DXVector3>(params.initialAcceleration, "gAccel", m_constantBuffers["ParticleSystemUpdateCS"]);
	setConstantBufferVariableTypedDatum<float>(m_fTimeStep, "gTimeStep", m_constantBuffers["ParticleSystemUpdateCS"]);
	setConstantBufferVariableTypedDatum<DXVector3>(params.emitterMinPosW, "gEmitterPosMinW", m_constantBuffers["ParticleSystemUpdateCS"]);
	setConstantBufferVariableTypedDatum<DXVector3>(params.emitterMaxPosW, "gEmitterPosMaxW", m_constantBuffers["ParticleSystemUpdateCS"]);
	setConstantBufferVariableTypedDatum<float>(params.lifetime, "gLifetime", m_constantBuffers["ParticleSystemUpdateCS"]);
	populateConstantBufferData(m_constantBuffers["ParticleSystemUpdateCS"]);
	m_pDeviceContext->CSSetConstantBuffers(m_constantBuffers["ParticleSystemUpdateCS"].bindDesc.BindPoint, 1, &m_constantBuffers["ParticleSystemUpdateCS"].pBuffer.p);

	// Set the particle buffer's UAV in the particle system update compute shader.
	const std::array<ID3D11UnorderedAccessView* const, 1> ppUAV = { m_particlesBuffer.pBuffer->GetUnorderedAccess() };
	m_pDeviceContext->CSSetUnorderedAccessViews(m_particlesBuffer.bindDesc.BindPoint, ppUAV.size(), ppUAV.data(), NULL);

	// Set the random number texture in the particle system update compute shader.
	const std::array<ID3D11ShaderResourceView* const, 1> ppSRV = { m_resources["RandomNumber"].pResourceView.p };
	m_pDeviceContext->CSSetShaderResources(0, ppSRV.size(), ppSRV.data());

	// Set the random number texture sampler state to a point sampler in the forward rendering vertex shader.
	m_pDeviceContext->CSSetSamplers(0, 1, &m_samplerStates["PointSampler"].p);

	// Dispatch enough threads to handle all the particles.
	m_pDeviceContext->Dispatch(/*50*/m_nMaxParticles / 512, 1, 1);

	// Unbind the particle buffer's UAV from the particle system update compute shader so that it can be accessed as a shader resource
	// inside the forward rendering vertex shader.
	const std::array<ID3D11UnorderedAccessView* const, 1> ppUAVNull = { NULL };
	m_pDeviceContext->CSSetUnorderedAccessViews(m_particlesBuffer.bindDesc.BindPoint, ppUAVNull.size(), ppUAVNull.data(), NULL);
#endif // 0


	
	// @TODO: add implementation here
}

// Unbinds all the shaders, resources and samplers that were bound to the pipeline by this effect.
// NOTE: This function is not really required as all the cleanup happens inside the apply() method.
void ParticleSystemEffect::cleanup()
{
#if 0
	// Unbind the shaders from the pipeline
	m_pDeviceContext->VSSetShader(NULL, NULL, 0);
	m_pDeviceContext->PSSetShader(NULL, NULL, 0);
	m_pDeviceContext->CSSetShader(NULL, NULL, 0);

	// Unbind the constant buffer in the particle system update compute shader.
	const std::array<ID3D11Buffer*, 1> ppNullBuffer = { NULL };
	m_pDeviceContext->CSSetConstantBuffers(m_constantBuffers["ParticleSystemUpdateCS"].bindDesc.BindPoint, ppNullBuffer.size(), ppNullBuffer.data());

	// Unbind the random number texture in the particle system update compute shader.
	const std::array<ID3D11ShaderResourceView* const, 1> ppSRV = { NULL };
	m_pDeviceContext->CSSetShaderResources(0, ppSRV.size(), ppSRV.data());

	// Set the random number texture sampler state to a point sampler in the forward rendering vertex shader.
	const std::array<ID3D11SamplerState* const, 1> ppSam = { NULL };
	m_pDeviceContext->CSSetSamplers(0, ppSam.size(), ppSam.data());

	// Populate the contents of the cbPerObject constant buffer in the forward rendering geometry shader.
	m_pDeviceContext->GSSetConstantBuffers(m_constantBuffers["ForwardRainGS"].bindDesc.BindPoint, ppNullBuffer.size(), ppNullBuffer.data());

	// Set the particle system SRV in the forward rendering vertex shader.
	const std::array<ID3D11ShaderResourceView* const, 1> ppSRV2 = { NULL };
	m_pDeviceContext->VSSetShaderResources(0, ppSRV2.size(), ppSRV2.data());
#endif // 0

}

// Initialization function which creates all the shaders and initializes
// all shader constant buffers, resources and sampler states.
void ParticleSystemEffect::init()
{
	DXEffect::init();

	// @TODO: add implementation here
}

// Returns a string which contains effect info
std::wstring ParticleSystemEffect::getEffectInfo()
{
	// @TODO: add implementation here
	return L"";
}

// Creates all the shaders required for the effect, basically calls to create*Shader. (pure virtual, must be implemented by derived class)
void ParticleSystemEffect::createShaders()
{
	// Initialize the forward rendering vertex shader
  	wpath shaderPath(constructShaderPath(m_shaderRoot, L"forwardParticleSystemVS.cso"));
	createVertexShader(shaderPath, m_shaderBlobs["ForwardVS"], m_vertexShaders["ForwardVS"]);

	// Set the vertex shader and buffer pointers.
	m_pVSBuffer = m_shaderBlobs["ForwardVS"]->GetBufferPointer();
	m_nVSBufferSize = m_shaderBlobs["ForwardVS"]->GetBufferSize();

	// Initialize the forward rendering rain geometry shader
	shaderPath = constructShaderPath(m_shaderRoot, L"forwardParticleSystemRainGS.cso");
	createGeometryShader(shaderPath, m_shaderBlobs["ForwardRainGS"], m_geometryShaders["ForwardRainGS"]);

	// Initialize the basic forward rendering pixel shader
	shaderPath = constructShaderPath(m_shaderRoot, L"forwardParticleSystemBasicPS.cso");
	createPixelShader(shaderPath, m_shaderBlobs["ForwardBasicPS"], m_pixelShaders["ForwardBasicPS"]);

	// Initialize the basic deferred rendering pixel shader
	shaderPath = constructShaderPath(m_shaderRoot, L"deferredParticleSystemBasicPS.cso");
	createPixelShader(shaderPath, m_shaderBlobs["DeferredBasicPS"], m_pixelShaders["DeferredBasicPS"]);

	// Initialize the particle system insert compute shader
	shaderPath = constructShaderPath(m_shaderRoot, L"particleSystemInsertCS.cso");
	createComputeShader(shaderPath, m_shaderBlobs["ParticleSystemInsertCS"], m_computeShaders["ParticleSystemInsertCS"]);

	// Initialize the particle system update compute shader
	shaderPath = constructShaderPath(m_shaderRoot, L"particleSystemUpdateCS.cso");
	createComputeShader(shaderPath, m_shaderBlobs["ParticleSystemUpdateCS"], m_computeShaders["ParticleSystemUpdateCS"]);
}

// Initializes the input layout for the effect. (pure virtual, must be implemented by derived class)
void ParticleSystemEffect::initInputLayout()
{
	createInputLayoutFromShaderInfo(m_shaderBlobs["ForwardVS"], m_pInputLayout);
}

// Creates and initializes the constant buffers of all shaders. (pure virtual, must be implemented by derived class)
void ParticleSystemEffect::initShaderConstantBuffers()
{
	// Initialize the cbPerObject constant buffer in the forward rendering rain geometry shader.
	createConstantBuffer(m_shaderBlobs["ForwardRainGS"], "cbPerObject", m_constantBuffers["ForwardRainGS"]);

	// Initialize the cbPerObject constant buffer in the basic deferred rendering pixel shader.
	createConstantBuffer(m_shaderBlobs["DeferredBasicPS"], "cbPerObject", m_constantBuffers["DeferredBasicPS"]);

	// Initialize the cbPerObject constant buffer in the particle system insert compute shader.
	createConstantBuffer(m_shaderBlobs["ParticleSystemInsertCS"], "cbPerObject", m_constantBuffers["ParticleSystemInsertCS"]);

	// Initialize the cbPerObject constant buffer in the particle system update compute shader.
	createConstantBuffer(m_shaderBlobs["ParticleSystemUpdateCS"], "cbPerObject", m_constantBuffers["ParticleSystemUpdateCS"]);

	// Create a staging buffer to display the structure counts of the particle system buffers.
	// Source: http://msdn.microsoft.com/en-us/library/windows/desktop/ff476393%28v=vs.85%29.aspx
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = 4;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	HR(m_pDevice->CreateBuffer(&desc, NULL, &m_buffers["StructureCounter"]));

	// @TODO: add implementation here
}

// Initializes the resources of all shaders. (pure virtual, must be implemented by derived class)
void ParticleSystemEffect::initShaderResources()
{
	// Use a constant seed for consistency
	std::mt19937 rng(1337);

	// Use random no's in [-1,1] for the initial velocity
	const DXVector3 &minPosW = m_initParams.emitterMinPosW, &maxPosW = m_initParams.emitterMaxPosW;
	std::uniform_real<float> velocityX(minPosW.x, maxPosW.x), velocityY(minPosW.y, maxPosW.y), velocityZ(minPosW.z, maxPosW.z);

	// Initialize the particles buffer with the initial particle system params.
	//Particle* pParticle = m_particlesBuffer.pBuffer->MapDiscard(m_pDeviceContext);
	std::vector<Particle> pParticle(m_nMaxParticles);
	for (std::size_t i = 0; i < m_nMaxParticles; ++i)
	{
		pParticle[i].age = 0.0f;
		pParticle[i].initPosition = DXVector3(velocityX(rng), velocityY(rng), velocityZ(rng));	// random position in [minPosW, maxPosW]
		pParticle[i].initVelocity = DXVector3(0.0f, 0.0f, 0.0f); //DXVector3(velocityX(rng), velocityY(rng), velocityZ(rng));
		pParticle[i].size = m_initParams.size;
		pParticle[i].type = PT_PARTICLE;
		pParticle[i].colour = m_particlesColour;
	}
	//m_particlesBuffer.pBuffer->Unmap(m_pDeviceContext);

	// Lets create the structured buffers here.
	const bool bIsDynamic(false), bAppendConsume(true);
	createStructuredBuffer<Particle>(m_shaderBlobs["ParticleSystemUpdateCS"], "gCurrentSimParticles", m_nMaxParticles, 
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, m_particlesBuffer, bIsDynamic, pParticle.data(), bAppendConsume);
	createStructuredBuffer<Particle>(m_shaderBlobs["ParticleSystemUpdateCS"], "gCurrentSimParticles", m_nMaxParticles,
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, m_particlesBuffer2, bIsDynamic, pParticle.data(), bAppendConsume);
	//m_pParticlesBuffer.reset(new StructuredBuffer<Particle>(m_pDevice, m_nMaxParticles));

	// Init the current and new simulation states.
	m_pCurrentSimParticles = &m_particlesBuffer;
	m_pNewSimParticles = &m_particlesBuffer2;

	// Create the random number texture.
	m_resources["RandomNumber"].pResourceView = CreateRandomTexture1DSRV(m_pDevice, m_nMaxParticles /*1024*/);	// @TODO: update so that the texture size is specified as a param	
}

// Initializes the sampler states of all shaders. (pure virtual, must be implemented by derived class)
void ParticleSystemEffect::initShaderSamplers()
{
	// Create a point sampler state.
	SamplerStateMaker samplerStateMaker;
	samplerStateMaker.makeTriPoint(m_pDevice, m_samplerStates["PointSampler"]);
	samplerStateMaker.makeTriLinear(m_pDevice, m_samplerStates["LinearSampler"]);
}

// Mutator function to swap the particle system simulation states.
void ParticleSystemEffect::swapSimStates()
{
	std::swap(m_pCurrentSimParticles, m_pNewSimParticles);
}

// Returns the structure count of the given unordered access view.
// Source: http://msdn.microsoft.com/en-us/library/windows/desktop/ff476393%28v=vs.85%29.aspx
UINT ParticleSystemEffect::getStructureCount(ID3D11UnorderedAccessView *pUAV)
{
	m_pDeviceContext->CopyStructureCount(m_buffers["StructureCounter"].p, 0, pUAV);
	D3D11_MAPPED_SUBRESOURCE subresource;
	HR(m_pDeviceContext->Map(m_buffers["StructureCounter"].p, 0, D3D11_MAP_READ, 0, &subresource));
	UINT numActiveElements = *(UINT*)subresource.pData;
	m_pDeviceContext->Unmap(m_buffers["StructureCounter"], 0);
	//staging->Release();
	return numActiveElements;
}

// Mutator function to set the current time step.
void ParticleSystemEffect::setTimeStep(const float dt)
{ 
	m_fTimeStep = dt; 
	m_fSpawnStep += dt; 
}

// Displays the structure count of the current and new simulation states.
void ParticleSystemEffect::displaySimStates()
{
	::OutputDebugString(std::wstring(L"Current sim particles: " + std::to_wstring(getStructureCount(m_pCurrentSimParticles->pBuffer->GetUnorderedAccess())) + L"\n").c_str());
	::OutputDebugString(std::wstring(L"New sim particles: " + std::to_wstring(getStructureCount(m_pNewSimParticles->pBuffer->GetUnorderedAccess())) + L"\n").c_str());
}