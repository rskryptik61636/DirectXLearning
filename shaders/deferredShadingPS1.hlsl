// GBuffer generation pixel shader, writes geometry and shading information to the GBuffer.

#include "deferredShadingCommon.hlsli"

// Diffuse colour map
Texture2D gDiffuseMap : register(t0);

// Specular colour map
Texture2D gSpecularMap : register(t1);

// Sampler state used to sample the textures.
SamplerState gSampler : register(s0);

PS_OUT deferredShadingPS1(VS_OUT pIn)
{
	// Init the output struct
	PS_OUT pOut;

	// Just copy the geometry input params into the output params struct
	pOut.posW = float4(pIn.posW, 1.0f);
	pOut.normalW = float4(normalize(pIn.normalW), 1.0f);

	// Compute the colour output params
	pOut.diffuse = gDiffuseMap.Sample(gSampler, pIn.texC);
	pOut.specular = gSpecularMap.Sample(gSampler, pIn.texC);

	return pOut;
}