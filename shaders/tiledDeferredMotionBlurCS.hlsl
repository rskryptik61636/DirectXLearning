// Motion blur using tiled deferred shading.

#include "tiledDeferredShadingCommon.hlsli"

// Constant buffer per frame
cbuffer cbPerFrame	: register(b0)
{
	float4x4 gCurrViewProj;	// current frame's view projection matrix
	float4x4 gPrevViewProj;	// previous frame's view projection matrix
	int gNumSamples;	// no. of samples taken across computed velocity vector
	float2 gViewportWidthRange;		// viewport min/max width range
	float2 gViewportHeightRange;	// viewport min/max height range

	float3 gEyePosW;
};

[numthreads(TILE_DIMS, TILE_DIMS, 1)]
void tiledDeferredMotionBlurCS(uint3 groupId			: SV_GroupID,
								uint3 dispatchThreadId : SV_DispatchThreadID,
								uint3 groupThreadId : SV_GroupThreadID
								)
{
	// Get the group index.
	uint groupIndex = getGroupIndex(groupThreadId);

	// Get the world space position corresponding to the current thread.
	uint2 texC = dispatchThreadId.xy;
	float3 posW = gPositionMap.Load(uint3(texC, 0)).xyz;

	// Determine the total no. of parallel, point and spot lights.
	uint nParallelLights, nPointLights, nSpotLights, dummy;
	gParallelLights.GetDimensions(nParallelLights, dummy);
	gPointLights.GetDimensions(nPointLights, dummy);
	gSpotLights.GetDimensions(nSpotLights, dummy);

	// Shortlist the lights for this current tile
	shortlistTileLights(posW, groupIndex, nPointLights, nSpotLights);

	// @TODO: Put back when done testing.
	// Read the current world-space normal, diffuse colour and specular colour;
	float3 normalW = gNormalMap.Load(uint3(texC, 0)).xyz;
	/*float4 diffuse = gDiffuseMap.Load(uint3(texC, 0));
	float4 specular = gSpecularMap.Load(uint3(texC, 0));*/

	// Compute the homogenous clip space position in the current frame and the previous frame
	// using the corresponding view projection matrices and compute a velocity vector
	// from the difference between them.
	float4 currPosH = mul(float4(posW, 1.0f), gCurrViewProj);
	currPosH /= currPosH.w;	// perspective divide
	//currPosH = 0.5f * currPosH + 0.5f;	// map from [-1,1] to [0,1]

	float4 prevPosH = mul(float4(posW, 1.0f), gPrevViewProj);
	prevPosH /= prevPosH.w;	// perspective divide
	//prevPosH = 0.5f * prevPosH + 0.5f;	// map from [-1,1] to [0,1]

	// Clamp the homogenous clip space positions to [-1,1] as they might be out of range,
	// i.e., they would have been ordinarily clipped.
	float winMin = -1, winMax = 1;	// homogenous clip space min max
	currPosH.xy = clamp(currPosH.xy, float2(winMin, winMin), float2(winMax, winMax));
	prevPosH.xy = clamp(prevPosH.xy, float2(winMin, winMin), float2(winMax, winMax));

	// Map the current and previous positions from homogenous clip space to screen space.	
	float viewMin = gViewportWidthRange.x, viewMax = gViewportWidthRange.y;	// viewport width min/max
	float2 currPosV, prevPosV;
	currPosV.x = windowToViewport(currPosH.x, winMin, winMax, viewMin, viewMax);
	prevPosV.x = windowToViewport(prevPosH.x, winMin, winMax, viewMin, viewMax);

	viewMin = gViewportHeightRange.x, viewMax = gViewportHeightRange.y;	// viewport height min/max
	currPosV.y = windowToViewport(currPosH.y, winMin, winMax, viewMin, viewMax);
	prevPosV.y = windowToViewport(prevPosH.y, winMin, winMax, viewMin, viewMax);

	// Divide by the no. of samples to get the per-sample velocity vector;
	float2 velocity = (currPosV.xy - prevPosV.xy) / /*gNumSamples*/ 2.f;
	//velocity /= (float)gNumSamples;

	// NOTE: Average lighting computation across the velocity vector. (Slower approach)
#if 1
	// Get the shading information from the GBuffer by sampling across the computed velocity across the given no. of samples.
	// NOTE: Clamping the viewport sampling coordinates to ensure they don't go out of range.
	//float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f), specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 litColour;
	uint2 viewMinClamp = uint2((uint)gViewportWidthRange.x, (uint)gViewportHeightRange.x);
	uint2 viewMaxClamp = uint2((uint)gViewportWidthRange.y, (uint)gViewportHeightRange.y);
	for (float n = 0; n < gNumSamples; ++n)
	{
		int2 texOffset = (int2)(n*velocity);
		uint2 clampedOffset = clamp(texC + texOffset, viewMinClamp, viewMaxClamp);

		// Sample the GBuffer textures at the clamped offset position.
		posW = gPositionMap.Load(uint3(clampedOffset, 0));
		normalW = gNormalMap.Load(uint3(clampedOffset, 0));
		float4 diffuse = gDiffuseMap.Load(uint3(clampedOffset, 0));

		float4 specular;
		float4 currSpecular = gSpecularMap.Load(uint3(clampedOffset, 0));
		if (currSpecular.y == 0.0f && currSpecular.z == 0.0f)	// The specular map is a grayscale value
			specular = float4(currSpecular.xxx, currSpecular.w);
		else
			specular = currSpecular;

		// Initialize the lit colour to the ambient lighting contribution.		
		if (nParallelLights > 0)
			litColour += gParallelLights[0].ambient.rgb * diffuse.rgb;
		else if (sNumShortlistedSpotLights > 0)
			litColour += gPointLights[sShortlistedPointLights[0]].ambient.rgb * diffuse.rgb;
		else if (sNumShortlistedSpotLights > 0)
			litColour += gSpotLights[sShortlistedSpotLights[0]].ambient.rgb * diffuse.rgb;
		else
			litColour = float3(0.0f, 0.0f, 0.0f);

		// Set the surface info for this pixel.
		SurfaceInfo surfaceInfo = { posW, normalW, diffuse, specular };

		// Accummulate the lighting contributions of all the parallel lights since they're not being culled,
		// as well as the shortlisted point and spot lights.
		[loop]
		for (uint i = 0; i < nParallelLights; ++i)
			litColour += ParallelLight(surfaceInfo, gParallelLights[i], gEyePosW);

		[loop]
		for (uint j = 0; j < sNumShortlistedPointLights; ++j)
			litColour += PointLight(surfaceInfo, gPointLights[sShortlistedPointLights[j]], gEyePosW);

		[loop]
		for (uint k = 0; k < sNumShortlistedSpotLights; ++k)
			litColour += SpotLight(surfaceInfo, gSpotLights[sShortlistedSpotLights[k]], gEyePosW);
	}

	// Average the computed colour by the no. of samples.
	litColour.xyz /= (float)gNumSamples;

	// Write the computed colour to the output.
	gOutput[texC] = float4(litColour, 1.0f);
#endif // 0


	// NOTE: Simple averaging of diffuse and specular colours approach. (Faster approach)
#if 0
	// Get the shading information from the GBuffer by sampling across the computed velocity across the given no. of samples.
	// NOTE: Clamping the viewport sampling coordinates to ensure they don't go out of range.
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f), specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	uint2 viewMinClamp = uint2((uint)gViewportWidthRange.x, (uint)gViewportHeightRange.x);
	uint2 viewMaxClamp = uint2((uint)gViewportWidthRange.y, (uint)gViewportHeightRange.y);
	for (float n = 1; n < gNumSamples; ++n)
	{
		int2 texOffset = (int2)(n*velocity);
		diffuse += gDiffuseMap.Load(uint3(clamp(texC + texOffset, viewMinClamp, viewMaxClamp), 0));

		float4 currSpecular = gSpecularMap.Load(uint3(clamp(texC + texOffset, viewMinClamp, viewMaxClamp), 0));
		if (currSpecular.y == 0.0f && currSpecular.z == 0.0f)	// The specular map is a grayscale value
			specular += float4(currSpecular.xxx, currSpecular.w);
		else
			specular += currSpecular;
	}

	// Divide the accummulated diffuse samples by the no. of samples to get the final sampled colours
	diffuse /= (float)gNumSamples;
	specular /= (float)gNumSamples;

	// Initialize the lit colour to the ambient lighting contribution.
	float3 litColour;
	if (nParallelLights > 0)
		litColour = gParallelLights[0].ambient.rgb * diffuse.rgb;
	else if (sNumShortlistedSpotLights > 0)
		litColour = gPointLights[sShortlistedPointLights[0]].ambient.rgb * diffuse.rgb;
	else if (sNumShortlistedSpotLights > 0)
		litColour = gSpotLights[sShortlistedSpotLights[0]].ambient.rgb * diffuse.rgb;
	//else if (nPointLights > 0)
	//	litColour = gPointLights[0].ambient.rgb * diffuse.rgb;
	//else if (nSpotLights > 0)
	//	litColour = gSpotLights[0].ambient.rgb * diffuse.rgb;
	else
		litColour = float3(0.0f, 0.0f, 0.0f);

	// Set the surface info for this pixel.
	SurfaceInfo surfaceInfo = { posW, normalW, diffuse, specular };

	// Accummulate the lighting contributions of all the parallel lights since they're not being culled,
	// as well as the shortlisted point and spot lights.
	[loop]
	for (uint i = 0; i < nParallelLights; ++i)
		litColour += ParallelLight(surfaceInfo, gParallelLights[i], gEyePosW);

	[loop]
	for (uint j = 0; j < sNumShortlistedPointLights; ++j)
		litColour += PointLight(surfaceInfo, gPointLights[sShortlistedPointLights[j]], gEyePosW);

	[loop]
	for (uint k = 0; k < sNumShortlistedSpotLights; ++k)
		litColour += SpotLight(surfaceInfo, gSpotLights[sShortlistedSpotLights[k]], gEyePosW);

	// Write the computed colour to the output.
	gOutput[texC] = float4(litColour, 1.0f);
#endif // 0

}