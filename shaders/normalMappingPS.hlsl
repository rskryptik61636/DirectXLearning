// include the lighting helper
#include "lighthelper.fx"

// constants for max no. of lights
#define MAX_PARALLEL_LIGHTS 4
#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 4

// constant buffer for lighting
cbuffer cbPerFrame
{
	Light gParallelLights[MAX_PARALLEL_LIGHTS];	// array of max no. of parallel lights
	int nParallelLights;	// actual no. of parallel lights passed

	Light gPointLights[MAX_POINT_LIGHTS];	// array of max no. of point lights
	int nPointLights;	// actual no. of point lights passed

	Light gSpotLights[MAX_SPOT_LIGHTS];	// array of max no. of spot lights
	int nSpotLights;	// actual no. of spot lights passed

	float3 gEyePosW;	// camera position in world space
};

// vertex shader output struct
struct VS_OUT
{
	float4 posH	: SV_POSITION;
	float3 posW	: POSITION;
	float3 normalW	: NORMAL;
	float2 texC	: TEXCOORD;
	float3 tangentW	: TANGENT;
};

Texture2D gDiffuseMap;	// diffuse lighting texture map
Texture2D gNormalMap;	// normal map
Texture2D gSpecMap;	// specular light map

SamplerState gTriLinearSam;	// linear filtering for minification, magnification and mipmapping

float4 normalMappingPS(VS_OUT pIn) : SV_TARGET
{
	// sample the diffuse lighting texture map
	float4 diffuse = gDiffuseMap.Sample(gTriLinearSam, pIn.texC);

	// sample the specular light map and map from [0,1] to [0,256]
	float4 spec = gSpecMap.Sample(gTriLinearSam, pIn.texC);
	spec.a *= 256.0f;

	// compute the perturbed normal from the normal map
	float3 normalW = ComputePerturbedNormalFromNormalMap(gNormalMap, gTriLinearSam, pIn.texC, pIn.normalW, pIn.tangentW);

	// compute the lighting contributions from all the input lights
	SurfaceInfo v = { pIn.posW, normalW, diffuse, spec };
	//float3 litColor = float3(0.0f, 0.0f, 0.0f);	// @TODO: remove when done testing

	float3 ambient = float3(0.0f, 0.0f, 0.0f);
	if (nParallelLights > 0)
		ambient = gParallelLights[0].ambient.xyz;
	else if (nPointLights > 0)
		ambient = gPointLights[0].ambient.xyz;
	else if (nSpotLights > 0)
		ambient = gSpotLights[0].ambient.xyz;
	float3 litColor = (diffuse.rgb * ambient.xyz);// +ParallelLight(v, gLight, gEyePosW);	@ TODO: remove when done testing

	[loop]
	for (int i = 0; i < nParallelLights; ++i)
	{		
		litColor += ParallelLight(v, gParallelLights[i], gEyePosW);
	}
	
	[loop]
	for (int i = 0; i < nPointLights; ++i)
	{
		litColor += PointLight(v, gPointLights[i], gEyePosW);
	}

	[loop]
	for (int i = 0; i < nSpotLights; ++i)
	{
		litColor += Spotlight(v, gSpotLights[i], gEyePosW);
	}

	// set the alpha component of the computed light color as the diffuse alpha component
	return float4(litColor, diffuse.a);
}