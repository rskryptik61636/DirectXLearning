// Basic pixel shader for particle systems in forward rendering

#include "particleSystemCommon.hlsli"

Texture2D<float4> gParticleTex : register(t0);

SamplerState gParticleSampler : register(s0);

float4 forwardParticleSystemBasicPS(ForwardGSOut pIn) : SV_TARGET
{
	return gParticleTex.Sample(gParticleSampler, pIn.texC);
	//return pIn.colour;
}