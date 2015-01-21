// Header file for the RenderStateMaker class
#ifndef RENDER_STATE_MAKER_H
#define RENDER_STATE_MAKER_H

#include "d3dUtil.h"

// Utility class that can be used to create commonly used render states
class RenderStateMaker
{
public:

	// Creates a rasterizer state wherein front facing triangles have CCW winding order
	// NOTE: Updating the default depth bias values as given on MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/ff476198%28v=vs.85%29.aspx (Mar-01-2014)
	void createCounterClockwiseFrontFacingRS(const DevicePtr &pDevice, RasterizerStatePtr &pRS,
											const INT iDepthBias = 0,
											const FLOAT fDepthBiasClamp = 0.0f,
											const FLOAT fSlopeScaledDepthBias = 0.0f);

	// Creates a rasterizer state with appropriate depth biasing to generate shadow maps
	// NOTE: Updating the default depth bias values as given on MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/ff476198%28v=vs.85%29.aspx (Mar-01-2014)
	void createShadowMapRS(const DevicePtr &pDevice, RasterizerStatePtr &pRS, 
							const INT iDepthBias=0,
							const FLOAT fDepthBiasClamp=0.0f,
							const FLOAT fSlopeScaledDepthBias=0.0f);

	// Creates a standard depth stencil state to enable depth testing
	void createDepthTestingDSS(const DevicePtr &pDevice, DepthStencilStatePtr &pDSS);

	// Creates an additive blending blend state
	void createAdditiveBlendingBS(const DevicePtr &pDevice, BlendStatePtr &pBS);

	// Creates a rasterizer state which culls neither the front nor the back faces.
	void createCullNoneRS(const DevicePtr &pDevice, RasterizerStatePtr &pRS);

	// Creates a blend state with the comparison state set to less_equal.
	void createLessEqualDepthTestingDSS(const DevicePtr &pDevice, DepthStencilStatePtr &pDSS);

private:

	// Creates a default rasterizer state description
	D3D11_RASTERIZER_DESC defaultRSDesc();

	// Creates a default depth stencil state description
	D3D11_DEPTH_STENCIL_DESC defaultDSSDesc();
};

#endif	// RENDER_STATE_MAKER_H