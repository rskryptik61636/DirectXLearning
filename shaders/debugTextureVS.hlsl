// Vertex shader which is used to debug a texture by drawing it onto a surface

#include "debugTextureCommon.hlsli"

// Constant buffer which contains per object constant
cbuffer cbPerObject
{
	float4x4 gWVP;	// world-view-projection matrix
	float4x4 gTexMtx;	// texture space matrix
};

VS_OUT debugTextureVS( VS_IN vIn )
{
	// Initialize the output struct
	VS_OUT vOut;

	// Transform the input point from local space to homogeneous clip space
	vOut.posH = mul(float4(vIn.posL, 1.0f), gWVP);

	// Transform the input texture coordinate to texture space
	vOut.texC = mul(float4(vIn.texC, 0.0f, 1.0f), gTexMtx).xy;

	return vOut;
}