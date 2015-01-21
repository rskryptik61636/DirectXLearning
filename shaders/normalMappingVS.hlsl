// constant buffer per object
cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTrans;
	float4x4 gWVP;
	float4x4 gTexMtx;
};

// vertex shader input struct
struct VS_IN
{
	float3 posL	: POSITION;
	float3 normalL	: NORMAL;
	float2 texC	: TEXCOORD;
	float3 tangentL	: TANGENT;
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

VS_OUT normalMappingVS( VS_IN vIn )
{
	VS_OUT vOut;

	// transform the input point into homogenous clip space
	vOut.posH = mul(float4(vIn.posL, 1.0f), gWVP);

	// transform the input point into world space
	vOut.posW = mul(float4(vIn.posL, 1.0f), gWorld).xyz;

	// transform the input normal into world space 
	// (multiplying by inverse transpose of world transform as the normal is the plane perpendicular to that of the point's)
	vOut.normalW = mul(vIn.normalL, (float3x3)gWorldInvTrans);

	// transform the input tex coord by the tex transform
	vOut.texC = mul(float4(vIn.texC, 0.0f, 1.0f), gTexMtx).xy;

	// transform the input tangent into world space
	vOut.tangentW = mul(float4(vIn.tangentL, 1.0f), gWorld).xyz;

	return vOut;
}