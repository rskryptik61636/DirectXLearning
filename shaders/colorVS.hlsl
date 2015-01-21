//=============================================================================
// color.fx by Frank Luna (C) 2008 All Rights Reserved.
//
// Transforms and colors geometry.
//=============================================================================

cbuffer cbPerObject
{
	float4x4 gWVP;
};

void colorVS(float3 iPosL  : POSITION,
	float4 iColor : COLOR,
	out float4 oPosH : SV_POSITION,
	out float4 oColor : COLOR)
{
	// Transform to homogeneous clip space.
	oPosH = mul(float4(iPosL, 1.0f), gWVP);

	// Just pass vertex color into the pixel shader.
	oColor = iColor;
}