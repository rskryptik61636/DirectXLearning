// Basic shading of texture mapped objects.

#include "dataTypes.hlsli"
#include "shadingUtils.hlsli"

// Constant buffer per frame.
cbuffer cbPerFrame : register(b0)
{
	uint nParallelLights;
	uint nPointLights;
	uint nSpotLights;

	float3 gEyePosW;
}

// Parallel, point and spot lights structured buffers.
StructuredBuffer<Light> gParallelLights : register(t0);
StructuredBuffer<Light> gPointLights : register(t1);
StructuredBuffer<Light> gSpotLights : register(t2);

// Diffuse and specular color maps.
Texture2D gDiffuseMap : register(t3);
Texture2D gSpecMap : register(t4);

// Texture sampler.
SamplerState gTexSam : register(s0);

float4 basicShadingPS(COMPLETE_VS_OUT pIn) : SV_TARGET
{
	// Get materials from texture maps.
	float4 diffuse = gDiffuseMap.Sample( gTexSam, pIn.texC );
	//float4 diffuse = diffuseRGB;
	float4 specular    = gSpecMap.Sample( gTexSam, pIn.texC );
	
	// Map [0,1] --> [0,256]
	specular.a *= 256.0f;
	
	// Interpolating normal can make it not be of unit length so normalize it.
    float3 normalW = normalize(pIn.normalW);

	// Set the surface info for this pixel.
	SurfaceInfo surfaceInfo = { pIn.posW, normalW, diffuse, specular };

	// Accummulate the lighting over all the given lights.
	float3 litColour = 
		accummulateLighting(
			gParallelLights,
			nParallelLights,
			gPointLights,
			nPointLights,
			gSpotLights,
			nSpotLights,
			surfaceInfo,
			gEyePosW);

	// set the alpha component of the computed light color as the diffuse alpha component
	return float4(litColour, diffuse.a);
}