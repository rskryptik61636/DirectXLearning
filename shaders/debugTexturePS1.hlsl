// Pixel shader which is used to debug a texture by drawing it onto a surface

#include "debugTextureCommon.hlsli"

// Constant buffer which contains the index of the texture channel which will be displayed
cbuffer cbPerFrame
{
	int nIndex;	// Must be in [0,3]
	float additive;	// Added to each sampled colour value.
	float multiplier;	// Muliplier for each sampled colour value.
};

// Texture which is to be debugged
Texture2D gDebugTexture;

// Sampler state which will be used to sample the texture
SamplerState texSampler;

float4 debugTexturePS1(VS_OUT pIn)	: SV_Target
{
	// Sample the texture
	float4 colour = gDebugTexture.Sample(texSampler, pIn.texC);
	//colour.rgb = 0.5f * colour.rgb + 0.5f;	// HACK: to convert normals from the [-1,1] to the [0,1] range, needs to be handled cleanly!!!

	// Return the nIndex channel if it is lesser than 3, else return the colour itself
	float4 final;
	if (nIndex < 3)
	{
		final = float4((multiplier * colour[nIndex] + additive).xxx, 1.0f);
		/*final = float4(colour[nIndex], colour[nIndex], colour[nIndex], 1.0f);*/
	}
	else
	{
		final = float4(multiplier * colour.rgb + additive, 1.0f);
	}
	return final;
	//return float4(colour[nIndex], colour[nIndex], colour[nIndex], 1.0f);
}