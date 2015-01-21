// Vertex shader which is used in the second pass of the shadow mapping effect

#include "renderShadowMapCommon.hlsli"

// Constant buffer per object
cbuffer cbPerObject	: register(b0)
{
	float4x4 gWVP;
	float4x4 gWorld;
	float4x4 gWorldInvTrans;
	float4x4 gLightViewProjTangent[MAX_SPOT_LIGHTS];	// @TODO: consider passing in no. of lights as a param to this buffer as well
	float4x4 gTexMtx;
};

VS_OUT renderShadowMapVS(VS_IN vIn)
{
	// Initialize the output param
	VS_OUT vOut;

	// Transform the point from object space to homogenous clip space
	vOut.posH = mul(float4(vIn.posL, 1.0f), gWVP);

	// Transform the point from object space to world space
	vOut.posW = mul(float4(vIn.posL, 1.0f), gWorld).xyz;

	// Transform the normal from object space to world space
	vOut.normalW = mul(vIn.normalL, (float3x3)gWorldInvTrans);

	// Transform the tangent from object space to world space
	vOut.tangentW = mul(vIn.tangentL, (float3x3)gWorld);

	// Transform the point from local space to each input light's texture space
	for (int i = 0; i < MAX_SPOT_LIGHTS; ++i)
		vOut.posLightT[i] = mul(float4(vIn.posL, 1.0f), gLightViewProjTangent[i]);

	// Transform the texture coordinate
	vOut.texC = mul(float4(vIn.texC, 0.0f, 1.0f), gTexMtx).xy;

	return vOut;
}