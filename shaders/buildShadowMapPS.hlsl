// Dummy pixel shader to build the shadow map of a scene from the perspective of a single given light

// Vertex shader output
struct VS_OUT
{
	float4 posH	: SV_POSITION;	// homogeneous clip space position
	float2 texC	: TEXCOORD;		// transformed texture coordinate
};

float4 buildShadowMapPS(VS_OUT pIn) : SV_TARGET
{
	// Compute the depth value of the current pixel
	float depth = pIn.posH.z / pIn.posH.w;

	return float4(depth, depth, depth, 1.0f);
}