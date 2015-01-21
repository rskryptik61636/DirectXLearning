#include "deferredShadingCommon.hlsli"

// constant buffer per object
cbuffer cbPerObject	: register(b0)
{
	float4x4 gWorld;
	float4x4 gWorldInvTrans;
	float4x4 gWVP;
	float4x4 gTexMtx;
};

VS_OUT deferredShadingVS( VS_IN vIn )
{
	// Init the output struct instance
	VS_OUT vOut;

	// Compute the position in homogenous clip space
	vOut.posH = mul(float4(vIn.posL, 1.0f), gWVP);

	// Compute the position in world space
	vOut.posW = mul(float4(vIn.posL, 1.0f), gWorld).xyz;

	// Compute the normal in world space
	vOut.normalW = mul(vIn.normalL, (float3x3)gWorldInvTrans);

	// Compute the tangent in world space
	vOut.tangentW = mul(vIn.tangentL, (float3x3)gWorld);

	// Compute the binormal in world space
	vOut.binormalW = mul(vIn.binormalL, (float3x3) gWorld);

	// Compute the transformed texture coordinate
	vOut.texC = mul(float4(vIn.texC, 0.0f, 1.0f), gTexMtx).xy;

	return vOut;
}