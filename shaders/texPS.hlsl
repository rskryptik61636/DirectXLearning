#include "lighthelper.fx"

#define MAX_PARALLEL_LIGHTS 50
#define MAX_POINT_LIGHTS 50
#define MAX_SPOT_LIGHTS 50

cbuffer cbPerFrame
{
	Light gParallelLights[MAX_PARALLEL_LIGHTS];
	int nParallelLights;

	Light gPointLights[MAX_POINT_LIGHTS];
	int nPointLights;

	Light gSpotLights[MAX_SPOT_LIGHTS];
	int nSpotLights;

	float3 gEyePosW;
	float gCurrTime;
};
 
// @TODO: remove when done testing
//cbuffer cbPerFrame
//{
//	Light gLight;
//	float3 gEyePosW;
//	float gCurrTime;
//};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;
Texture2D gSpecMap;

SamplerState gTriLinearSam;

struct VS_OUT
{
	float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
    float3 normalW : NORMAL;
    float2 texC    : TEXCOORD;
};

float4 texPS(VS_OUT pIn) : SV_Target
{
	// Get materials from texture maps.
	float4 diffuseRGB = gDiffuseMap.Sample( gTriLinearSam, pIn.texC );
	float4 diffuse = diffuseRGB;
	float4 spec    = gSpecMap.Sample( gTriLinearSam, pIn.texC );
	
	// Map [0,1] --> [0,256]
	spec.a *= 256.0f;
	
	// Interpolating normal can make it not be of unit length so normalize it.
    float3 normalW = normalize(pIn.normalW);
    
	// Compute the lit color for this pixel.
    SurfaceInfo v = {pIn.posW, normalW, diffuse, spec};
	float3 ambient = float3(0.0f, 0.0f, 0.0f);
	if (nParallelLights > 0)
		ambient = gParallelLights[0].ambient.xyz;
	else if (nPointLights > 0)
		ambient = gPointLights[0].ambient.xyz;
	else if (nSpotLights > 0)
		ambient = gSpotLights[0].ambient.xyz;
	float3 litColor = (diffuseRGB.rgb * ambient.xyz);// +ParallelLight(v, gLight, gEyePosW);	@ TODO: remove when done testing

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
		litColor += SpotLight(v, gSpotLights[i], gEyePosW);
	}

	// set the alpha component of the computed light color as the diffuse alpha component
	return float4(litColor, diffuse.a);
}