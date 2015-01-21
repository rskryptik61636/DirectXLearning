// GBuffer class houses all the textures used for Deferred Shading.
// Based off of the Shadow Map class.

#ifndef G_BUFFER_H
#define G_BUFFER_H

#include "d3dUtil.h"

#include "Texture2D.h"

class GBuffer
{
public:

	// Param ctor
	// @param	pDevice: D3D11 device
	// @param	width: Width of all the GBuffer textures
	// @param	height: Height of all the GBuffer textures
	explicit GBuffer(const DevicePtr &pDevice, const UINT width, const UINT height);

	// Dtor
	~GBuffer();

	// Function to recreate the GBuffer textures with the given width and height
	// @param uiWidth: new width of all GBuffer textures
	// @param uiHeight: new height of all GBuffer textures
	void recreateBuffers(const DevicePtr &pDevice, const UINT uiWidth, const UINT uiHeight);

	// Accessor function which returns the position map
	Texture2D* positionMap();

	// Accessor function which returns the normal map
	Texture2D* normalMap();

	// Accessor function which returns the depth map
	Depth2D* depthMap();

	// Accessor function which returns the diffuse map
	Texture2D* diffuseMap();

	// Accessor function which returns the specular map
	Texture2D* specularMap();

	// Binds the GBuffer render targets and depth stencil view to the IA stage.
	// The render targets are bound in the following order:
	// 1) Position map
	// 2) Normal map
	// 3) Diffuse map
	// 4) Specular map
	void bindRenderTargets(const DeviceContextPtr &pDeviceContext);

	// Unbinds the GBuffer render targets and depth stencil view from the IA stage.
	void unbindRenderTargets(const DeviceContextPtr &pDeviceContext);

	// Clear the GBuffer's depth stencil view 
	void clearDSV(const DeviceContextPtr &pDeviceContext);

	// Clear the GBuffer's render targets.
	void clearRenderTargets(const DeviceContextPtr &pDeviceContext);

	// Accessor functions for the shadow map's width and height
	inline UINT width() const	{ return m_nWidth; }
	inline UINT height() const	{ return m_nHeight; }

private:
	GBuffer(const GBuffer& rhs);
	GBuffer& operator=(const GBuffer& rhs);

private:
	UINT m_nWidth;
	UINT m_nHeight;
	//UINT m_nGBuffers;

	CTexture2DPtr m_pPositionMap, m_pNormalMap, m_pDiffuseMap, m_pSpecularMap;
	CDepth2DPtr m_pDepthMap;
	/*RenderTargetViewPtr m_pPositionMapRTV, m_pNormalMapRTV, m_pDiffuseMapRTV, m_pSpecularMapRTV;
	ShaderResourceViewPtr m_pPositionMapSRV, m_pNormalMapSRV, m_pDepthMapSRV, m_pDiffuseMapSRV, m_pSpecularMapSRV;
	DepthStencilViewPtr m_pDepthMapDSV;*/

	//std::vector<DepthStencilViewPtr> m_pDepthMapSubDSV;

	D3D11_VIEWPORT m_viewport;
};	// end of class GBuffer
typedef std::unique_ptr<GBuffer> GBufferPtr;

#endif // G_BUFFER_H