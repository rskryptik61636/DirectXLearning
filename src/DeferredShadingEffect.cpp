// Source file for the DeferredShadingEffect class

#include "DeferredShadingEffect.h"

#include <SamplerStateMaker.h>

// Default ctor.
DeferredShadingEffect::EffectParams::EffectParams()
: eTech(ET_CLASSIC), ePass(EP_G_BUFFER_GENERATION),
world(NULL), worldInvTrans(NULL), wvp(NULL), texMtx(NULL),
currViewProj(NULL), prevViewProj(NULL), pDiffuseMap(),
pSpecularMap(), pPositionMap(), pNormalMap(), eyePosW(),
parallelLights(NULL), pointLights(NULL), spotLights(NULL),
nMotionBlurSamples(0), shadowMapDims(DXVector2(0.0f, 0.0f)),
pShadowMap(), spotLightsShadowTransforms(NULL)
{}

// Param ctor
// @param shaderRoot: Absolute path of the shaders root directory.
// @param pDevice: D3D device object.
// @param pDeviceContext: D3D immediate device context.
// @param uiWidth: Client window width
// @param uiHeight: Client window height
DeferredShadingEffect::DeferredShadingEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext)
: DXEffect(shaderRoot, pDevice, pDeviceContext),
m_nParallelLights(0), m_nPointLights(0), m_nSpotLights(0),
m_eGBufferEffect(DeferredShadingEffect::GE_STANDARD),
m_eShadingEffect(DeferredShadingEffect::SE_STANDARD),
m_pDebugTextureEffect(new DebugTextureEffect(shaderRoot, pDevice, pDeviceContext)),
m_nTileSize(16), m_nTileDims(m_nTileSize*m_nTileSize),	// NOTE: Must reflect the same value as defined in the the tiled deferred shader.
m_bShowLightsPerTile(FALSE), m_nTapSize(0), m_nMaxTapSize(5)
{}

// Param ctor
// @param shaderRoot: Absolute path of the shaders root directory.
// @param pDevice: D3D device object.
// @param pDeviceContext: D3D immediate device context.
// @param nClientWidth: Client window width.
// @param nClientHeight: Client window height.
DeferredShadingEffect::DeferredShadingEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext,
	const UINT nClientWidth, const UINT nClientHeight)
	: DeferredShadingEffect(shaderRoot, pDevice, pDeviceContext)	
{
	// Init the tiled output buffer such that it can be used as both a UAV as well as an SRV.
	m_pTiledOutput.reset(new Texture2D(m_pDevice, nClientWidth, nClientHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE));
}

// Dtor
DeferredShadingEffect::~DeferredShadingEffect()	{}

// Causes the effect to be applied with the given params.
void DeferredShadingEffect::apply(const EffectParams &params)
{
	// Set the current technique.
	m_eCurrTech = params.eTech;

	switch (params.ePass)
	{
		case EP_G_BUFFER_GENERATION:
		{
			// Set the vertex shader and all its params
			m_pDeviceContext->VSSetShader(m_pVS, NULL, 0);

			setConstantBufferVariableDataWithMatrix(*params.texMtx, "gTexMtx", m_VSCBPerObject);
			setConstantBufferVariableDataWithMatrix(*params.world, "gWorld", m_VSCBPerObject);
			setConstantBufferVariableDataWithMatrix(*params.worldInvTrans, "gWorldInvTrans", m_VSCBPerObject);
			setConstantBufferVariableDataWithMatrix(*params.wvp, "gWVP", m_VSCBPerObject);
			populateConstantBufferData(m_VSCBPerObject);
			m_pDeviceContext->VSSetConstantBuffers(m_VSCBPerObject.bindDesc.BindPoint, 1, &m_VSCBPerObject.pBuffer.p);

			// Set the pixel shader and all its params based on the current technique
			switch (m_eGBufferEffect)
			{
				case DeferredShadingEffect::GE_STANDARD:
				{
					m_pDeviceContext->PSSetShader(m_pGBufferPS, NULL, 0);

					const std::array<ID3D11ShaderResourceView* const, 2> ppSRV =
					{ params.pDiffuseMap.p, params.pSpecularMap.p };
					m_pDeviceContext->PSSetShaderResources(0, ppSRV.size(), ppSRV.data());

					m_pDeviceContext->PSSetSamplers(0, 1, &m_samGBufferPS.pSamplerState.p);
				}
				break;

				case DeferredShadingEffect::GE_NORMAL_MAPPING:
				{
					m_pDeviceContext->PSSetShader(m_pGBufferPS2, NULL, 0);

					const std::array<ID3D11ShaderResourceView* const, 3> ppSRV =
					{ params.pDiffuseMap.p, params.pSpecularMap.p, params.pNormalMap.p };
					m_pDeviceContext->PSSetShaderResources(0, ppSRV.size(), ppSRV.data());

					m_pDeviceContext->PSSetSamplers(0, 1, &m_samGBufferPS2.pSamplerState.p);
				}
				break;
			}
		}
		break;

		case EP_DEFERRED_SHADING:
		{
			// Proceed based on the selected technique.
			switch (params.eTech)
			{
				case ET_CLASSIC:
				{
					// Set the vertex shader and all its params
					m_pDeviceContext->VSSetShader(m_pVS, NULL, 0);

					setConstantBufferVariableDataWithMatrix(*params.texMtx, "gTexMtx", m_VSCBPerObject);
					setConstantBufferVariableDataWithMatrix(*params.world, "gWorld", m_VSCBPerObject);
					setConstantBufferVariableDataWithMatrix(*params.worldInvTrans, "gWorldInvTrans", m_VSCBPerObject);
					setConstantBufferVariableDataWithMatrix(*params.wvp, "gWVP", m_VSCBPerObject);
					populateConstantBufferData(m_VSCBPerObject);
					m_pDeviceContext->VSSetConstantBuffers(m_VSCBPerObject.bindDesc.BindPoint, 1, &m_VSCBPerObject.pBuffer.p);

					switch (m_eShadingEffect)
					{
						case DeferredShadingEffect::SE_STANDARD:
						{
							// Set the deferred texture mapping pixel shader and all its params
							m_pDeviceContext->PSSetShader(m_pTexPS, NULL, 0);

							setConstantBufferVariableData(reinterpret_cast<const BYTE*>(&params.eyePosW), sizeof(params.eyePosW), "gEyePosW", m_cbPerFrameTexPS);
							populateConstantBufferData(m_cbPerFrameTexPS);
							m_pDeviceContext->PSSetConstantBuffers(m_cbPerFrameTexPS.bindDesc.BindPoint, 1, &m_cbPerFrameTexPS.pBuffer.p);

							setStructuredLights(*params.parallelLights, m_nParallelLights, m_sbParallelLights);
							setStructuredLights(*params.pointLights, m_nPointLights, m_sbPointLights);
							setStructuredLights(*params.spotLights, m_nSpotLights, m_sbSpotLights);

							const std::array<ID3D11ShaderResourceView* const, 7> ppSRV =
							{ m_sbParallelLights.pBuffer.get() ? m_sbParallelLights.pBuffer->GetShaderResource() : NULL,
							m_sbPointLights.pBuffer.get() ? m_sbPointLights.pBuffer->GetShaderResource() : NULL,
							m_sbSpotLights.pBuffer.get() ? m_sbSpotLights.pBuffer->GetShaderResource() : NULL,
							params.pPositionMap.p,
							params.pNormalMap.p,
							params.pDiffuseMap.p,
							params.pSpecularMap.p };
							m_pDeviceContext->PSSetShaderResources(0, ppSRV.size(), ppSRV.data());

							m_pDeviceContext->PSSetSamplers(0, 1, &m_samTexPS.pSamplerState.p);
						}
						break;

						case DeferredShadingEffect::SE_MOTION_BLUR:
						{
							// Set the deferred motion blur pixel shader and all its params
							m_pDeviceContext->PSSetShader(m_pMotionBlurPS, NULL, 0);

							setConstantBufferVariableDataWithMatrix(*params.currViewProj, "gCurrViewProj", m_cbPerFrameMotionBlurPS);
							setConstantBufferVariableDataWithMatrix(*params.prevViewProj, "gPrevViewProj", m_cbPerFrameMotionBlurPS);
							setConstantBufferVariableData(reinterpret_cast<const BYTE*>(&params.nMotionBlurSamples),
								sizeof(params.nMotionBlurSamples), "gNumSamples", m_cbPerFrameMotionBlurPS);
							setConstantBufferVariableData(reinterpret_cast<const BYTE*>(&params.eyePosW), sizeof(params.eyePosW), "gEyePosW", m_cbPerFrameMotionBlurPS);
							populateConstantBufferData(m_cbPerFrameMotionBlurPS);
							m_pDeviceContext->PSSetConstantBuffers(m_cbPerFrameMotionBlurPS.bindDesc.BindPoint, 1, &m_cbPerFrameMotionBlurPS.pBuffer.p);

							setStructuredLights(*params.parallelLights, m_nParallelLights, m_sbParallelLights);
							setStructuredLights(*params.pointLights, m_nPointLights, m_sbPointLights);
							setStructuredLights(*params.spotLights, m_nSpotLights, m_sbSpotLights);

							const std::array<ID3D11ShaderResourceView* const, 7> ppSRV =
							{ m_sbParallelLights.pBuffer.get() ? m_sbParallelLights.pBuffer->GetShaderResource() : NULL,
							m_sbPointLights.pBuffer.get() ? m_sbPointLights.pBuffer->GetShaderResource() : NULL,
							m_sbSpotLights.pBuffer.get() ? m_sbSpotLights.pBuffer->GetShaderResource() : NULL,
							params.pPositionMap.p,
							params.pNormalMap.p,
							params.pDiffuseMap.p,
							params.pSpecularMap.p };
							m_pDeviceContext->PSSetShaderResources(0, ppSRV.size(), ppSRV.data());

							m_pDeviceContext->PSSetSamplers(0, 1, &m_samMotionBlurPS.pSamplerState.p);
						}
						break;

						case DeferredShadingEffect::SE_SHADOW_MAPPING:
						{
							// Set the deferred simple shadow mapping pixel shader and all its params
							m_pDeviceContext->PSSetShader(m_pSimpleShadowMappingPS, NULL, 0);

							setConstantBufferVariableData(reinterpret_cast<const BYTE*>(&params.eyePosW), sizeof(params.eyePosW), "gEyePosW", m_cbPerFrameShadowMappingPS);
							populateConstantBufferData(m_cbPerFrameShadowMappingPS);
							m_pDeviceContext->PSSetConstantBuffers(m_cbPerFrameShadowMappingPS.bindDesc.BindPoint, 1, &m_cbPerFrameShadowMappingPS.pBuffer.p);

							setStructuredLights(*params.parallelLights, m_nParallelLights, m_sbParallelLights);
							setStructuredLights(*params.pointLights, m_nPointLights, m_sbPointLights);
							setStructuredLights(*params.spotLights, m_nSpotLights, m_sbSpotLights);
							setStructuredMatrices(*params.spotLightsShadowTransforms, params.spotLightsShadowTransforms->size(),
								m_sbSpotLightsShadowTransforms);

							const std::array<ID3D11ShaderResourceView* const, 9> ppSRV =
							{ m_sbParallelLights.pBuffer.get() ? m_sbParallelLights.pBuffer->GetShaderResource() : NULL,
							m_sbPointLights.pBuffer.get() ? m_sbPointLights.pBuffer->GetShaderResource() : NULL,
							m_sbSpotLights.pBuffer.get() ? m_sbSpotLights.pBuffer->GetShaderResource() : NULL,
							params.pPositionMap.p,
							params.pNormalMap.p,
							params.pDiffuseMap.p,
							params.pSpecularMap.p,
							params.pShadowMap.p,
							m_sbSpotLightsShadowTransforms.pBuffer->GetShaderResource() };
							m_pDeviceContext->PSSetShaderResources(0, ppSRV.size(), ppSRV.data());

							const std::array<ID3D11SamplerState* const, 2> ppSamplers = { m_samStandardShading.pSamplerState.p, m_samShadowMappingPS.pSamplerState.p };
							m_pDeviceContext->PSSetSamplers(0, ppSamplers.size(), ppSamplers.data());
						}
						break;

					}
				}
				break;

				case ET_TILED:
				{
					// @TODO: Update with the implementations for all the other effects.
					switch (m_eShadingEffect)
					{
						case DeferredShadingEffect::SE_STANDARD:
						case DeferredShadingEffect::SE_MOTION_BLUR:
						{
							// Set the tiled deferred texture mapping compute shader and all its params.
							m_pDeviceContext->CSSetShader(m_computeShaders["TiledDeferredTexCS"], NULL, 0);

							setConstantBufferVariableTypedDatum<DXVector3>(params.eyePosW, "gEyePosW", m_constantBuffers["TiledDeferredTexCS"]);
							setConstantBufferVariableTypedDatum<UINT>(m_toggleStates["ShowLightsPerTile"]->getState() ? 1 : 0, 
																	"gShowLightsPerTile", m_constantBuffers["TiledDeferredTexCS"]);
							populateConstantBufferData(m_constantBuffers["TiledDeferredTexCS"]);
							std::array<ID3D11Buffer*, 1> ppConstBuf = { m_constantBuffers["TiledDeferredTexCS"].pBuffer.p };
							m_pDeviceContext->CSSetConstantBuffers(m_constantBuffers["TiledDeferredTexCS"].bindDesc.BindPoint,
								ppConstBuf.size(), ppConstBuf.data());

							std::array<ID3D11UnorderedAccessView*, 1> ppUAV = { m_pTiledOutput->GetUnorderedAccess() };
							std::array<UINT, 1> pInitCounts = { 0 };
							m_pDeviceContext->CSSetUnorderedAccessViews(0, ppUAV.size(), ppUAV.data(), pInitCounts.data());

							setStructuredLights(*params.parallelLights, m_nParallelLights, m_sbParallelLights);
							setStructuredLights(*params.pointLights, m_nPointLights, m_sbPointLights);
							setStructuredLights(*params.spotLights, m_nSpotLights, m_sbSpotLights);

							std::array<ID3D11ShaderResourceView*, 7> ppSRV =
							{ m_sbParallelLights.pBuffer.get() ? m_sbParallelLights.pBuffer->GetShaderResource() : NULL,
							m_sbPointLights.pBuffer.get() ? m_sbPointLights.pBuffer->GetShaderResource() : NULL,
							m_sbSpotLights.pBuffer.get() ? m_sbSpotLights.pBuffer->GetShaderResource() : NULL,
							params.pPositionMap.p,
							params.pNormalMap.p,
							params.pDiffuseMap.p,
							params.pSpecularMap.p };
							m_pDeviceContext->CSSetShaderResources(0, ppSRV.size(), ppSRV.data());

							// Dispatch enough thread groups to cover the entire tiled output buffer.
							const UINT dispatchWidth = (static_cast<UINT>(m_pTiledOutput->GetWidth()) + m_nTileSize - 1) / m_nTileSize;
							const UINT dispatchHeight = (static_cast<UINT>(m_pTiledOutput->GetHeight()) + m_nTileSize - 1) / m_nTileSize;
							m_pDeviceContext->Dispatch(dispatchWidth, dispatchHeight, 1);

							// Unbind the compute shader and its params.
							m_pDeviceContext->CSSetShader(NULL, NULL, 0);

							std::fill(ppConstBuf.begin(), ppConstBuf.end(), reinterpret_cast<ID3D11Buffer*>(NULL));
							//for (std::size_t i = 0; i < ppConstBuf.size(); ++i)	ppConstBuf[i] = NULL;
							m_pDeviceContext->CSSetConstantBuffers(m_constantBuffers["TiledDeferredTexCS"].bindDesc.BindPoint,
								ppConstBuf.size(), ppConstBuf.data());

							std::fill(ppUAV.begin(), ppUAV.end(), reinterpret_cast<ID3D11UnorderedAccessView*>(NULL));
							//for (std::size_t i = 0; i < ppUAV.size(); ++i)	ppUAV[i] = NULL;
							m_pDeviceContext->CSSetUnorderedAccessViews(0, ppUAV.size(), ppUAV.data(), pInitCounts.data());

							std::fill(ppSRV.begin(), ppSRV.end(), reinterpret_cast<ID3D11ShaderResourceView*>(NULL));
							//for (std::size_t i = 0; i < ppSRV.size(); ++i)	ppSRV[i] = NULL;
							m_pDeviceContext->CSSetShaderResources(0, ppSRV.size(), ppSRV.data());							
						}
						break;

						case SE_SHADOW_MAPPING:
						{
							// Perform simple shadow mapping if the tap size is 0, else use it in performing PCF based shadow mapping.
							if (m_nTapSize == 0)
							{
								// Set the tiled deferred simple shadow mapping compute shader and all its params.
								m_pDeviceContext->CSSetShader(m_computeShaders["TiledDeferredShadowMapSimpleCS"], NULL, 0);

								setConstantBufferVariableTypedDatum<DXVector3>(params.eyePosW, "gEyePosW",
									m_constantBuffers["TiledDeferredShadowMapSimpleCS"]);
								populateConstantBufferData(m_constantBuffers["TiledDeferredShadowMapSimpleCS"]);
								std::array<ID3D11Buffer*, 1> ppConstBuf = { m_constantBuffers["TiledDeferredShadowMapSimpleCS"].pBuffer.p };
								m_pDeviceContext->CSSetConstantBuffers(m_constantBuffers["TiledDeferredShadowMapSimpleCS"].bindDesc.BindPoint,
									ppConstBuf.size(), ppConstBuf.data());

								std::array<ID3D11UnorderedAccessView*, 1> ppUAV = { m_pTiledOutput->GetUnorderedAccess() };
								std::array<UINT, 1> pInitCounts = { 0 };
								m_pDeviceContext->CSSetUnorderedAccessViews(0, ppUAV.size(), ppUAV.data(), pInitCounts.data());

								setStructuredLights(*params.parallelLights, m_nParallelLights, m_sbParallelLights);
								setStructuredLights(*params.pointLights, m_nPointLights, m_sbPointLights);
								setStructuredLights(*params.spotLights, m_nSpotLights, m_sbSpotLights);
								setStructuredMatrices(*params.spotLightsShadowTransforms, params.spotLightsShadowTransforms->size(),
									m_sbSpotLightsShadowTransforms);

								std::array<ID3D11ShaderResourceView*, 9> ppSRV =
								{ m_sbParallelLights.pBuffer.get() ? m_sbParallelLights.pBuffer->GetShaderResource() : NULL,
								m_sbPointLights.pBuffer.get() ? m_sbPointLights.pBuffer->GetShaderResource() : NULL,
								m_sbSpotLights.pBuffer.get() ? m_sbSpotLights.pBuffer->GetShaderResource() : NULL,
								params.pPositionMap.p,
								params.pNormalMap.p,
								params.pDiffuseMap.p,
								params.pSpecularMap.p,
								params.pShadowMap.p,
								m_sbSpotLightsShadowTransforms.pBuffer->GetShaderResource() };
								m_pDeviceContext->CSSetShaderResources(0, ppSRV.size(), ppSRV.data());

								std::array<ID3D11SamplerState*, 1> ppSam = { m_samShadowMappingPS.pSamplerState.p };
								m_pDeviceContext->CSSetSamplers(0, ppSam.size(), ppSam.data());

								// Dispatch enough thread groups to cover the entire tiled output buffer.
								const UINT dispatchWidth = (static_cast<UINT>(m_pTiledOutput->GetWidth()) + m_nTileSize - 1) / m_nTileSize;
								const UINT dispatchHeight = (static_cast<UINT>(m_pTiledOutput->GetHeight()) + m_nTileSize - 1) / m_nTileSize;
								m_pDeviceContext->Dispatch(dispatchWidth, dispatchHeight, 1);

								// Unbind the compute shader and its params.
								m_pDeviceContext->CSSetShader(NULL, NULL, 0);

								std::fill(ppConstBuf.begin(), ppConstBuf.end(), reinterpret_cast<ID3D11Buffer*>(NULL));
								//for (std::size_t i = 0; i < ppConstBuf.size(); ++i)	ppConstBuf[i] = NULL;
								m_pDeviceContext->CSSetConstantBuffers(m_constantBuffers["TiledDeferredShadowMapSimpleCS"].bindDesc.BindPoint,
									ppConstBuf.size(), ppConstBuf.data());

								std::fill(ppUAV.begin(), ppUAV.end(), reinterpret_cast<ID3D11UnorderedAccessView*>(NULL));
								//for (std::size_t i = 0; i < ppUAV.size(); ++i)	ppUAV[i] = NULL;
								m_pDeviceContext->CSSetUnorderedAccessViews(0, ppUAV.size(), ppUAV.data(), pInitCounts.data());

								std::fill(ppSRV.begin(), ppSRV.end(), reinterpret_cast<ID3D11ShaderResourceView*>(NULL));
								//for (std::size_t i = 0; i < ppSRV.size(); ++i)	ppSRV[i] = NULL;
								m_pDeviceContext->CSSetShaderResources(0, ppSRV.size(), ppSRV.data());

								std::fill(ppSam.begin(), ppSam.end(), reinterpret_cast<ID3D11SamplerState*>(NULL));
								m_pDeviceContext->CSSetSamplers(0, ppSam.size(), ppSam.data());
							}
							else
							{
								// Set the tiled deferred simple shadow mapping compute shader and all its params.
								m_pDeviceContext->CSSetShader(m_computeShaders["TiledDeferredShadowMapPCFCS"], NULL, 0);

								setConstantBufferVariableTypedDatum<DXVector3>(params.eyePosW, "gEyePosW",
									m_constantBuffers["TiledDeferredShadowMapPCFCS"]);
								setConstantBufferVariableTypedDatum<UINT>(m_nTapSize, "gTapSize", m_constantBuffers["TiledDeferredShadowMapPCFCS"]);
								setConstantBufferVariableTypedDatum<DXVector2>(params.shadowMapDims, "gShadowMapSize",
									m_constantBuffers["TiledDeferredShadowMapPCFCS"]);
								populateConstantBufferData(m_constantBuffers["TiledDeferredShadowMapPCFCS"]);
								std::array<ID3D11Buffer*, 1> ppConstBuf = { m_constantBuffers["TiledDeferredShadowMapPCFCS"].pBuffer.p };
								m_pDeviceContext->CSSetConstantBuffers(m_constantBuffers["TiledDeferredShadowMapPCFCS"].bindDesc.BindPoint,
									ppConstBuf.size(), ppConstBuf.data());

								std::array<ID3D11UnorderedAccessView*, 1> ppUAV = { m_pTiledOutput->GetUnorderedAccess() };
								std::array<UINT, 1> pInitCounts = { 0 };
								m_pDeviceContext->CSSetUnorderedAccessViews(0, ppUAV.size(), ppUAV.data(), pInitCounts.data());

								setStructuredLights(*params.parallelLights, m_nParallelLights, m_sbParallelLights);
								setStructuredLights(*params.pointLights, m_nPointLights, m_sbPointLights);
								setStructuredLights(*params.spotLights, m_nSpotLights, m_sbSpotLights);
								setStructuredMatrices(*params.spotLightsShadowTransforms, params.spotLightsShadowTransforms->size(),
									m_sbSpotLightsShadowTransforms);

								std::array<ID3D11ShaderResourceView*, 9> ppSRV =
								{ m_sbParallelLights.pBuffer.get() ? m_sbParallelLights.pBuffer->GetShaderResource() : NULL,
								m_sbPointLights.pBuffer.get() ? m_sbPointLights.pBuffer->GetShaderResource() : NULL,
								m_sbSpotLights.pBuffer.get() ? m_sbSpotLights.pBuffer->GetShaderResource() : NULL,
								params.pPositionMap.p,
								params.pNormalMap.p,
								params.pDiffuseMap.p,
								params.pSpecularMap.p,
								params.pShadowMap.p,
								m_sbSpotLightsShadowTransforms.pBuffer->GetShaderResource() };
								m_pDeviceContext->CSSetShaderResources(0, ppSRV.size(), ppSRV.data());

								std::array<ID3D11SamplerState*, 1> ppSam = { m_samplerStates["TriLinearComparison"].pSamplerState.p };
								m_pDeviceContext->CSSetSamplers(0, ppSam.size(), ppSam.data());

								// Dispatch enough thread groups to cover the entire tiled output buffer.
								const UINT dispatchWidth = (static_cast<UINT>(m_pTiledOutput->GetWidth()) + m_nTileSize - 1) / m_nTileSize;
								const UINT dispatchHeight = (static_cast<UINT>(m_pTiledOutput->GetHeight()) + m_nTileSize - 1) / m_nTileSize;
								m_pDeviceContext->Dispatch(dispatchWidth, dispatchHeight, 1);

								// Unbind the compute shader and its params.
								m_pDeviceContext->CSSetShader(NULL, NULL, 0);

								std::fill(ppConstBuf.begin(), ppConstBuf.end(), reinterpret_cast<ID3D11Buffer*>(NULL));
								//for (std::size_t i = 0; i < ppConstBuf.size(); ++i)	ppConstBuf[i] = NULL;
								m_pDeviceContext->CSSetConstantBuffers(m_constantBuffers["TiledDeferredShadowMapPCFCS"].bindDesc.BindPoint,
									ppConstBuf.size(), ppConstBuf.data());

								std::fill(ppUAV.begin(), ppUAV.end(), reinterpret_cast<ID3D11UnorderedAccessView*>(NULL));
								//for (std::size_t i = 0; i < ppUAV.size(); ++i)	ppUAV[i] = NULL;
								m_pDeviceContext->CSSetUnorderedAccessViews(0, ppUAV.size(), ppUAV.data(), pInitCounts.data());

								std::fill(ppSRV.begin(), ppSRV.end(), reinterpret_cast<ID3D11ShaderResourceView*>(NULL));
								//for (std::size_t i = 0; i < ppSRV.size(); ++i)	ppSRV[i] = NULL;
								m_pDeviceContext->CSSetShaderResources(0, ppSRV.size(), ppSRV.data());

								std::fill(ppSam.begin(), ppSam.end(), reinterpret_cast<ID3D11SamplerState*>(NULL));
								m_pDeviceContext->CSSetSamplers(0, ppSam.size(), ppSam.data());
							}
						}
						break;

					}

					if (m_eShadingEffect == DeferredShadingEffect::SE_MOTION_BLUR)
					{
						// Set the DebugTextureEffect's VS and motion blur PS.
						// @TODO: Clean this up.
						m_pDeviceContext->VSSetShader(m_vertexShaders["DebugTextureVS"], NULL, 0);

						setConstantBufferVariableDataWithMatrix(DXMatrix::Identity(), "gWVP", m_constantBuffers["DebugTextureVS"]);
						setConstantBufferVariableDataWithMatrix(DXMatrix::Identity(), "gTexMtx", m_constantBuffers["DebugTextureVS"]);
						populateConstantBufferData(m_constantBuffers["DebugTextureVS"]);
						std::array<ID3D11Buffer*, 1> ppConstBuf = { m_constantBuffers["DebugTextureVS"].pBuffer.p };
						m_pDeviceContext->VSSetConstantBuffers(m_constantBuffers["DebugTextureVS"].bindDesc.BindPoint, ppConstBuf.size(), ppConstBuf.data());

						m_pDeviceContext->PSSetShader(m_pixelShaders["DeferredMotionBlurFinalPS"], NULL, 0);

						setConstantBufferVariableDataWithMatrix(*params.currViewProj, "gCurrViewProj", m_constantBuffers["DeferredMotionBlurFinalPS"]);
						setConstantBufferVariableDataWithMatrix(*params.prevViewProj, "gPrevViewProj", m_constantBuffers["DeferredMotionBlurFinalPS"]);
						setConstantBufferVariableTypedDatum<float>(static_cast<float>(params.nMotionBlurSamples), "gNumSamples", m_constantBuffers["DeferredMotionBlurFinalPS"]);
						populateConstantBufferData(m_constantBuffers["DeferredMotionBlurFinalPS"]);
						ppConstBuf[0] = m_constantBuffers["DeferredMotionBlurFinalPS"].pBuffer.p;
						m_pDeviceContext->PSSetConstantBuffers(m_constantBuffers["DeferredMotionBlurFinalPS"].bindDesc.BindPoint, ppConstBuf.size(), ppConstBuf.data());

						std::array<ID3D11ShaderResourceView*, 2> ppSRV = { params.pPositionMap.p, m_pTiledOutput->GetShaderResource() };
						m_pDeviceContext->PSSetShaderResources(0, ppSRV.size(), ppSRV.data());

						std::array<ID3D11SamplerState*, 1> ppSam = { m_samMotionBlurPS.pSamplerState.p };
						m_pDeviceContext->PSSetSamplers(0, ppSam.size(), ppSam.data());
					}
					else
					{
						// Set the DebugTextureEffect's params and apply it to render the contents of the tiled output buffer.
						m_pDebugTextureEffect->setWVP(DXMatrix::Identity());	// Identity transform since we're rendering on a full screen quad.
						m_pDebugTextureEffect->setTexMtx(DXMatrix::Identity());
						m_pDebugTextureEffect->setTextureIndex(DebugTextureEffect::DC_ALL);
						m_pDebugTextureEffect->setDebugTexture(m_pTiledOutput->GetShaderResource());
						m_pDebugTextureEffect->apply();
					}

					// @TODO: add implementation here
				}
				break;
			}			
		}
		break;

		// @TODO: add implementation here
	}				
}

// Unbinds all the shaders, resources and samplers that were bound to the pipeline by this effect.
void DeferredShadingEffect::cleanup()
{
	// @TODO: revise to clear based on the currently selected technique
	switch (m_eCurrTech)
	{
		case ET_CLASSIC:
		{
			// Unbind the shaders
			m_pDeviceContext->VSSetShader(0, 0, 0);
			m_pDeviceContext->PSSetShader(0, 0, 0);

			// Unbind the pixel shader resources
			const std::array<ID3D11ShaderResourceView* const, 9> ppSRV = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
			m_pDeviceContext->PSSetShaderResources(m_sbParallelLights.bindDesc.BindPoint, ppSRV.size(), ppSRV.data());

			// Unbind the pixel shader samplers
			ID3D11SamplerState* const pSam = NULL;
			m_pDeviceContext->PSSetSamplers(m_samGBufferPS.bindDesc.BindPoint, 1, &pSam);

			// Unbind the deferred texture mapping pixel shader's samplers iff the bind point is different
			// from that of the GBuffer generation pixel shader's
			if (m_samGBufferPS.bindDesc.BindPoint != m_samTexPS.bindDesc.BindPoint)
			{
				ID3D11SamplerState* const pSam = NULL;
				m_pDeviceContext->PSSetSamplers(m_samTexPS.bindDesc.BindPoint, 1, &pSam);
			}
		}
		break;

		case ET_TILED:
		{
			// @TODO: Clean this up.
			if (m_eShadingEffect == DeferredShadingEffect::SE_MOTION_BLUR)
			{
				// Set the DebugTextureEffect's VS and motion blur PS.
				// @TODO: Clean this up.
				m_pDeviceContext->VSSetShader(NULL, NULL, 0);

				std::array<ID3D11Buffer*, 1> ppConstBuf = { NULL };
				m_pDeviceContext->VSSetConstantBuffers(m_constantBuffers["DebugTextureVS"].bindDesc.BindPoint, ppConstBuf.size(), ppConstBuf.data());

				m_pDeviceContext->PSSetShader(NULL, NULL, 0);

				m_pDeviceContext->PSSetConstantBuffers(m_constantBuffers["DeferredMotionBlurFinalPS"].bindDesc.BindPoint, ppConstBuf.size(), ppConstBuf.data());

				std::array<ID3D11ShaderResourceView*, 2> ppSRV = { NULL, NULL };
				m_pDeviceContext->PSSetShaderResources(0, ppSRV.size(), ppSRV.data());

				std::array<ID3D11SamplerState*, 1> ppSam = { NULL };
				m_pDeviceContext->PSSetSamplers(0, ppSam.size(), ppSam.data());
			}
			else
			{
				// Apply the DebugTextureEffect's cleanup.
				m_pDebugTextureEffect->cleanup();
			}			
		}
		break;
	}	
}

// Toggles the states of the rendering techniques based on keyboard input
void DeferredShadingEffect::toggleStates()
{
	// Variables to manage key presses
	// NOTE: v. v. important that the booleans be static as they will otherwise
	//		 be redefined every time this function is called which renders the 
	//		 key polling useless.
	static bool bKeyPressed(false), bKeyPressed2(false), bKeyPressed3(false);

	// Set the current GBuffer effect based on keyboard input
	if ((GetAsyncKeyState('1')) && !bKeyPressed)
	{
		m_eGBufferEffect = GE_STANDARD;
		bKeyPressed = true;
	}
	else if ((GetAsyncKeyState('2')) && !bKeyPressed)
	{
		m_eGBufferEffect = GE_NORMAL_MAPPING;
		bKeyPressed = true;
	}
	else
		bKeyPressed = false;

	// Set the current shading effect based on keyboard input
	if ((GetAsyncKeyState('3')) && !bKeyPressed2)
	{
		m_eShadingEffect = SE_STANDARD;
		bKeyPressed2 = true;
	}
	else if ((GetAsyncKeyState('4')) && !bKeyPressed2)
	{
		m_eShadingEffect = SE_MOTION_BLUR;
		bKeyPressed2 = true;
	}
	else if ((GetAsyncKeyState('5')) && !bKeyPressed2)
	{
		m_eShadingEffect = SE_SHADOW_MAPPING;
		bKeyPressed2 = true;
	}
	else
		bKeyPressed2 = false;

	// Toggle the show lights count per tile iff the current technique is tiled deferred shading.
	if (m_eCurrTech == ET_TILED)
		m_toggleStates["ShowLightsPerTile"]->toggleState();

	// Increment the PCF tap size based on keyboard input iff shadow mapping is the current shading effect.
	if (m_eShadingEffect == SE_SHADOW_MAPPING)
	{
		if ((GetAsyncKeyState('P')) && !bKeyPressed3)
		{
			m_nTapSize = ++m_nTapSize % (m_nMaxTapSize + 1);	// mod tap size to ensure that the tap size cycles back around.
			bKeyPressed3 = true;
			//Sleep(75);	// Introduce a slight delay so that the tap size counter doesn't increment rapidly.
		}
		else if (!(GetAsyncKeyState('P')))
			bKeyPressed3 = false;
	}
	
	// @TODO: add implementation here
}

// Recreates the internal GBuffer with the given width and height.
// Initialization function which creates all the shaders and initializes
// all shader constant buffers, resources and sampler states.
void DeferredShadingEffect::init()
{
	DXEffect::init();

	// Initialize the DebugTextureEffect.
	m_pDebugTextureEffect->init();

	// Initialize the lights per tile toggle state.
	m_toggleStates["ShowLightsPerTile"].reset(new ToggleState(L"Show lights per tile", 'L'));
}

// Variant of the init function which also specifies the no. of parallel, point and spot lights in the scene.
void DeferredShadingEffect::init(const UINT nParallelLights, const UINT nPointLights, const UINT nSpotLights)
{
	// Set the no. of lights of each type.
	m_nParallelLights = nParallelLights;
	m_nPointLights = nPointLights;
	m_nSpotLights = nSpotLights;

	// Invoke the standard init function.
	init();
}

// Returns a string which contains effect info
std::wstring DeferredShadingEffect::getEffectInfo()
{
#if 0
	return /*m_pTextureMappingToggler->dispState() + L"\n" + */
		m_pNormalMappingToggler->dispState() + L"\n" +
		m_pMotionBlurToggler->dispState();
#endif // 0

	// @TODO: add implementation here

	return L"";	
}

// Creates all the shaders required for the effect, basically calls to create*Shader. (pure virtual, must be implemented by derived class)
void DeferredShadingEffect::createShaders()
{
	// Create the vertex shader
	wpath shaderPath(constructShaderPath(m_shaderRoot, wpath(L"deferredShadingVS.cso")));
	createVertexShader(shaderPath, m_pVSByteCode, m_pVS);

	// Create the GBuffer generation pass pixel shader
	shaderPath = constructShaderPath(m_shaderRoot, wpath(L"deferredShadingPS1.cso"));
	createPixelShader(shaderPath, m_pGBufferPSByteCode, m_pGBufferPS);

	// Create the normal mapping GBuffer generation pass pixel shader and init its corresponding toggle state
	shaderPath = constructShaderPath(m_shaderRoot, wpath(L"deferredShadingPS2.cso"));
	createPixelShader(shaderPath, m_pGBufferPS2ByteCode, m_pGBufferPS2);

	// Create the deferred texture mapping pixel shader and init its corresponding toggle state
	shaderPath = constructShaderPath(m_shaderRoot, wpath(L"deferredTexPS.cso"));
	createPixelShader(shaderPath, m_pTexPSByteCode, m_pTexPS);

	// Create the deferred motion blur pixel shader and its corresponding toggle state
	shaderPath = constructShaderPath(m_shaderRoot, wpath(L"deferredMotionBlurPS.cso"));
	createPixelShader(shaderPath, m_pMotionBlurPSByteCode, m_pMotionBlurPS);

	// Create the deferred shadow mapping pixel shader
	shaderPath = constructShaderPath(m_shaderRoot, wpath(L"deferredShadowMapSimplePS.cso"));
	createPixelShader(shaderPath, m_pSimpleShadowMappingPSByteCode, m_pSimpleShadowMappingPS);

	// Created the tiled deferred shading texture mapping compute shader.
	shaderPath = constructShaderPath(m_shaderRoot, wpath(L"tiledDeferredTexCS.cso"));
	createComputeShader(shaderPath, m_shaderBlobs["TiledDeferredTexCS"], m_computeShaders["TiledDeferredTexCS"]);

	// Created the tiled deferred shading motion blur compute shader.
	shaderPath = constructShaderPath(m_shaderRoot, wpath(L"tiledDeferredMotionBlurCS.cso"));
	createComputeShader(shaderPath, m_shaderBlobs["TiledDeferredMotionBlurCS"], m_computeShaders["TiledDeferredMotionBlurCS"]);

	// Create the tiled deferred simple shadow mapping compute shader.
	shaderPath = constructShaderPath(m_shaderRoot, wpath(L"tiledDeferredShadowMapSimpleCS.cso"));
	createComputeShader(shaderPath, m_shaderBlobs["TiledDeferredShadowMapSimpleCS"], m_computeShaders["TiledDeferredShadowMapSimpleCS"]);

	// Create the tiled deferred PCF shadow mapping compute shader.
	shaderPath = constructShaderPath(m_shaderRoot, wpath(L"tiledDeferredShadowMapPCFCS.cso"));
	createComputeShader(shaderPath, m_shaderBlobs["TiledDeferredShadowMapPCFCS"], m_computeShaders["TiledDeferredShadowMapPCFCS"]);

	// Create the deferred motion blur final pixel shader.
	shaderPath = constructShaderPath(m_shaderRoot, wpath(L"deferredMotionBlurFinalPS.cso"));
	createPixelShader(shaderPath, m_shaderBlobs["DeferredMotionBlurFinalPS"], m_pixelShaders["DeferredMotionBlurFinalPS"]);

	// Create the debug texture effect vertex shader.
	// @TODO: Clean this up!
	shaderPath = constructShaderPath(m_shaderRoot, wpath(L"debugTextureVS.cso"));
	createVertexShader(shaderPath, m_shaderBlobs["DebugTextureVS"], m_vertexShaders["DebugTextureVS"]);

	// @TODO: add implementation here
}

// Initializes the input layout for the effect. (pure virtual, must be implemented by derived class)
void DeferredShadingEffect::initInputLayout()
{
	// Init the input layout from the vertex shader's byte code
	createInputLayoutFromShaderInfo(m_pVSByteCode, m_pInputLayout);
}

// Creates and initializes the constant buffers of all shaders. (pure virtual, must be implemented by derived class)
void DeferredShadingEffect::initShaderConstantBuffers()
{
	// Create the vertex shader's cbPerObject constant buffer.
	createConstantBuffer(m_pVSByteCode, "cbPerObject", m_VSCBPerObject);

	// Create the deferred texture mapping pixel shader's cbPerFrame constant buffer.
	createConstantBuffer(m_pTexPSByteCode, "cbPerFrame", m_cbPerFrameTexPS);

	// Create the deferred motion blur pixel shader's cbPerFrame constant buffer
	createConstantBuffer(m_pMotionBlurPSByteCode, "cbPerFrame", m_cbPerFrameMotionBlurPS);

	// Create the deferred texture mapping pixel shader's light structured buffers.
	createStructuredBuffer<SLight>(m_pTexPSByteCode, "gParallelLights", m_nParallelLights, D3D11_BIND_SHADER_RESOURCE, m_sbParallelLights);
	createStructuredBuffer<SLight>(m_pTexPSByteCode, "gPointLights", m_nPointLights, D3D11_BIND_SHADER_RESOURCE, m_sbPointLights);
	createStructuredBuffer<SLight>(m_pTexPSByteCode, "gSpotLights", m_nSpotLights, D3D11_BIND_SHADER_RESOURCE, m_sbSpotLights);

	// Create the deferred shadow mapping pixel shader's cbPerFrame constant buffer
	createConstantBuffer(m_pSimpleShadowMappingPSByteCode, "cbPerFrame", m_cbPerFrameShadowMappingPS);

	// Create the deferred shadow mapping pixel shader's spot light shadow transforms structured buffers.
	createStructuredBuffer<DXMatrix>(m_pSimpleShadowMappingPSByteCode, "gSpotLightsShadowTransforms", m_nSpotLights, 
									D3D11_BIND_SHADER_RESOURCE, m_sbSpotLightsShadowTransforms);

	// Create the tiled deferred texture mapping compute shader's cbPerFrame constant buffer
	createConstantBuffer(m_shaderBlobs["TiledDeferredTexCS"], "cbPerFrame", m_constantBuffers["TiledDeferredTexCS"]);

	// Create the tiled deferred motion blur compute shader's cbPerFrame constant buffer.
	createConstantBuffer(m_shaderBlobs["TiledDeferredMotionBlurCS"], "cbPerFrame", m_constantBuffers["TiledDeferredMotionBlurCS"]);

	// Create the tiled deferred simple shadow mapping compute shader's cbPerFrame constant buffer.
	createConstantBuffer(m_shaderBlobs["TiledDeferredShadowMapSimpleCS"], "cbPerFrame", m_constantBuffers["TiledDeferredShadowMapSimpleCS"]);

	// Create the tiled deferred PCF based shadown mapping compute shader's cbPerFrame constant buffer.
	createConstantBuffer(m_shaderBlobs["TiledDeferredShadowMapPCFCS"], "cbPerFrame", m_constantBuffers["TiledDeferredShadowMapPCFCS"]);

	// Create the deferred motion blur final pixel shader's cbPerFrame constant buffer.
	createConstantBuffer(m_shaderBlobs["DeferredMotionBlurFinalPS"], "cbPerFrame", m_constantBuffers["DeferredMotionBlurFinalPS"]);

	// Create the debug texture vertex shader's cbPerObject constant buffer.
	createConstantBuffer(m_shaderBlobs["DebugTextureVS"], "cbPerObject", m_constantBuffers["DebugTextureVS"]);

	// @TODO: add implementation here
}

// Initializes the resources of all shaders. (pure virtual, must be implemented by derived class)
void DeferredShadingEffect::initShaderResources()
{

	// @TODO: add implementation here
}

// Initializes the sampler states of all shaders. (pure virtual, must be implemented by derived class)
void DeferredShadingEffect::initShaderSamplers()
{
	// Init the GBuffer generation pixel shader's sampler state to a trilinear sampler state
	SamplerStateMaker samplerStateMaker;
	samplerStateMaker.makeTriLinear(m_pDevice, m_samGBufferPS.pSamplerState);

	// Init the normal mapping GBuffer generation pixel shader's sampler state to a trilinear sampler state
	samplerStateMaker.makeTriLinear(m_pDevice, m_samGBufferPS2.pSamplerState);

	// Init the deferred texture mapping pixel shader's sampler state to a point sampler state
	samplerStateMaker.makeTriPoint(m_pDevice, m_samTexPS.pSamplerState);

	// Init the deferred motion blur pixel shader's sampler state to a point sampler state
	samplerStateMaker.makeTriPoint(m_pDevice, m_samMotionBlurPS.pSamplerState);

	// Init the standard deferred shading sampler to state to a point sampler state (this is what will be used by most of the deferred shaders)
	samplerStateMaker.makeTriPoint(m_pDevice, m_samStandardShading.pSamplerState);

	// Init the shadow mapping sampler state to a point sampler state
	samplerStateMaker.makeTriPoint(m_pDevice, m_samShadowMappingPS.pSamplerState);

	// Add the comparison sampler state to the sampler state map.
	samplerStateMaker.makeTriLinearComparison(m_pDevice, m_samplerStates["TriLinearComparison"].pSamplerState);

	// @TODO: add implementation here
}

// Displays the current state of the deferred shading effects
void DeferredShadingEffect::dispEffectStates(const SpriteBatchPtr &pSpriteBatch, const SpriteFontPtr &pSpriteFont, 
											const UINT uiClientWidth, const UINT uiClientHeight)
{
	pSpriteBatch->Begin();

	// Display the states of the GBuffer effects
	const float lineOffset(pSpriteFont->GetLineSpacing()), 
		showTilesHeight(lineOffset),
		seHeight(showTilesHeight + lineOffset + SE_MAX_EFFECTS*lineOffset), 
		geHeight(seHeight + lineOffset + GE_MAX_EFFECTS*lineOffset);
	float xOffset(0), yOffset(uiClientHeight - geHeight);
	displayEffectState(pSpriteBatch, pSpriteFont, L"Texture Mapping: ", m_eGBufferEffect == GE_STANDARD, DXVector2(xOffset, yOffset));
	displayEffectState(pSpriteBatch, pSpriteFont, L"Normal Mapping: ", m_eGBufferEffect == GE_NORMAL_MAPPING, DXVector2(xOffset, yOffset + lineOffset));
	
	// Display the states of the shading effects
	xOffset = 0;	yOffset = uiClientHeight - seHeight;
	displayEffectState(pSpriteBatch, pSpriteFont, L"Standard shading: ", m_eShadingEffect == SE_STANDARD, DXVector2(xOffset, yOffset));
	displayEffectState(pSpriteBatch, pSpriteFont, L"Motion blur: ", m_eShadingEffect == SE_MOTION_BLUR, DXVector2(xOffset, yOffset + lineOffset));

	// Construct the display string for shadow mapping based on the current tap size.
	std::wstring strShadow(L"Shadow mapping");
	if (m_eShadingEffect == SE_SHADOW_MAPPING)
	{
		if (m_nTapSize == 0)
		{
			strShadow += L" (Simple): ";
		}
		else
		{
			const std::wstring strTapSize(std::to_wstring(m_nTapSize));
			strShadow += L" (" + strTapSize + L"x" + strTapSize + L" PCF): ";
		}
	}
	else
	{
		strShadow += L": ";
	}		
	
	displayEffectState(pSpriteBatch, pSpriteFont, strShadow.c_str(), m_eShadingEffect == SE_SHADOW_MAPPING, DXVector2(xOffset, yOffset + 2*lineOffset));

	// Display the state of the show lights per tile flag.
	xOffset = 0;	yOffset = uiClientHeight - showTilesHeight;
	displayEffectState(pSpriteBatch, pSpriteFont, L"Show lights per tile: ", m_toggleStates["ShowLightsPerTile"]->getState(), DXVector2(xOffset, yOffset));

	pSpriteBatch->End();
}

// Mutator method to resize the tiled output buffer.
void DeferredShadingEffect::resizeTiledOutput(const UINT nClientWidth, const UINT nClientHeight)
{
	// Recreate the tiled output buffer given the new client width and height.
	m_pTiledOutput.reset(new Texture2D(m_pDevice, nClientWidth, nClientHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE));
}