// Header file which contains common structs and resources used in deferred shading

// Vertex shader input struct
struct VS_IN
{
	float3 posL	: POSITION;
	float3 normalL	: NORMAL;
	float2 texC	: TEXCOORD;
	float3 tangentL	: TANGENT;
	float3 binormalL : BINORMAL;
};

// Vertex shader output struct
struct VS_OUT
{
	float4 posH	: SV_POSITION;
	float3 posW	: POSITION;
	float3 normalW	: NORMAL;
	float2 texC	: TEXCOORD;
	float3 tangentW	: TANGENT;
	float3 binormalW : BINORMAL;
};

// Pixel shader output struct
struct PS_OUT
{
	float4 posW	: SV_TARGET0;
	float4 normalW	: SV_TARGET1;
	float4 diffuse	: SV_TARGET2;
	float4 specular	: SV_TARGET3;
};