// Motion blur using deferred shading.

#include "deferredShadingCommon.hlsli"

#include "deferredShadingCommonPS.hlsli"

// Constant buffer per frame
cbuffer cbPerFrame	: register(b0)
{
	float4x4 gCurrViewProj;	// current frame's view projection matrix
	float4x4 gPrevViewProj;	// previous frame's view projection matrix
	int gNumSamples;	// no. of samples taken across computed velocity vector

	float3 gEyePosW;
};

// GBuffer map sampler.
SamplerState gBufferSampler : register(s0);

float4 deferredMotionBlurPS(VS_OUT vIn) : SV_TARGET
{
	// Get the geometry information from the GBuffer.
	float3 posW = gPositionMap.Sample(gBufferSampler, vIn.texC).xyz;
	float3 normalW = gNormalMap.Sample(gBufferSampler, vIn.texC).xyz;

	// Compute the homogenous clip space position in the current frame and the previous frame
	// using the corresponding view projection matrices and compute a velocity vector
	// from the difference between them.
	float4 currPosH = mul(float4(posW, 1.0f), gCurrViewProj);
	currPosH /= currPosH.w;	// perspective divide
	//currPosH = 0.5f * currPosH + 0.5f;	// map from [-1,1] to [0,1]

	float4 prevPosH = mul(float4(posW, 1.0f), gPrevViewProj);
	prevPosH /= prevPosH.w;	// perspective divide
	//prevPosH = 0.5f * prevPosH + 0.5f;	// map from [-1,1] to [0,1]

	float2 velocity = (currPosH.xy - prevPosH.xy) / gNumSamples /*8.f*/;

#if 0
	// Accummulate the lighting computations across the velocity vector by the given no. of samples
	float3 litColour = float3(0.0f, 0.0f, 0.0f);
	for (int n = 1; n <= gNumSamples; ++n)
	{
		// Get the geometry information from the GBuffer.
		float3 posW = gPositionMap.Sample(gBufferSampler, vIn.texC + n*velocity).xyz;
		float3 normalW = gNormalMap.Sample(gBufferSampler, vIn.texC + n*velocity).xyz;

		// Get the shading information from the GBuffer.
		float4 diffuse = gDiffuseMap.Sample(gBufferSampler, vIn.texC + n*velocity);
		float4 specular = gSpecularMap.Sample(gBufferSampler, vIn.texC + n*velocity);
		if (specular.y == 0.0f && specular.z == 0.0f)	// The specular map value is a grayscale value
			specular = float4(specular.x, specular.x, specular.x, specular.w);

		// Get the no. of lights of each type in the scene.
		uint nParallelLights, nPointLights, nSpotLights, dummy;
		gParallelLights.GetDimensions(nParallelLights, dummy);
		gPointLights.GetDimensions(nPointLights, dummy);
		gSpotLights.GetDimensions(nSpotLights, dummy);

		// Compute the lit colour for this pixel.

		// Initialize the lit colour to the ambient lighting contribution.		
		if (nParallelLights > 0)
			litColour += gParallelLights[0].ambient.rgb * diffuse.rgb;
		else if (nPointLights > 0)
			litColour += gPointLights[0].ambient.rgb * diffuse.rgb;
		else if (nSpotLights > 0)
			litColour += gSpotLights[0].ambient.rgb * diffuse.rgb;
		else
			litColour += float3(0.0f, 0.0f, 0.0f);

		// Set the surface info for this pixel.
		SurfaceInfo surfaceInfo = { posW, normalW, diffuse, specular };

		// Accummulate the lighting contributions of each light in the scene.
		[loop]
		for (uint i = 0; i < nParallelLights; ++i)
			litColour += ParallelLight(surfaceInfo, gParallelLights[i], gEyePosW);

		[loop]
		for (uint j = 0; j < nPointLights; ++j)
			litColour += PointLight(surfaceInfo, gPointLights[j], gEyePosW);

		[loop]
		for (uint k = 0; k < nSpotLights; ++k)
			litColour += SpotLight(surfaceInfo, gSpotLights[k], gEyePosW);
	}

	// Divide the computed light colour by the no. of samples and return it
	return float4(litColour / float3(gNumSamples, gNumSamples, gNumSamples), 1.0f);
#endif	// 0

#if 1
	// Get the shading information from the GBuffer by sampling across the computed velocity across the given no. of samples.
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f), specular = float4(0.0f, 0.0f, 0.0f, 0.0f); 
	for (float n = 0; n < gNumSamples; ++n)
	{
		diffuse += gDiffuseMap.Sample(gBufferSampler, vIn.texC + n*velocity);
		
		float4 currSpecular = gSpecularMap.Sample(gBufferSampler, vIn.texC + n*velocity);
		if (currSpecular.y == 0.0f && currSpecular.z == 0.0f)	// The specular map is a grayscale value
			specular += float4(currSpecular.x, currSpecular.x, currSpecular.x, currSpecular.w);
		else
			specular += currSpecular;
	}

	// Divide the accummulated diffuse samples by the no. of samples to get the final sampled colours
	diffuse /= (float)gNumSamples;
	specular /= (float)gNumSamples;

	// Get the no. of lights of each type in the scene.
	uint nParallelLights, nPointLights, nSpotLights, dummy;
	gParallelLights.GetDimensions(nParallelLights, dummy);
	gPointLights.GetDimensions(nPointLights, dummy);
	gSpotLights.GetDimensions(nSpotLights, dummy);

	// Compute the lit colour for this pixel.

	// Initialize the lit colour to the ambient lighting contribution.
	float3 litColour;
	if (nParallelLights > 0)
		litColour = gParallelLights[0].ambient.rgb * diffuse.rgb;
	else if (nPointLights > 0)
		litColour = gPointLights[0].ambient.rgb * diffuse.rgb;
	else if (nSpotLights > 0)
		litColour = gSpotLights[0].ambient.rgb * diffuse.rgb;
	else
		litColour = float3(0.0f, 0.0f, 0.0f);

	// Set the surface info for this pixel.
	SurfaceInfo surfaceInfo = { posW, normalW, diffuse, specular };

	// Accummulate the lighting contributions of each light in the scene.
	[loop]
	for (uint i = 0; i < nParallelLights; ++i)
		litColour += ParallelLight(surfaceInfo, gParallelLights[i], gEyePosW);

	[loop]
	for (uint j = 0; j < nPointLights; ++j)
		litColour += PointLight(surfaceInfo, gPointLights[j], gEyePosW);

	[loop]
	for (uint k = 0; k < nSpotLights; ++k)
		litColour += SpotLight(surfaceInfo, gSpotLights[k], gEyePosW);

	return float4(litColour, 1.0f);
#endif	// 0
	
}