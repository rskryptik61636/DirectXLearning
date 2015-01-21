// Simple shadow mapping using deferred shading, no filtering involved.

#include "deferredShadingCommon.hlsli"

#include "deferredShadingCommonPS.hlsli"

// Constant buffer for lighting (considering only one spot light for now)
cbuffer cbPerFrame	: register(b0)
{
	float3 gEyePosW;	// eye position in world space
};

Texture2DArray<float> gShadowMap	: register(t7);	// Shadow maps for each of the spot lights.
StructuredBuffer<float4x4> gSpotLightsShadowTransforms : register(t8);	// View projection tangent space transformations for each of the spots lights.

SamplerState gBufferSampler	: register(s0);	// Used for sampling the GBuffer textures
SamplerState gShadowSam	: register(s1);	// Used for sampling the shadow map

float4 deferredShadowMapSimplePS(VS_OUT pIn) : SV_TARGET
{
	// Get the geometry information from the GBuffer.
	float3 posW = gPositionMap.Sample(gBufferSampler, pIn.texC).xyz;
	float3 normalW = gNormalMap.Sample(gBufferSampler, pIn.texC).xyz;

	// Get the shading information from the GBuffer.
	float4 diffuse = gDiffuseMap.Sample(gBufferSampler, pIn.texC);
	float4 specular = gSpecularMap.Sample(gBufferSampler, pIn.texC);
	if (specular.y == 0.0f && specular.z == 0.0f)	// The specular map value is a grayscale value
		specular = float4(specular.x, specular.x, specular.x, specular.w);

	// Get the no. of lights of each type in the scene. (NOTE: Only handling spot lights for now)
	uint nParallelLights, nPointLights, nSpotLights, dummy;
	gParallelLights.GetDimensions(nParallelLights, dummy);
	gPointLights.GetDimensions(nPointLights, dummy);
	gSpotLights.GetDimensions(nSpotLights, dummy);

	// Compute the lit colour for this pixel.

	// Initialize the lit colour to the ambient lighting contribution.
	float3 litColour;
	litColour = gSpotLights[0].ambient.rgb * diffuse.rgb;	

	// Set the surface info for this pixel.
	SurfaceInfo surfaceInfo = { posW, normalW, diffuse, specular };

	// Accumulate lighting contributions of spot lights while accounting for shadows.
	[loop]	// Tells HLSL to give preference to the for loop's constructs: http://msdn.microsoft.com/en-us/library/bb313978%28v=xnagamestudio.31%29.aspx
	for (uint i = 0; i < nSpotLights; ++i)
	{
		// Compute the light's texture space position and normalize as it would have been modified during interpolation
		float4 currTex = mul(float4(posW, 1.0f), gSpotLightsShadowTransforms[i]); //pIn.posLightT[i];
			currTex.xyz /= currTex.w;

		// Only shade the current point if it lies within the light's clip volume.
		if (inRange(currTex.x, currTex.y))
		{
			// Compare the depth of the light's texture space position against its corresponding depth value in the shadow map,
			// and proceed with the lighting calculations iff the point is not in shadow.
			float pointDepth = currTex.z, lightDepth = gShadowMap.SampleLevel(gShadowSam, float3(currTex.xy, i), 0).r;
			if (pointDepth > lightDepth)	// in shadow, return the ambient colour
			{
				//litColor += (gSpotLights[i].ambient.rgb * diffuse.rgb);
				continue;
			}

			// Compute the final pixel colour
			SurfaceInfo material = { posW, normalW, diffuse, specular };

			litColour += Spotlight(material, gSpotLights[i], gEyePosW);
		}
	}

	// Accummulate lighting contributions of parallel and point lights without accounting for shadows.
	[loop]
	for (uint i = 0; i < nParallelLights; ++i)
		litColour += ParallelLight(surfaceInfo, gParallelLights[i], gEyePosW);

	[loop]
	for (uint j = 0; j < nPointLights; ++j)
		litColour += PointLight(surfaceInfo, gPointLights[j], gEyePosW);

	return float4(litColour.rgb, 1.0f);
}