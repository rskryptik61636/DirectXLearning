// Source file for the ShadowNormalMappingEffect class

#include "ShadowNormalMappingEffect.h"

#include "SamplerStateMaker.h"

// Max no. of spot lights
const int ShadowNormalMappingEffect::nMaxSpotLights = 4;

// Param ctor
ShadowNormalMappingEffect::ShadowNormalMappingEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext)
: DXEffect(shaderRoot, pDevice, pDeviceContext),
m_eCurrentPass(EP_SHADOW_MAP_GENERATION),	// init the current pass to the shadow map generation pass
m_eCurrentTechnique(ET_SIMPLE)	// init the current technique to the standard method
{}

// Dtor
ShadowNormalMappingEffect::~ShadowNormalMappingEffect()
{}

// Causes the effect to be applied. (virtual, may be implemented by derived class)
void ShadowNormalMappingEffect::apply()
{
	//DXEffect::apply();	// @TODO: remove when done testing

	switch (m_eCurrentPass)
	{
	case EP_SHADOW_MAP_GENERATION:
	{
		// Set the first pass' vertex shader
		m_pDeviceContext->VSSetShader(m_pFirstPassVertexShader.p, NULL, 0);

		// Reset the pixel shader to disable what might already be set
		m_pDeviceContext->PSSetShader(NULL, NULL, 0);					

		populateConstantBufferData(m_cbPerObjectFirstPass);	// populate the first pass' vertex shader's constant buffer
		const std::vector<ID3D11Buffer*> pVSBuffers(1, m_cbPerObjectFirstPass.pBuffer.p);
		m_pDeviceContext->VSSetConstantBuffers(m_cbPerObjectFirstPass.bindDesc.BindPoint, pVSBuffers.size(), pVSBuffers.data());
	}
		break;

	case EP_SHADOW_MAP_RENDER:
	{
		// Set the second pass' vertex shader
		m_pDeviceContext->VSSetShader(m_pSecondPassVertexShader.p, NULL, 0);

		// populate the second pass' vertex shader's constant buffer
		populateConstantBufferData(m_cbPerObjectSecondPass);
		const std::vector<ID3D11Buffer*> pVSBuffers(1, m_cbPerObjectSecondPass.pBuffer.p);
		m_pDeviceContext->VSSetConstantBuffers(m_cbPerObjectSecondPass.bindDesc.BindPoint, pVSBuffers.size(), pVSBuffers.data());

		// Set the second pass' pixel shader
		switch (m_eCurrentTechnique)
		{
			case ET_SIMPLE:
			{
				m_pDeviceContext->PSSetShader(m_pSecondPassSimplePixelShader.p, NULL, 0);

				// populate the second pass' simple pixel shader's constant buffer
				populateConstantBufferData(m_cbPerFrameSecondPassSimple);
				const std::vector<ID3D11Buffer*> pPSBuffers(1, m_cbPerFrameSecondPassSimple.pBuffer.p);
				m_pDeviceContext->PSSetConstantBuffers(m_cbPerFrameSecondPassSimple.bindDesc.BindPoint, pPSBuffers.size(), pPSBuffers.data());

				// create an array of the shader resource views that need to be set in the simple pixel shader
				const std::array<ID3D11ShaderResourceView*, 4> ppResourceViews = { m_resDiffuseMap.pResourceView.p, m_resSpecularMap.pResourceView.p,
																					m_resNormalMap.pResourceView.p, m_resShadowMap.pResourceView.p };
				m_pDeviceContext->PSSetShaderResources(m_resDiffuseMap.bindDesc.BindPoint, ppResourceViews.size(), ppResourceViews.data());

				// set the sampler states in the simple pixel shader
				m_pDeviceContext->PSSetSamplers(m_iSamplerStartBindPoint, m_ppSamplers.size(), m_ppSamplers.data());
			}
			break;

			case ET_PCF_1_TAP:
			case ET_PCF_16_TAP:
			{
				m_pDeviceContext->PSSetShader(m_pSecondPassPCFPixelShader.p, NULL, 0);

				// populate the second pass' PCF pixel shader's constant buffer
				populateConstantBufferData(m_cbPerFrameSecondPassPCF);
				const std::vector<ID3D11Buffer*> pPSBuffers(1, m_cbPerFrameSecondPassPCF.pBuffer.p);
				m_pDeviceContext->PSSetConstantBuffers(m_cbPerFrameSecondPassPCF.bindDesc.BindPoint, pPSBuffers.size(), pPSBuffers.data());

				// create an array of the shader resource views that need to be set in the PCF pixel shader
				const std::array<ID3D11ShaderResourceView*, 4> ppResourceViewsPCF = { m_resDiffuseMapPCF.pResourceView.p, 
																						m_resSpecularMapPCF.pResourceView.p,
																						m_resNormalMapPCF.pResourceView.p, 
																						m_resShadowMapPCF.pResourceView.p };
				m_pDeviceContext->PSSetShaderResources(m_resDiffuseMapPCF.bindDesc.BindPoint, ppResourceViewsPCF.size(), ppResourceViewsPCF.data());

				// set the sampler states in the PCF pixel shader
				m_pDeviceContext->PSSetSamplers(m_iSamplerStartBindPointPCF, m_ppSamplersPCF.size(), m_ppSamplersPCF.data());
			}
			break;
		}
	}
		break;
	}
}

// Initialization function which creates all the shaders and initializes
// all shader constant buffers, resources and sampler states.
void ShadowNormalMappingEffect::init()
{
	DXEffect::init();
}

// Returns a string which contains effect info
std::wstring ShadowNormalMappingEffect::getEffectInfo()
{
	std::wstring strEffectInfo = L"Effect: Shadow Mapping\nTechinique: ";
	switch (m_eCurrentTechnique)
	{
	case ET_SIMPLE:
		strEffectInfo += L"Simple";
		break;

	case ET_PCF_1_TAP:
		strEffectInfo += L"PCF 1 tap";
		break;

	case ET_PCF_16_TAP:
		strEffectInfo += L"PCF 16 tap";
		break;
	}

	return strEffectInfo;
}

// Mutator func to set the current technique
void ShadowNormalMappingEffect::setCurrentTechnique(const EffectTechnique eCurrentTechnique)
{
	m_eCurrentTechnique = eCurrentTechnique;

	// Update the no. of taps in the cbPerFrame constant buffer for the PCF based technique
	switch (m_eCurrentTechnique)
	{
		case ET_PCF_1_TAP:
		{
			const float nTaps(1.0f);
			setConstantBufferVariableData(reinterpret_cast<const BYTE*>(&nTaps), sizeof(nTaps), "nTapSize", m_cbPerFrameSecondPassPCF);
		}
		break;

		case ET_PCF_16_TAP:
		{
			const float nTaps(16.0f);
			setConstantBufferVariableData(reinterpret_cast<const BYTE*>(&nTaps), sizeof(nTaps), "nTapSize", m_cbPerFrameSecondPassPCF);
		}
			break;
	}
}

// mutator funcs to set the constant buffer members

// cbPerObject first pass

// set the first pass' light world view projection matrix (world matrix * light's view projection matrix)
void ShadowNormalMappingEffect::setFirstPassLightWVP(const DXMatrix &lightWVP)
{
	setConstantBufferVariableDataWithMatrix(lightWVP, "gWVP", m_cbPerObjectFirstPass);
}

// cbPerObject second pass

// set the second pass' world view projection matrix
void ShadowNormalMappingEffect::setSecondPassWVP(const DXMatrix &wvp)
{
	setConstantBufferVariableDataWithMatrix(wvp, "gWVP", m_cbPerObjectSecondPass);
}

// set the second pass' world matrix
void ShadowNormalMappingEffect::setSecondPassWorld(const DXMatrix &world)
{
	setConstantBufferVariableDataWithMatrix(world, "gWorld", m_cbPerObjectSecondPass);
}

// set the second pass' inverse transpose of the world matrix
void ShadowNormalMappingEffect::setSecondPassWorldInvTrans(const DXMatrix &worldInvTrans)
{
	setConstantBufferVariableDataWithMatrix(worldInvTrans, "gWorldInvTrans", m_cbPerObjectSecondPass);
}

// set the second pass' light space view projection tangent matrix
void ShadowNormalMappingEffect::setSecondPassLightWorldViewProjTangentMatrix(const std::vector<DXMatrix> &lightWorldViewProjTangent)
{
	// TODO: fix this hack!
	std::vector<DXMatrix> tmp(lightWorldViewProjTangent.size());
	for (std::size_t i = 0; i < tmp.size(); ++i)
		tmp[i] = lightWorldViewProjTangent[i].Transpose();

	setConstantBufferVariableArray<DXMatrix>(/*lightWorldViewProjTangent*/tmp, nMaxSpotLights, "gLightViewProjTangent", m_cbPerObjectSecondPass);
	//setConstantBufferVariableDataWithMatrix(lightWorldViewProjTangent, "gLightViewProjTangent", m_cbPerObjectSecondPass);
}

// set the second pass' texture transformation matrix
void ShadowNormalMappingEffect::setSecondPassTexMtx(const DXMatrix &texMtx)
{
	setConstantBufferVariableDataWithMatrix(texMtx, "gTexMtx", m_cbPerObjectSecondPass);
}

// cbPerFrame

// set the spot light
void ShadowNormalMappingEffect::setSpotLights(const std::vector<Light> &spotLights)
{
	switch (m_eCurrentTechnique)
	{
	case ET_SIMPLE:
		{
			setLights(spotLights, nMaxSpotLights, "gSpotLights", "nSpotLights", m_cbPerFrameSecondPassSimple);
		}
		break;

	case ET_PCF_1_TAP:
	case ET_PCF_16_TAP:
		{
			setLights(spotLights, nMaxSpotLights, "gSpotLights", "nSpotLights", m_cbPerFrameSecondPassPCF);
		}
		break;
	}
}

// set the world space eye pos
void ShadowNormalMappingEffect::setEyePosW(const DXVector3 &eyePosW)
{
	switch (m_eCurrentTechnique)
	{
		case ET_SIMPLE:
		{
			setConstantBufferVariableData(reinterpret_cast<const BYTE*>(&eyePosW), sizeof(eyePosW), "gEyePosW", m_cbPerFrameSecondPassSimple);
		}
		break;

		case ET_PCF_1_TAP:
		case ET_PCF_16_TAP:
		{
			setConstantBufferVariableData(reinterpret_cast<const BYTE*>(&eyePosW), sizeof(eyePosW), "gEyePosW", m_cbPerFrameSecondPassPCF);
		}
		break;
	}	
}

// set the diffuse texture map
void ShadowNormalMappingEffect::setDiffuseMap(const ShaderResourceViewPtr &pDiffuseMap)
{
	switch (m_eCurrentTechnique)
	{
		case ET_SIMPLE:
		{
			m_resDiffuseMap.pResourceView = pDiffuseMap;
		}
		break;

		case ET_PCF_1_TAP:
		case ET_PCF_16_TAP:
		{
			m_resDiffuseMapPCF.pResourceView = pDiffuseMap;
		}
		break;
	}
}

// set the specular light map
void ShadowNormalMappingEffect::setSpecularMap(const ShaderResourceViewPtr &pSpecularMap)
{
	switch (m_eCurrentTechnique)
	{
		case ET_SIMPLE:
		{
			m_resSpecularMap.pResourceView = pSpecularMap;
		}
		break;

		case ET_PCF_1_TAP:
		case ET_PCF_16_TAP:
		{
			m_resSpecularMapPCF.pResourceView = pSpecularMap;
		}
		break;
	}	
}

// set the normal map
void ShadowNormalMappingEffect::setNormalMap(const ShaderResourceViewPtr &pNormalMap)
{
	switch (m_eCurrentTechnique)
	{
	case ET_SIMPLE:
	{
		m_resNormalMap.pResourceView = pNormalMap;
	}
		break;

	case ET_PCF_1_TAP:
	case ET_PCF_16_TAP:
	{
		m_resNormalMapPCF.pResourceView = pNormalMap;
	}
		break;
	}	
}

// set the shadow map
void ShadowNormalMappingEffect::setShadowMap(const ShaderResourceViewPtr &pShadowMap)
{
	switch (m_eCurrentTechnique)
	{
		case ET_SIMPLE:
		{
			m_resShadowMap.pResourceView = pShadowMap;
		}
		break;

		case ET_PCF_1_TAP:
		case ET_PCF_16_TAP:
		{
			m_resShadowMapPCF.pResourceView = pShadowMap;
		}
		break;
	}	
}

// set the shadow map dims
void ShadowNormalMappingEffect::setShadowMapSize(const UINT nWidth, const UINT nHeight)
{
	switch (m_eCurrentTechnique)
	{
		case ET_PCF_1_TAP:
		case ET_PCF_16_TAP:
		{
			const DXVector2 dims(nWidth, nHeight);
			setConstantBufferVariableData(reinterpret_cast<const BYTE*>(&dims), sizeof(dims), "nShadowMapSize", m_cbPerFrameSecondPassPCF);
		}
		break;
	}
}

// unbind the shadow map SRV from the pixel shader
void ShadowNormalMappingEffect::releaseShadowMap()
{
	ID3D11ShaderResourceView *pEmpty = { NULL };
	m_pDeviceContext->PSSetShaderResources(m_resShadowMap.bindDesc.BindPoint, 1, &pEmpty);
}

// @TODO: keep the ones that we'll need
#if 0
// set the world matrix
void setWorld(const DXMatrix &world);

// set the inverse transpose of the world matrix
void setWorldInvTrans(const DXMatrix &worldInvTrans);

// set the texture transformation matrix
void setTexMtx(const DXMatrix &texMtx);

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


// All the constant buffers across all shaders are set. (pure virtual, must be implemented by derived class)
void ShadowNormalMappingEffect::setShaderConstantBuffers() /*=0*/
{
	// Decide which set of constant buffers to set based on the current pass
	switch (m_eCurrentPass)
	{
	case EP_SHADOW_MAP_GENERATION:
		{
			populateConstantBufferData(m_cbPerObjectFirstPass);	// populate the first pass' vertex shader's constant buffer
			const std::vector<ID3D11Buffer*> pVSBuffers(1, m_cbPerObjectFirstPass.pBuffer.p);
			m_pDeviceContext->VSSetConstantBuffers(m_cbPerObjectFirstPass.bindDesc.BindPoint, pVSBuffers.size(), pVSBuffers.data());
		}
		break;

	case EP_SHADOW_MAP_RENDER:
		{
			// populate the second pass' vertex shader's constant buffer
			populateConstantBufferData(m_cbPerObjectSecondPass);
			const std::vector<ID3D11Buffer*> pVSBuffers(1, m_cbPerObjectSecondPass.pBuffer.p);
			m_pDeviceContext->VSSetConstantBuffers(m_cbPerObjectSecondPass.bindDesc.BindPoint, pVSBuffers.size(), pVSBuffers.data());

			// Decide which set of constant buffers to use based on the current technique
			switch (m_eCurrentTechnique)
			{
				case ET_SIMPLE:
				{
					// populate the second pass' simple pixel shader's constant buffer
					populateConstantBufferData(m_cbPerFrameSecondPassSimple);
					const std::vector<ID3D11Buffer*> pPSBuffers(1, m_cbPerFrameSecondPassSimple.pBuffer.p);
					m_pDeviceContext->PSSetConstantBuffers(m_cbPerFrameSecondPassSimple.bindDesc.BindPoint, pPSBuffers.size(), pPSBuffers.data());
				}
				break;

				case ET_PCF_1_TAP:
				case ET_PCF_16_TAP:
				{
					// populate the second pass' PCF pixel shader's constant buffer
					populateConstantBufferData(m_cbPerFrameSecondPassPCF);
					const std::vector<ID3D11Buffer*> pPSBuffers(1, m_cbPerFrameSecondPassPCF.pBuffer.p);
					m_pDeviceContext->PSSetConstantBuffers(m_cbPerFrameSecondPassPCF.bindDesc.BindPoint, pPSBuffers.size(), pPSBuffers.data());
				}
				break;				
			}			
		}		
		break;
	}
}

// All the shader textures(resources) are set. (pure virtual, must be implemented by derived class)
void ShadowNormalMappingEffect::setShaderResources() /*=0*/
{
	// NOTE: Considered making the resource view arrays class members so that they can be set once during initialization
	//		 and then reused but that may not be a good idea because the resource views can be changed during runtime.

	// Decide which resources to set based on the current pass (nothing to set in the shadow map generation pass)
	// as well as the current technique
	switch (m_eCurrentPass)
	{
	case EP_SHADOW_MAP_RENDER:
		{
			switch (m_eCurrentTechnique)
			{
				case ET_SIMPLE:
				{
					// create an array of the shader resource views that need to be set in the simple pixel shader
					std::vector<ID3D11ShaderResourceView*> ppResourceViews(4);
					ppResourceViews[0] = m_resDiffuseMap.pResourceView.p;
					ppResourceViews[1] = m_resSpecularMap.pResourceView.p;
					ppResourceViews[2] = m_resNormalMap.pResourceView.p;
					ppResourceViews[3] = m_resShadowMap.pResourceView.p;
					m_pDeviceContext->PSSetShaderResources(m_resDiffuseMap.bindDesc.BindPoint, ppResourceViews.size(), ppResourceViews.data());
				}
				break;

				case ET_PCF_1_TAP:
				case ET_PCF_16_TAP:
				{
					// create an array of the shader resource views that need to be set in the PCF pixel shader
					std::vector<ID3D11ShaderResourceView*> ppResourceViewsPCF(4);
					ppResourceViewsPCF[0] = m_resDiffuseMapPCF.pResourceView.p;
					ppResourceViewsPCF[1] = m_resSpecularMapPCF.pResourceView.p;
					ppResourceViewsPCF[2] = m_resNormalMapPCF.pResourceView.p;
					ppResourceViewsPCF[3] = m_resShadowMapPCF.pResourceView.p;
					m_pDeviceContext->PSSetShaderResources(m_resDiffuseMapPCF.bindDesc.BindPoint, ppResourceViewsPCF.size(), ppResourceViewsPCF.data());
				}
				break;
			}
			
		}
		break;
	}
}

// All the shader samplers are set. (pure virtual, must be implemented by derived class)
void ShadowNormalMappingEffect::setShaderSamplers() /*=0*/
{
	// Decide which samplers to set based on the current pass (nothing to set in the shadow map generation pass)
	// as well as the current technique
	switch (m_eCurrentPass)
	{
	case EP_SHADOW_MAP_RENDER:
		{
			switch (m_eCurrentTechnique)
			{
				case ET_SIMPLE:
				{
					// set the sampler states in the simple pixel shader
					m_pDeviceContext->PSSetSamplers(m_iSamplerStartBindPoint, m_ppSamplers.size(), m_ppSamplers.data());
				}
				break;

				case ET_PCF_1_TAP:
				case ET_PCF_16_TAP:
				{
					// set the sampler states in the PCF pixel shader
					m_pDeviceContext->PSSetSamplers(m_iSamplerStartBindPointPCF, m_ppSamplersPCF.size(), m_ppSamplersPCF.data());
				}
				break;						
			}			
		}
		break;
	}
}

// All the shaders are set. (pure virtual, must be implemented by derived class)
void ShadowNormalMappingEffect::setShaders() /*=0*/
{
	// Decide which shaders to set based on the current pass as well as the current technique
	switch (m_eCurrentPass)
	{
	case EP_SHADOW_MAP_GENERATION:
		{
			// Set the first pass' vertex shader
			m_pDeviceContext->VSSetShader(m_pFirstPassVertexShader.p, NULL, 0);

			// Reset the pixel shader to disable what might already be set
			m_pDeviceContext->PSSetShader(NULL, NULL, 0);
		}
		break;

	case EP_SHADOW_MAP_RENDER:
		{
			// Set the second pass' vertex shader
			m_pDeviceContext->VSSetShader(m_pSecondPassVertexShader.p, NULL, 0);

			// Set the second pass' pixel shader
			switch (m_eCurrentTechnique)
			{
				case ET_SIMPLE:
				{
					m_pDeviceContext->PSSetShader(m_pSecondPassSimplePixelShader.p, NULL, 0);
				}
				break;

				case ET_PCF_1_TAP:
				case ET_PCF_16_TAP:
				{
					m_pDeviceContext->PSSetShader(m_pSecondPassPCFPixelShader.p, NULL, 0);
				}
				break;
			}			
		}
		break;
	}
}

// Creates all the shaders required for the effect, basically calls to create*Shader. (pure virtual, must be implemented by derived class)
void ShadowNormalMappingEffect::createShaders() /*=0*/
{
	// Create the shadow map generation pass' vertex shader
	wpath vertexShaderPath(constructShaderPath(m_shaderRoot, wpath(L"buildShadowMapVS.cso")));
	createVertexShader(vertexShaderPath, m_pFirstPassVertexByteCode, m_pFirstPassVertexShader);

	// Create a dummy pixel shader for the shadow map generation pass
	// @TODO: remove when done testing
	wpath pixelShaderPath(constructShaderPath(m_shaderRoot, wpath(L"buildShadowMapPS.cso")));
	createPixelShader(pixelShaderPath, m_pFirstPassDummyPixelByteCode, m_pFirstPassDummyPixelShader);

	// Create the shadow map render pass' vertex shader
	vertexShaderPath = constructShaderPath(m_shaderRoot, wpath(L"renderShadowMapVS.cso"));
	createVertexShader(vertexShaderPath, m_pSecondPassVertexByteCode, m_pSecondPassVertexShader);

	// Create the shadow map render pass' simple pixel shader
	pixelShaderPath = constructShaderPath(m_shaderRoot, wpath(L"renderShadowMapSimplePS.cso"));
	createPixelShader(pixelShaderPath, m_pSecondPassSimplePixelByteCode, m_pSecondPassSimplePixelShader);

	// Create the shadow map render pass' PCF based pixel shader
	pixelShaderPath = constructShaderPath(m_shaderRoot, wpath(L"renderShadowMapPCFPS.cso"));
	createPixelShader(pixelShaderPath, m_pSecondPassPCFPixelByteCode, m_pSecondPassPCFPixelShader);

	// @TODO: Create shaders for the shadow map render pass
}

// Initializes the input layout for the effect. (pure virtual, must be implemented by derived class)
void ShadowNormalMappingEffect::initInputLayout() /*= 0*/
{
	// Determine input layout from the shadow map render pass' vertex shader
	createInputLayoutFromShaderInfo(m_pSecondPassVertexByteCode, m_pInputLayout);
}

// Creates and initializes the constant buffers of all shaders. (pure virtual, must be implemented by derived class)
void ShadowNormalMappingEffect::initShaderConstantBuffers() /*=0*/
{
	// Create the cbPerObject constant buffer in the shadow map generation pass' vertex shader
	std::string bufName("cbPerObject");
	createConstantBuffer(m_pFirstPassVertexByteCode, bufName, m_cbPerObjectFirstPass);

	// Create the cbPerObject constant buffer in the shadow map render pass' vertex shader
	createConstantBuffer(m_pSecondPassVertexByteCode, bufName, m_cbPerObjectSecondPass);

	// Create the cbPerFrame constant buffer in the shadow map render pass' simple pixel shader
	bufName = "cbPerFrame";
	createConstantBuffer(m_pSecondPassSimplePixelByteCode, bufName, m_cbPerFrameSecondPassSimple);

	// Create the cbPerFrame constant buffer in the shadow map render pass' PCF based pixel shader
	createConstantBuffer(m_pSecondPassPCFPixelByteCode, bufName, m_cbPerFrameSecondPassPCF);

	// @TODO: Create constant buffers for the shadow map render pass' shaders
}

// Initializes the resources of all shaders. (pure virtual, must be implemented by derived class)
void ShadowNormalMappingEffect::initShaderResources() /*=0*/
{
	// Initialize shader resources as required by the shadow map render pass' shaders
	
	// Get the binding info of all the pixel shader textures for the simple technique
	ShaderReflectionPtr pReflector;
	HR(D3DReflect(m_pSecondPassSimplePixelByteCode->GetBufferPointer(), m_pSecondPassSimplePixelByteCode->GetBufferSize(),
		IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&pReflector.p)));

	m_resDiffuseMap.strName = "gDiffuseMap";
	HR(pReflector->GetResourceBindingDescByName(m_resDiffuseMap.strName.c_str(), &m_resDiffuseMap.bindDesc));

	m_resSpecularMap.strName = "gSpecularMap";
	HR(pReflector->GetResourceBindingDescByName(m_resSpecularMap.strName.c_str(), &m_resSpecularMap.bindDesc));

	m_resNormalMap.strName = "gNormalMap";
	HR(pReflector->GetResourceBindingDescByName(m_resNormalMap.strName.c_str(), &m_resNormalMap.bindDesc));

	m_resShadowMap.strName = "gShadowMap";
	HR(pReflector->GetResourceBindingDescByName(m_resShadowMap.strName.c_str(), &m_resShadowMap.bindDesc));

	// close the reflection interface
	pReflector.Release();

	// Get the binding info of all the pixel shader textures for the PCF technique
	HR(D3DReflect(m_pSecondPassPCFPixelByteCode->GetBufferPointer(), m_pSecondPassPCFPixelByteCode->GetBufferSize(),
		IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&pReflector.p)));

	m_resDiffuseMapPCF.strName = "gDiffuseMap";
	HR(pReflector->GetResourceBindingDescByName(m_resDiffuseMapPCF.strName.c_str(), &m_resDiffuseMapPCF.bindDesc));

	m_resSpecularMapPCF.strName = "gSpecularMap";
	HR(pReflector->GetResourceBindingDescByName(m_resSpecularMapPCF.strName.c_str(), &m_resSpecularMapPCF.bindDesc));

	m_resNormalMapPCF.strName = "gNormalMap";
	HR(pReflector->GetResourceBindingDescByName(m_resNormalMapPCF.strName.c_str(), &m_resNormalMapPCF.bindDesc));

	m_resShadowMapPCF.strName = "gShadowMap";
	HR(pReflector->GetResourceBindingDescByName(m_resShadowMapPCF.strName.c_str(), &m_resShadowMapPCF.bindDesc));

	// close the reflection interface
	pReflector.Release();
}

// Initializes the sampler states of all shaders. (pure virtual, must be implemented by derived class)
void ShadowNormalMappingEffect::initShaderSamplers() /*=0*/
{
	// Get the binding info about the sampler states for the simple technique
	ShaderReflectionPtr pReflector;
	HR(D3DReflect(m_pSecondPassSimplePixelByteCode->GetBufferPointer(), m_pSecondPassSimplePixelByteCode->GetBufferSize(),
		IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&pReflector.p)));
	m_samTriLinear.strName = "gTriLinearSam";
	HR(pReflector->GetResourceBindingDescByName(m_samTriLinear.strName.c_str(), &m_samTriLinear.bindDesc));
	m_samShadow.strName = "gShadowSam";
	HR(pReflector->GetResourceBindingDescByName(m_samShadow.strName.c_str(), &m_samShadow.bindDesc));
	pReflector.Release();

	// Get the binding info about the sampler states for the PCF technique
	HR(D3DReflect(m_pSecondPassPCFPixelByteCode->GetBufferPointer(), m_pSecondPassPCFPixelByteCode->GetBufferSize(),
		IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&pReflector.p)));
	m_samTriLinearPCF.strName = "gTriLinearSam";
	HR(pReflector->GetResourceBindingDescByName(m_samTriLinearPCF.strName.c_str(), &m_samTriLinearPCF.bindDesc));
	m_samShadowPCF.strName = "gShadowSam";
	HR(pReflector->GetResourceBindingDescByName(m_samShadowPCF.strName.c_str(), &m_samShadowPCF.bindDesc));
	pReflector.Release();

	// Initialize shader sampler states as required by the shadow map render pass' shaders
	SamplerStateMaker samplerStateMaker;
	samplerStateMaker.makeTriLinear(m_pDevice, m_samTriLinear.pSamplerState);
	samplerStateMaker.makeTriPoint(m_pDevice, m_samShadow.pSamplerState);
	samplerStateMaker.makeTriLinearComparison(m_pDevice, m_samShadowPCF.pSamplerState);

	// Copy the shadow sampler state resource view from the standard technique to that of the PCF technique's
	m_samTriLinearPCF.pSamplerState = m_samTriLinear.pSamplerState;

	// Determine the starting bind point at which the sampler states will be bound in the shader
	// and initialize the sampler state array which will be bound to the simple pixel shader accordingly
	m_ppSamplers.resize(2);
	if (m_samTriLinear.bindDesc.BindPoint < m_samShadow.bindDesc.BindPoint)
	{
		m_iSamplerStartBindPoint = m_samTriLinear.bindDesc.BindPoint;
		m_ppSamplers[0] = m_samTriLinear.pSamplerState.p;
		m_ppSamplers[1] = m_samShadow.pSamplerState.p;
	}
	else
	{
		m_iSamplerStartBindPoint = m_samShadow.bindDesc.BindPoint;
		m_ppSamplers[0] = m_samShadow.pSamplerState.p;
		m_ppSamplers[1] = m_samTriLinear.pSamplerState.p;
	}
	
	// Determine the starting bind point at which the sampler states will be bound in the shader
	// and initialize the sampler state array which will be bound to the PCF pixel shader accordingly
	m_ppSamplersPCF.resize(2);
	if (m_samTriLinearPCF.bindDesc.BindPoint < m_samShadowPCF.bindDesc.BindPoint)
	{
		m_iSamplerStartBindPointPCF = m_samTriLinearPCF.bindDesc.BindPoint;
		m_ppSamplersPCF[0] = m_samTriLinearPCF.pSamplerState.p;
		m_ppSamplersPCF[1] = m_samShadowPCF.pSamplerState.p;
	}
	else
	{
		m_iSamplerStartBindPointPCF = m_samShadowPCF.bindDesc.BindPoint;
		m_ppSamplersPCF[0] = m_samShadowPCF.pSamplerState.p;
		m_ppSamplersPCF[1] = m_samTriLinearPCF.pSamplerState.p;
	}
}