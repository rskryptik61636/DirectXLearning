// Vertex shader input struct
struct SkyboxVSIn
{
	float3 posL	: POSITION;
};

// Vertex shader output struct
struct SkyboxVSOut
{
	float4 posH	: SV_Position;
	float3 texC : TEXCOORD;
};