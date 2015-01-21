// Vertex shader which is used to build the shadow map of a scene from the perspective of a single given light

// Constant buffer containing the world view projection matrix of the light's shadow frustum
cbuffer cbPerObject
{
	float4x4 gWVP;
};

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

VS_OUT buildShadowMapVS( VS_IN vIn )
{
	// Initialize the output param
	VS_OUT vOut;

	vOut.posH = mul(float4(vIn.posL, 1.0), gWVP);	// convert the point from object to the homogeneous clip space defined by the light's shadow frustum
	vOut.texC = vIn.texC;	// pass the tex coord as is

	return vOut;	
}