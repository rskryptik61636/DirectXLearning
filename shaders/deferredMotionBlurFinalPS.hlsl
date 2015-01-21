// Motion blur as a post processing effect. Based off of the GPU Gems 3 article. (http://http.developer.nvidia.com/GPUGems3/gpugems3_ch27.html)

#include "debugTextureCommon.hlsli"

// Constants per frame
cbuffer cbPerFrame : register(b0)
{
	float4x4 gCurrViewProj;	// current frame's view projection matrix
	float4x4 gPrevViewProj;	// previous frame's view projection matrix
	float gNumSamples;	// no. of samples taken across computed velocity vector
}

// Position map
Texture2D<float4> gPositionMap : register(t0);

// Colour output
Texture2D<float4> gColourOutput : register(t1);

// Sampler for the position and colour output
SamplerState gSampler : register(s0);

float4 deferredMotionBlurFinalPS(VS_OUT pIn) : SV_TARGET
{
	// Get the current world space position.
	float3 posW = gPositionMap.Sample(gSampler, pIn.texC).xyz;

	// Compute the clip-space position in the current frame.
	float4 currPos = mul(float4(posW, 1.0f), gCurrViewProj);
	currPos.xy /= currPos.w;	// Perspective divide to get in [-1,1]

	// Compute the clip-space position in the previous frame.
	float4 prevPos = mul(float4(posW, 1.0f), gPrevViewProj);
	prevPos.xy /= prevPos.w;	// Perspective divide to get in [-1,1]

	// Compute the velocity vector as the difference of the two clip-space positions.
	float2 velocity = (currPos.xy - prevPos.xy) / gNumSamples /*2.0f*/;

	// Average the colour output over the given no. of samples using the velocity vector.
	float4 finalColour = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (float n = 1; n <= gNumSamples; ++n)
	{
		finalColour += gColourOutput.Sample(gSampler, saturate(pIn.texC + n*velocity));	// NOTE: using saturate to keep the tex coord in [0,1]
	}
	finalColour /= gNumSamples;

	return finalColour;
}