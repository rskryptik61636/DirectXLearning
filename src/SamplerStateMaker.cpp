// source file for the SamplerStateMaker class

#include "SamplerStateMaker.h"

// Creates a sampler state that uses linear filtering for minification, magnification as well as mipmaping
void SamplerStateMaker::makeTriLinear(const DevicePtr &pDevice, SamplerStatePtr &pSamplerState)	const
{
	// define the sampler state to be used in the pixel shader
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));

	// linear filtering for minification, magnification as well as mipmaping
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	// tile the texture along u, v and w
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	// set max anisotropy to 1 which really doesn't matter as we're not performing anisotropic filtering
	samplerDesc.MaxAnisotropy = 1;

	// use no special MIP clamping or bias
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// don't use a comparison function
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	// border address mode is not used, so this parameter is ignored
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;

	// create the sampler state
	HR(pDevice->CreateSamplerState(&samplerDesc, &pSamplerState.p));
}

// Creates a sampler state that uses point filtering for minification, magnification as well as mipmaping, 
// can be used to sample shadow map textures.
void SamplerStateMaker::makeTriPoint(const DevicePtr &pDevice, SamplerStatePtr &pSamplerState)
{
	// Get the default sampler state description and update the filter
	D3D11_SAMPLER_DESC desc = defaultSamplerDesc();
	desc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;

	// Create the sampler state
	HR(pDevice->CreateSamplerState(&desc, &pSamplerState.p));
}

// Creates a comparison sampler state that uses linear filtering for minification, magnification as well as mipmaping
void SamplerStateMaker::makeTriLinearComparison(const DevicePtr &pDevice, SamplerStatePtr &pSamplerState)
{
	// Get the default sampler state description and update it accordingly.
	D3D11_SAMPLER_DESC desc = defaultSamplerDesc();
	desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

	// Create the sampler state.
	HR(pDevice->CreateSamplerState(&desc, &pSamplerState.p));
}

// Creates a default sampler state description as defined in: http://msdn.microsoft.com/en-us/library/windows/desktop/ff476207%28v=vs.85%29.aspx
D3D11_SAMPLER_DESC SamplerStateMaker::defaultSamplerDesc()
{
	D3D11_SAMPLER_DESC desc;
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.MinLOD = -FLT_MAX;
	desc.MaxLOD = FLT_MAX;
	desc.MipLODBias = 0.0f;
	desc.MaxAnisotropy = 1;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	for (std::size_t i = 0; i < 4; ++i)
		desc.BorderColor[i] = 1.0f;

	return desc;
}