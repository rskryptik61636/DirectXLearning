// Pixel shader for the skybox effect

#include "skyboxCommon.hlsli"

// Cube map
TextureCube gSkyboxTexture	: register(t0);

// Cube map sampler
SamplerState gSkyboxSampler	: register(s0);

float4 skyboxPS(SkyboxVSOut pIn) : SV_TARGET
{
	return gSkyboxTexture.Sample(gSkyboxSampler, pIn.texC);
}