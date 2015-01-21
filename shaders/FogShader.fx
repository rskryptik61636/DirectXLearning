// fog shader fx file
#include "lighthelper.fx"

cbuffer cbPerFrame
{
	Light gLight;
	float3 gEyePosW;
}

cbuffer cbPerObject
{
	float4x4 gWVPMatrix;
	float4x4 gWorldMatrix;
	float4x4 gTextureMatrix;
}

cbuffer cbFixed
{
	// hardcoding fog values, but might want to make this dynamic
	float gFogStart = 5.0f, gFogRange = 140.0f;
	float3 gFogColour = { 0.7f, 0.7f, 0.7f };
}

// define textures outside since non-numeric values cannot be added to a buffer
Texture2D gDiffuseMap;
Texture2D gSpecularMap;

SamplerState gTriLinearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VS_IN
{
	float3 posL		: POSITION;
	float3 normalL	: NORMAL;
	float2 texC	: TEXCOORD;
};

struct VS_OUT
{
	float4 posH		: SV_POSITION;
	float3 posW		: POSITION;
	float3 normalW	: NORMAL;
	float2 texC0	: TEXCOORD0;
	float2 texC1	: TEXCOORD1;
	float fogLerp	: FOG;
};

VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;

	// transform input point to homogenous clip space
	vOut.posH = mul( float4(vIn.posL, 1.0f), gWVPMatrix );

	// transform input point and normal to world space
	vOut.posW = mul( float4(vIn.posL, 1.0f), gWorldMatrix );
	vOut.normalW = mul( float4(vIn.normalL, 1.0f), gWorldMatrix );

	// set unmodified and modified texture coordinates
	vOut.texC0 = vIn.texC;
	vOut.texC1 = mul( float4(vIn.texC, 0.0f, 1.0f), gTextureMatrix );

	// compute fog linear interpolation factor;
	float fogDistance = distance(vOut.posW, gEyePosW);
	vOut.fogLerp = saturate( (fogDistance - gFogStart) / gFogRange );

	return vOut;
}

float4 PS(VS_OUT vIn)	: SV_Target
{
	// get alpha value from the diffuse colour map using unmodified texture coordinates for clipping purposes
	float clipAlpha = gDiffuseMap.Sample( gTriLinearSampler, vIn.texC0 ).a;

	// clip any pixels with clipAlpha - 0.1
	clip(clipAlpha - 0.1f);

	// get the diffuse and specular texture color
	float4 diffuseColour = gDiffuseMap.Sample( gTriLinearSampler, vIn.texC1 );
	float4 specularColour = gSpecularMap.Sample( gTriLinearSampler, vIn.texC1 );

	// map specularColour alpha value from [0,1] to [0,256]
	specularColour.a *= 256.0f;

	// normalize the normal 
	float3 normalW = normalize(vIn.normalW);

	// calculate the lighting for the current pixel
	SurfaceInfo surfaceInfo = { vIn.posW, normalW, diffuseColour, specularColour };
	float3 litColour = ParallelLight( surfaceInfo, gLight, gEyePosW );
	//litColour *= float3(1.0f, 1.0f, 1.0f);

	// calculate the fog colour by interpolating between the litColour 
	// -and the default gFogColour by fogLerp
	float3 foggyColour = lerp( litColour, gFogColour, vIn.fogLerp );

	// return the output colour
	//return float4(litColour, clipAlpha);
	return float4(foggyColour, clipAlpha);
}

technique10 FogTechnique
{
	pass P0
	{
		SetVertexShader( CompileShader(vs_4_0, VS()) );
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader(ps_4_0, PS()) );
	}
}