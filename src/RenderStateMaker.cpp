// Source file for the RenderStateMaker class

#include "RenderStateMaker.h"

// Creates a rasterizer state wherein front facing triangles have CCW winding order
void RenderStateMaker::createCounterClockwiseFrontFacingRS(const DevicePtr &pDevice, RasterizerStatePtr &pRS,
	const INT iDepthBias /*= 100000*/,
	const FLOAT fDepthBiasClamp /*= 0.0f*/,
	const FLOAT fSlopeScaledDepthBias /*= 1.0f*/)
{
	// get the default rasterizer state description and modify it
	// to specify that front facing triangles have a CCW winding order
	D3D11_RASTERIZER_DESC rsDesc = defaultRSDesc();
	rsDesc.FrontCounterClockwise = TRUE;
	rsDesc.DepthBias = iDepthBias;
	rsDesc.DepthBiasClamp = fDepthBiasClamp;
	rsDesc.SlopeScaledDepthBias = fSlopeScaledDepthBias;

	// create the corresponding rasterizer state
	HR(pDevice->CreateRasterizerState(&rsDesc, &pRS.p));
}

// Creates a rasterizer state with appropriate depth biasing to generate shadow maps
void RenderStateMaker::createShadowMapRS(const DevicePtr &pDevice, RasterizerStatePtr &pRS,
	const INT iDepthBias /*= 100000.0f*/,
	const FLOAT fDepthBiasClamp /*= 0.0f*/,
	const FLOAT fSlopeScaledDepthBias /*= 1.0f*/)
{
	// get the default rasterizer state description and modify the depth bias values
	// to the user specified values
	D3D11_RASTERIZER_DESC rsDesc = defaultRSDesc();
	rsDesc.DepthBias = iDepthBias;
	rsDesc.DepthBiasClamp = fDepthBiasClamp;
	rsDesc.SlopeScaledDepthBias = fSlopeScaledDepthBias;

	// create the rasterizer state
	HR(pDevice->CreateRasterizerState(&rsDesc, &pRS.p));
}

// Creates a standard depth stencil state to enable depth testing
void RenderStateMaker::createDepthTestingDSS(const DevicePtr &pDevice, DepthStencilStatePtr &pDSS)
{
	// get the default depth stencil state description and create a depth stencil state from it
	D3D11_DEPTH_STENCIL_DESC dssDesc = defaultDSSDesc();
	HR(pDevice->CreateDepthStencilState(&dssDesc, &pDSS.p));
}

// Creates as additive blending depth stencil state
void RenderStateMaker::createAdditiveBlendingBS(const DevicePtr &pDevice, BlendStatePtr &pBS)
{
	// Get the default depth stencil desc and modify it
	D3D11_BLEND_DESC desc = CD3D11_BLEND_DESC(D3D11_DEFAULT);
	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	// Create the blend state
	HR(pDevice->CreateBlendState(&desc, &pBS.p));
}

// Creates a rasterizer state which culls neither the front nor the back faces.
void RenderStateMaker::createCullNoneRS(const DevicePtr &pDevice, RasterizerStatePtr &pRS)
{
	// Get the default rasterizer desc and set the cull mode to none.
	D3D11_RASTERIZER_DESC rsDesc(defaultRSDesc());
	rsDesc.CullMode = D3D11_CULL_NONE;

	// Create the rasterizer state.
	HR(pDevice->CreateRasterizerState(&rsDesc, &pRS.p));
}

// Creates a blend state with the comparison state set to less_equal.
void RenderStateMaker::createLessEqualDepthTestingDSS(const DevicePtr &pDevice, DepthStencilStatePtr &pDSS)
{
	// Get the default blend state desc and set the comparison state to less_equal.
	D3D11_DEPTH_STENCIL_DESC dssDesc = defaultDSSDesc();
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	// Create the depth-stencil state.
	HR(pDevice->CreateDepthStencilState(&dssDesc, &pDSS.p));
}

// Creates a default rasterizer state description
D3D11_RASTERIZER_DESC RenderStateMaker::defaultRSDesc()
{
	// set the default values as specified in: http://msdn.microsoft.com/en-us/library/windows/desktop/ff476198%28v=vs.85%29.aspx
	D3D11_RASTERIZER_DESC rsDesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);
	return rsDesc;

	// @TODO: remove when done testing
#if 0
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthBias = 0;
	rsDesc.SlopeScaledDepthBias = 0.0f;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.ScissorEnable = FALSE;
	rsDesc.MultisampleEnable = FALSE;
	rsDesc.AntialiasedLineEnable = FALSE;
#endif // 0

}

// Creates a default depth stencil state description
D3D11_DEPTH_STENCIL_DESC RenderStateMaker::defaultDSSDesc()
{
	// set the default values as specified in http://msdn.microsoft.com/en-us/library/windows/desktop/ff476110%28v=vs.85%29.aspx
	D3D11_DEPTH_STENCIL_DESC dssDesc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);
	return dssDesc;

	// @TODO: remove when done testing
#if 0
	dssDesc.DepthEnable = TRUE;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dssDesc.StencilEnable = FALSE;
	dssDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dssDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dssDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dssDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dssDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
#endif // 0

	
}