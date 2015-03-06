// Texture mapping using deferred shading.

#include "deferredShadingCommon.hlsli"

#include "deferredShadingCommonPS.hlsli"

// Constant buffer per frame
cbuffer cbPerFrame	: register(b0)
{
	float3 gEyePosW;
};

// GBuffer map sampler.
SamplerState gBufferSampler : register(s0);

float4 deferredTexPS(VS_OUT vIn) : SV_TARGET
{
	// Get the geometry information from the GBuffer.
	float3 posW = gPositionMap.Sample(gBufferSampler, vIn.texC).xyz;
	float3 normalW = gNormalMap.Sample(gBufferSampler, vIn.texC).xyz;

	// Get the shading information from the GBuffer.
	float4 diffuse = gDiffuseMap.Sample(gBufferSampler, vIn.texC);
	float4 specular = gSpecularMap.Sample(gBufferSampler, vIn.texC);
	if (specular.y == 0.0f && specular.z == 0.0f)	// The specular map value is a grayscale value
		specular = float4(specular.x, specular.x, specular.x, specular.w);

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
}