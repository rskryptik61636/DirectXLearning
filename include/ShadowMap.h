//***************************************************************************************
// ShadowMap.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper class for implementing shadows via shadow mapping algorithm.
//***************************************************************************************

// Modified by Rohit S to work with DXEngine's version of d3dUtil.h

#ifndef SHADOW_MAPPER_H
#define SHADOW_MAPPER_H

#include "d3dUtil.h"
//#include "Camera.h"

class ShadowMap
{
public:
	ShadowMap(const DevicePtr &pDevice, UINT width, UINT height, UINT nMaps = 1);
	~ShadowMap();

	ShaderResourceViewPtr DepthMapSRV();

	void BindDsvAndSetNullRenderTarget(const DeviceContextPtr &pDeviceContext);

	void BindSubDsvAndSetNullRenderTarget(const DeviceContextPtr &pDeviceContext, const UINT nIndex);

	void ClearDepthStencilBuffer(const DeviceContextPtr &pDeviceContext);

	// Accessor functions for the shadow map's width and height
	inline UINT width() const	{ return m_nWidth; }
	inline UINT height() const	{ return m_nHeight; }

private:
	ShadowMap(const ShadowMap& rhs);
	ShadowMap& operator=(const ShadowMap& rhs);

private:
	UINT m_nWidth;
	UINT m_nHeight;
	UINT m_nShadowMaps;

	ShaderResourceViewPtr m_pDepthMapSRV;
	DepthStencilViewPtr m_pDepthMapDSV;
	std::vector<DepthStencilViewPtr> m_pDepthMapSubDSV;

	D3D11_VIEWPORT m_viewport;
};
typedef std::unique_ptr<ShadowMap> ShadowMapPtr;

#endif // SHADOW_MAPPER_H