// header file for the SamplerStateMaker class
#ifndef SAMPLER_STATE_MAKER_H
#define SAMPLER_STATE_MAKER_H

#include "d3dUtil.h"

// Class that creates SamplerState objects
class SamplerStateMaker
{
public:

	// Creates a sampler state that uses linear filtering for minification, magnification as well as mipmaping
	void makeTriLinear(const DevicePtr &pDevice, SamplerStatePtr &pSamplerState) const;

	// Creates a sampler state that uses point filtering for minification, magnification as well as mipmaping, 
	// can be used to sample shadow map textures.
	void makeTriPoint(const DevicePtr &pDevice, SamplerStatePtr &pSamplerState);

	// Creates a comparison sampler state that uses linear filtering for minification, magnification as well as mipmaping
	void makeTriLinearComparison(const DevicePtr &pDevice, SamplerStatePtr &pSamplerState);

private:

	// Creates a default sampler state description as defined in: http://msdn.microsoft.com/en-us/library/windows/desktop/ff476207%28v=vs.85%29.aspx
	D3D11_SAMPLER_DESC defaultSamplerDesc();

};	// end of class SamplerStateMaker

#endif	// SAMPLER_STATE_MAKER_H