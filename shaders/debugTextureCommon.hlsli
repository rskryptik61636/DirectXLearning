// Header file which contains common structs used by the debug texture shaders

// Vertex shader input
struct VS_IN
{
	float3 posL	: POSITION;	// object space position
	float3 normalL	: NORMAL;
	float2 texC	: TEXCOORD;
	float3 tangentL	: TANGENT;
};

// Vertex shader output
struct VS_OUT
{
	float4 posH	: SV_POSITION;	// homogeneous clip space position
	float2 texC	: TEXCOORD;		// transformed texture coordinate
};