//***************************************************************************************
// ShadowMap.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "ShadowMap.h"

ShadowMap::ShadowMap(const DevicePtr &pDevice, UINT width, UINT height, UINT nMaps /*= 1*/)
: m_nWidth(width), m_nHeight(height), m_nShadowMaps(nMaps)/*, m_pDepthMapSRV(0), m_pDepthMapDSV(0)*/	// NOTE: don't need this anymore as they are smart pointers
{
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
    m_viewport.Width    = static_cast<float>(width);
    m_viewport.Height   = static_cast<float>(height);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

	// Use typeless format because the DSV is going to interpret
	// the bits as DXGI_FORMAT_D24_UNORM_S8_UINT, whereas the SRV is going to interpret
	// the bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.
	D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width     = m_nWidth;
    texDesc.Height    = m_nHeight;
    texDesc.MipLevels = 1;
	texDesc.ArraySize = /*1;*/ m_nShadowMaps;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    texDesc.SampleDesc.Count   = 1;  
    texDesc.SampleDesc.Quality = 0;  
    texDesc.Usage          = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0; 
    texDesc.MiscFlags      = 0;

    Texture2DPtr pDepthMap = 0;
    HR(pDevice->CreateTexture2D(&texDesc, 0, &pDepthMap.p));

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//if (m_nShadowMaps == 1)	// Initialize the depth stencil view based on the no. of shadow maps we're targeting
	//{
	//	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//	dsvDesc.Texture2D.MipSlice = 0;

	//	HR(pDevice->CreateDepthStencilView(pDepthMap.p, &dsvDesc, &m_pDepthMapDSV.p));
	//}
	//else
	{
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		dsvDesc.Texture2DArray.MipSlice = 0;
		dsvDesc.Texture2DArray.ArraySize = m_nShadowMaps;
		dsvDesc.Texture2DArray.FirstArraySlice = 0;

		HR(pDevice->CreateDepthStencilView(pDepthMap.p, &dsvDesc, &m_pDepthMapDSV.p));

		// Reuse the DSV description and just modify the array size and the first array slice per index
		dsvDesc.Texture2DArray.ArraySize = 1;

		// Initialize the sub DSV array since we are targeting more than one shadow map
		m_pDepthMapSubDSV.resize(m_nShadowMaps);
		for (std::size_t i = 0; i < m_pDepthMapSubDSV.size(); ++i)
		{
			dsvDesc.Texture2DArray.FirstArraySlice = i;
			HR(pDevice->CreateDepthStencilView(pDepthMap.p, &dsvDesc, &m_pDepthMapSubDSV[i].p));
		}
	}
    
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; /*DXGI_FORMAT_R32_FLOAT;*/
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
 //   if (m_nShadowMaps == 1)	// Initialize the SRV based on the no. of shadow maps that we are targeting
	//{
	//	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	//	srvDesc.Texture2D.MostDetailedMip = 0;
	//}
	//else
	{
		srvDesc.Texture2DArray.MipLevels = texDesc.MipLevels;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.Texture2DArray.ArraySize = m_nShadowMaps;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
	}
    HR(pDevice->CreateShaderResourceView(pDepthMap.p, &srvDesc, &m_pDepthMapSRV.p));

    // View saves a reference to the texture so we can release our reference.
    //pDepthMap.Release();	// NOTE: don't need this anymore
}

ShadowMap::~ShadowMap()
{
    /*ReleaseCOM(m_pDepthMapSRV);	// NOTE: don't need this anymore
	ReleaseCOM(m_pDepthMapDSV);*/
}

ShaderResourceViewPtr ShadowMap::DepthMapSRV()
{
    return m_pDepthMapSRV/*.p*/;
}

void ShadowMap::BindDsvAndSetNullRenderTarget(const DeviceContextPtr &pDeviceContext)
{
    pDeviceContext->RSSetViewports(1, &m_viewport);

	// Set null render target because we are only going to draw to depth buffer.
	// Setting a null render target will disable color writes.
    ID3D11RenderTargetView* renderTargets[1] = {0};
	pDeviceContext->OMSetRenderTargets(1, renderTargets, m_pDepthMapDSV.p);
    
    pDeviceContext->ClearDepthStencilView(m_pDepthMapDSV.p, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void ShadowMap::BindSubDsvAndSetNullRenderTarget(const DeviceContextPtr &pDeviceContext, const UINT nIndex)
{
	// Ensure that nIndex is within range
	assert(nIndex <= m_nShadowMaps);

	pDeviceContext->RSSetViewports(1, &m_viewport);

	// Set null render target because we are only going to draw to depth buffer.
	// Setting a null render target will disable color writes.
	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	pDeviceContext->OMSetRenderTargets(1, renderTargets, m_pDepthMapSubDSV[nIndex].p);
}

void ShadowMap::ClearDepthStencilBuffer(const DeviceContextPtr &pDeviceContext)
{
	// Set null render target and the depth stencil view and clear it
	pDeviceContext->OMSetRenderTargets(0, NULL, m_pDepthMapDSV.p);	// @TODO: get rid off if we don't need it
	pDeviceContext->ClearDepthStencilView(m_pDepthMapDSV.p, D3D11_CLEAR_DEPTH, 1.0f, 0);
}