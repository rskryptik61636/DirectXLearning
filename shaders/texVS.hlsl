#include "lighthelper.fx"

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWVP; 
	float4x4 gTexMtx;
};

struct VS_IN
{
	float3 posL    : POSITION;
	float3 normalL : NORMAL;
	float2 texC    : TEXCOORD;
};

struct VS_OUT
{
	float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
    float3 normalW : NORMAL;
    float2 texC    : TEXCOORD;
};
 
VS_OUT texVS(VS_IN vIn)
{
	VS_OUT vOut;

	// multiple texture coordinates by gCurrTime to get a rotating fireball
	//vIn.texC *= float2(sin(gCurrTime*180.0f/PI),cos(gCurrTime*180.0f/PI));
	
	// Transform to world space space.
	vOut.posW    = mul(float4(vIn.posL, 1.0f), gWorld);
	vOut.normalW = mul(float4(vIn.normalL, 0.0f), gWorld);
		
	// Transform to homogeneous clip space.
	vOut.posH = mul(float4(vIn.posL, 1.0f), gWVP);
	
	// Output vertex attributes for interpolation across triangle.
	vOut.texC  = mul(float4(vIn.texC, 0.0f, 1.0f), gTexMtx);
	
	return vOut;
}