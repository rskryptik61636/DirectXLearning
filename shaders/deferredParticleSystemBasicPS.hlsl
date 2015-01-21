// Deferred particle system pixel shader, writes geometry and shading information of the particles to the GBuffer.

#include "deferredShadingCommon.hlsli"

#include "particleSystemCommon.hlsli"

// Per object constants
cbuffer cbPerObject : register(b0)
{
	float3 gEyePosW;
};

// Particles colour map
Texture2D<float4> gParticleTex : register(t0);

// Sampler state used to sample the particles colour map.
SamplerState gParticleSampler : register(s0);

PS_OUT deferredParticleSystemBasicPS(ForwardGSOut pIn)
{
	// Init the output struct
	PS_OUT pOut;

	// Copy the input position into the output params struct and compute the normal as (cam_position - particle_position)
	pOut.posW = float4(pIn.posW, 1.0f);
	pOut.normalW = float4(normalize(gEyePosW - pIn.posW), 1.0f);

	// Set the diffuse output colour as that of sampled particle texture.
	pOut.diffuse = gParticleTex.Sample(gParticleSampler, pIn.texC);

	// Hardcode the specular colour to white for now.
	pOut.specular = float4(1.0f, 1.0f, 1.0f, 1.0f);

	return pOut;
}