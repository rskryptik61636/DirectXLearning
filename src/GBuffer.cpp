// GBuffer class houses all the textures used for Deferred Shading.
// Based off of the Shadow Map class.

#include "GBuffer.h"

// Param ctor
// @param	pDevice: D3D11 device
// @param	width: Width of all the GBuffer textures
// @param	height: Height of all the GBuffer textures
GBuffer::GBuffer(const DevicePtr &pDevice, const UINT width, UINT const height)
{
	// Invoke the recreate buffers format since we're doing the same thing
	recreateBuffers(pDevice, width, height);

	// @TODO: remove when done testing
#if 0
	// Create the default texture desc which will be common to all the GBuffer textures
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.ArraySize = 1;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Height = m_viewport.Height;
	textureDesc.MipLevels = 0;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;	// @TODO: should be modified if MSAA is enabled
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.Width = m_viewport.Width;

	// Specialize the texture desc, RTV (DSV for the depth stencil view) desc and SRV desc for each of the render targets and create them accordingly.

	// Position map
	textureDesc.Format = DXGI_FORMAT_R32G32B32_TYPELESS;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	Texture2DPtr pTexture;
	HR(pDevice->CreateTexture2D(&textureDesc, NULL, &pTexture.p));

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	HR(pDevice->CreateRenderTargetView(pTexture.p, &rtvDesc, &m_pPositionMapRTV.p));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	HR(pDevice->CreateShaderResourceView(pTexture.p, &srvDesc, &m_pPositionMapSRV.p));

	// Normal map	(@TODO: change the format if necessary)
	HR(pDevice->CreateRenderTargetView(pTexture.p, &rtvDesc, &m_pNormalMapRTV.p));
	HR(pDevice->CreateShaderResourceView(pTexture.p, &srvDesc, &m_pNormalMapSRV.p));

	// Diffuse map (need to change the texture format for the colour buffers)
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_TYPELESS;
	HR(pDevice->CreateTexture2D(&textureDesc, NULL, &pTexture.p));
	
	rtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	HR(pDevice->CreateRenderTargetView(pTexture.p, &rtvDesc, &m_pDiffuseMapRTV.p));

	srvDesc.Format = rtvDesc.Format;
	HR(pDevice->CreateShaderResourceView(pTexture.p, &srvDesc, &m_pDiffuseMapSRV.p));

	// Specular map
	HR(pDevice->CreateRenderTargetView(pTexture.p, &rtvDesc, &m_pSpecularMapRTV.p));
	HR(pDevice->CreateShaderResourceView(pTexture.p, &srvDesc, &m_pSpecularMapSRV.p));

	// Depth map
	textureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	HR(pDevice->CreateTexture2D(&textureDesc, NULL, &pTexture.p));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	HR(pDevice->CreateDepthStencilView(pTexture.p, &dsvDesc, &m_pDepthMapDSV.p));

	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	HR(pDevice->CreateShaderResourceView(pTexture.p, NULL, &m_pDepthMapSRV.p));
#endif	// 0
}

// Dtor
GBuffer::~GBuffer()	{}

// Function to recreate the GBuffer textures with the given width and height
// @param uiWidth: new width of all GBuffer textures
// @param uiHeight: new height of all GBuffer textures
void GBuffer::recreateBuffers(const DevicePtr &pDevice, const UINT uiWidth, const UINT uiHeight)
{
	// Update the width and height
	m_nWidth = uiWidth;	m_nHeight = uiHeight;

	// Init the viewport
	m_viewport.Width = static_cast<FLOAT>(m_nWidth);
	m_viewport.Height = static_cast<FLOAT>(m_nHeight);
	m_viewport.TopLeftX = m_viewport.TopLeftY = 0.0f;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	// @TODO: optimize the buffer formats

	// Create the position map
	DXGI_FORMAT format(/*DXGI_FORMAT_R32G32B32_FLOAT*/DXGI_FORMAT_R16G16B16A16_FLOAT);
	UINT renderTargetBindFlags(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	m_pPositionMap.reset(new Texture2D(pDevice, m_nWidth, m_nHeight, format, renderTargetBindFlags));

	// Create the normal map
	m_pNormalMap.reset(new Texture2D(pDevice, m_nWidth, m_nHeight, format, renderTargetBindFlags));

	// Create the diffuse map
	format = /*DXGI_FORMAT_R32G32B32A32_FLOAT*/ DXGI_FORMAT_R8G8B8A8_UNORM;
	m_pDiffuseMap.reset(new Texture2D(pDevice, m_nWidth, m_nHeight, format));

	// Create the specular map
	m_pSpecularMap.reset(new Texture2D(pDevice, m_nWidth, m_nHeight, format));

	// Create the depth map
	m_pDepthMap.reset(new Depth2D(pDevice, m_nWidth, m_nHeight));
}

// Accessor function which returns the position map
Texture2D* GBuffer::positionMap()	{ return m_pPositionMap.get(); }

// Accessor function which returns the normal map
Texture2D* GBuffer::normalMap()	{ return m_pNormalMap.get(); }

// Accessor function which returns the depth map
Depth2D* GBuffer::depthMap()	{ return m_pDepthMap.get(); }

// Accessor function which returns the diffuse map
Texture2D* GBuffer::diffuseMap()	{ return m_pDiffuseMap.get(); }

// Accessor function which returns the specular map
Texture2D* GBuffer::specularMap()	{ return m_pSpecularMap.get(); }

// Binds the GBuffer render targets and depth stencil view to the IA stage.
// The render targets are bound in the following order:
// 1) Position map
// 2) Normal map
// 3) Diffuse map
// 4) Specular map
void GBuffer::bindRenderTargets(const DeviceContextPtr &pDeviceContext)
{
	// Bind the render targets and the depth stencil view in the required order
	const std::array<ID3D11RenderTargetView* const, 4> ppRenderTargets = 
	{ m_pPositionMap->GetRenderTarget(),
		m_pNormalMap->GetRenderTarget(),
		m_pDiffuseMap->GetRenderTarget(),
		m_pSpecularMap->GetRenderTarget() };
	pDeviceContext->OMSetRenderTargets(ppRenderTargets.size(), ppRenderTargets.data(), m_pDepthMap->GetDepthStencil());
}

// Unbinds the GBuffer render targets and depth stencil view from the IA stage.
void GBuffer::unbindRenderTargets(const DeviceContextPtr &pDeviceContext)
{
	// Set the render targets and depth stencil view to NULL
	const std::array<ID3D11RenderTargetView* const, 4> ppRenderTargets = { NULL, NULL, NULL, NULL };
	pDeviceContext->OMSetRenderTargets(ppRenderTargets.size(), ppRenderTargets.data(), NULL);
}

// Clear the GBuffer's depth stencil view 
void GBuffer::clearDSV(const DeviceContextPtr &pDeviceContext)
{
	pDeviceContext->ClearDepthStencilView(m_pDepthMap->GetDepthStencil(), D3D11_CLEAR_DEPTH, 0.999f, 0);
}

// Clear the GBuffer's render targets.
void GBuffer::clearRenderTargets(const DeviceContextPtr &pDeviceContext)
{
	const FLOAT clearColour[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	pDeviceContext->ClearRenderTargetView(m_pPositionMap->GetRenderTarget(), clearColour);
	pDeviceContext->ClearRenderTargetView(m_pNormalMap->GetRenderTarget(), clearColour);
	pDeviceContext->ClearRenderTargetView(m_pDiffuseMap->GetRenderTarget(), clearColour);
	pDeviceContext->ClearRenderTargetView(m_pSpecularMap->GetRenderTarget(), clearColour);
}