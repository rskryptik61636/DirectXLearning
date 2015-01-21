// ccp of clip.fx for my own personal gratification

#include "lighthelper.fx"

cbuffer cbPerFrame
{
	Light gLight;
	float3 gEyePosW;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWVP;
	float4x4 gTexMtx;
};

// textures are non-numeric and cannot be added to a cbuffer
Texture2D gDiffuseMap;
Texture2D gSpecMap;

SamplerState gTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VS_IN
{
	float3 posL		:	POSITION;
	float3 normalL	:	NORMAL;
	float2 texC		:	TEXCOORD;
};

struct VS_OUT
{
	float4 posH		:	SV_POSITION;
	float3 posW		:	POSITION;
	float3 normalW	:	NORMAL;
	float2 texC0	:	TEXCOORD0;
	float2 texC1	:	TEXCOORD1;
};

VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;
	
	// transform to world space
	vOut.posW = mul(float4(vIn.posL, 1.0f), gWorld);
	vOut.normalW = mul(float4(vIn.normalL, 1.0f), gWorld);
	
	// transform to homogeneous clip space
	vOut.posH = mul(float4(vIn.posL, 1.0f), gWVP);
	
	// output vertex attributes for interpolation across the triangle
	vOut.texC0 = vIn.texC;
	vOut.texC1 = mul(float4(vIn.texC, 0.0f, 1.0f), gTexMtx);
	
	return vOut;
}

float4 PS(VS_OUT pIn)	:	SV_Target
{
	// get the alpha value of the current pixel
	float alpha = gDiffuseMap.Sample(gTriLinearSam, pIn.texC0).a;
	
	// reject any pixel who's alpha value is < (alpha-0.25)
	clip(alpha - 0.25f);
	
	// sample the diffuse and specular maps
	float4 diffuseColour = gDiffuseMap.Sample(gTriLinearSam, pIn.texC1);
	float4 specularColour = gSpecMap.Sample(gTriLinearSam, pIn.texC1);
	
	// map alpha value [0,1] --> [0,256]
	specularColour.a *= 256.0f;

	// normalize normal as interpolation might cause it not to be a unit vector
	float3 normalW = normalize(pIn.normalW);
	
	SurfaceInfo surfaceInfo = { pIn.posW, normalW, diffuseColour, specularColour };
	float3 currPixelColour = ParallelLight(surfaceInfo, gLight, gEyePosW);
	
	return float4(currPixelColour, alpha);
}

technique10 ClipTech
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
	}
}