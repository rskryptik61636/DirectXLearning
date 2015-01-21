// Header file which contains common structs used by the renderShadowMap shaders

// Max no. of lights in the scene
#define MAX_SPOT_LIGHTS 4

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
	float3 posW : POSITION;		// world space position
	float3 normalW : NORMAL;	// normal in world space
	float3 tangentW : TANGENT;	// tangent in world space
	float4 posLightT[MAX_SPOT_LIGHTS] : POSITIONT;	// light's texture space position
	float2 texC	: TEXCOORD;		// transformed texture coordinate
};