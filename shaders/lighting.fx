//=============================================================================
// lighting.fx by Frank Luna (C) 2008 All Rights Reserved.
//
// Transforms and lights geometry.
//=============================================================================

#include "PhongToonShaders.fx"
//#include "StraussShader.fx"
 
cbuffer cbPerFrame
{
	Light gLight;
	int gLightType; 
	int gShadingType;	// 0 = Phong, 1 = Toon, 2 = Strauss
	int gShaderImplType;	// 0 = vertex shader, 1 = pixel shader
	float3 gEyePosW;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWVP;
};

struct VS_IN
{
	float3 posL    : POSITION;
	float3 normalL : NORMAL;
	float4 diffuse : DIFFUSE;
	float4 spec    : SPECULAR;
};

struct VS_OUT
{
	float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
    float3 normalW : NORMAL;
    float4 diffuse : DIFFUSE;
    float4 spec    : SPECULAR;
	float3 color   : COLOR;
};

VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;
	
	// Transform to world space space.
	vOut.posW    = mul(float4(vIn.posL, 1.0f), gWorld);
	vOut.normalW = mul(float4(vIn.normalL, 0.0f), gWorld);
	
	// Transform to homogeneous clip space.
	vOut.posH = mul(float4(vIn.posL, 1.0f), gWVP);
	
	// Output vertex attributes for interpolation across triangle.
	vOut.diffuse = vIn.diffuse;
	vOut.spec    = vIn.spec;

	/* Phong shading - vertex shader implementation (begin) */
	if( !gShaderImplType )
	{
		SurfaceInfo v = {vOut.posW, vOut.normalW, vOut.diffuse, vOut.spec};
		//StraussParams sP = {0.7, 0.7, 0.7};

		if(gShadingType == 0)	// Phong
			vOut.color = PhongShading(v, gLight, gEyePosW, gLightType);
		if(gShadingType == 1)	// Toon
			vOut.color = ToonShading(v, gLight, gEyePosW, gLightType);
		/*if(gShadingType == 2)
			vOut.color = StraussShading(v, gLight, sP, gEyePosW, gLightType);*/
    
		/*if( gLightType == 0 ) // Parallel
		{
			vOut.color = ParallelLight(v, gLight, gEyePosW,gShadingType);
		}
		else if( gLightType == 1 ) // Point
		{
			vOut.color = PointLight(v, gLight, gEyePosW,gShadingType);
		}
		else // Spot
		{
			vOut.color = Spotlight(v, gLight, gEyePosW,gShadingType);
		}*/
	}
	/* Phong shading - vertex shader implementation (end) */

	return vOut;
}
 
float4 PS(VS_OUT pIn) : SV_Target
{
	if( !gShaderImplType)	
		return float4(pIn.color,pIn.diffuse.a);
	else
	{
		// Interpolating normal can make it not be of unit length so normalize it.
		pIn.normalW = normalize(pIn.normalW);
   
		SurfaceInfo v = {pIn.posW, pIn.normalW, pIn.diffuse, pIn.spec};
		//StraussParams sP = {0.7, 0.7, 0.7};

		float3 litColor;
		if(gShadingType == 0)	// Phong
			litColor = PhongShading(v, gLight, gEyePosW, gLightType);
		if(gShadingType == 1)	// Toon
			litColor = ToonShading(v, gLight, gEyePosW, gLightType);
		/*if(gShadingType == 2)
			litColor = StraussShading(v, gLight, sP, gEyePosW, gLightType);*/

		/*if( gLightType == 0 ) // Parallel
		{
			litColor = ParallelLight(v, gLight, gEyePosW,gShadingType);
		}
		else if( gLightType == 1 ) // Point
		{
			litColor = PointLight(v, gLight, gEyePosW,gShadingType);
		}
		else // Spot
		{
			litColor = Spotlight(v, gLight, gEyePosW,gShadingType);
		}*/

		return float4(litColor, pIn.diffuse.a);
	}
}

technique10 LightTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}



