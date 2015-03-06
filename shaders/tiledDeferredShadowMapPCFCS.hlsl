// Simple shadow mapping using tiled deferred shading.

#include "tiledDeferredShadingCommon.hlsli"

// Per frame constants
cbuffer cbPerFrame : register(b0)
{
	float3 gEyePosW;
	uint gTapSize;	// PCF tap size. Should be in [1,5].
	float2 gShadowMapSize;	// (width,height) of the shadow map.
};

Texture2DArray<float> gShadowMap	: register(t7);	// Shadow maps for each of the spot lights.
StructuredBuffer<float4x4> gSpotLightsShadowTransforms : register(t8);	// View projection tangent space transformations for each of the spots lights.

SamplerComparisonState gShadowSam	: register(s0);	// Used for sampling the shadow map and performing PCF filtering

[numthreads(TILE_DIMS, TILE_DIMS, 1)]
void tiledDeferredShadowMapPCFCS(uint3 groupId			: SV_GroupID,
									uint3 dispatchThreadId	: SV_DispatchThreadID,
									uint3 groupThreadId		: SV_GroupThreadID
									)
{
	// Apparently a compiler bug on Fermi causes SV_GroupIndex to return the incorrect group index,
	// which is why it has to be computed manually.
	// TODO: Verify this is still the case as the demo was written 4 years ago.
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

	// Read the current world-space normal, diffuse colour and specular colour;
	float3 normalW = gNormalMap.Load(uint3(texC, 0)).xyz;
	float4 diffuse = gDiffuseMap.Load(uint3(texC, 0));
	float4 specular = gSpecularMap.Load(uint3(texC, 0));
	if (specular.y == 0.0f && specular.z == 0.0f)	// Account for the specular value being grayscale.
		specular = float4(specular.xxx, specular.w);

	// Initialize the lit colour to the ambient lighting contribution.
	float3 litColour;
	if (nParallelLights > 0)
		litColour = gParallelLights[0].ambient.rgb * diffuse.rgb;
	else if (sNumShortlistedSpotLights > 0)
		litColour = gPointLights[sShortlistedPointLights[0]].ambient.rgb * diffuse.rgb;
	else if (sNumShortlistedSpotLights > 0)
		litColour = gSpotLights[sShortlistedSpotLights[0]].ambient.rgb * diffuse.rgb;
	else
		litColour = float3(0.0f, 0.0f, 0.0f);

	// Set the surface info for this pixel.
	SurfaceInfo surfaceInfo = { posW, normalW, diffuse, specular };

	// Accummulate the lighting contributions of all the parallel lights since they're not being culled,
	// as well as the shortlisted point and spot lights.
	// Perform the shadow mapping tests for all the spot lights in the scene.
	[loop]
	for (uint i = 0; i < nParallelLights; ++i)
		litColour += ParallelLight(surfaceInfo, gParallelLights[i], gEyePosW);

	[loop]
	for (uint j = 0; j < sNumShortlistedPointLights; ++j)
		litColour += PointLight(surfaceInfo, gPointLights[sShortlistedPointLights[j]], gEyePosW);

	[loop]
	for (uint k = 0; k < sNumShortlistedSpotLights; ++k)
	{
		// Compute the light's texture space position and normalize as it would have been modified during interpolation
		float4 currTex = mul(float4(posW, 1.0f), gSpotLightsShadowTransforms[sShortlistedSpotLights[k]]); //pIn.posLightT[i];
		currTex.xyz /= currTex.w;

		// Only shade the current point if it lies within the light's clip volume.
		if (inRange(currTex.x, currTex.y))
		{
			// Extract the depth value from the shadow map.
			float pointDepth = currTex.z; 

			// Compute the PCF shadow interpolation factor based on the no. of taps.
			// The min/max limits of the PCF sampling offsets are computed as follows:
			// a) x/y tap length = sqrt(tap size)
			// b) min/max limit = +-(tap length/2 - 0.5) will ensure that the center is 0
			float shadowFactor;
			//float tapLength = sqrt((float)gTapSize);
			float tapCenter = 0.5f*(float)gTapSize - 0.5f;
			float minLimit = -tapCenter, maxLimit = tapCenter;
			[loop]
			for (float y = minLimit; y <= maxLimit; ++y)
			{
				[loop]
				for (float x = minLimit; x <= maxLimit; ++x)
				{
					shadowFactor += gShadowMap.SampleCmpLevelZero(gShadowSam, float3(currTex.xy + pcfOffset(x, y, gShadowMapSize), sShortlistedSpotLights[k]), currTex.z);
				}
			}
			shadowFactor /= pow((float)gTapSize, 2.0f);
		
			litColour += shadowFactor * SpotLight(surfaceInfo, gSpotLights[sShortlistedSpotLights[k]], gEyePosW);
		}		
	}		

	// Write the computed colour to the output.
	gOutput[texC] = float4(litColour, 1.0f);

}