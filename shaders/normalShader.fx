// geometry shader for the normal debug draw effect

#include "PhongToonShaders.fx"

cbuffer cbPerFrame
{
	Light gLight;
	int gLightType; 
	//int gShadingType;	// 0 = Phong, 1 = Toon, 2 = Strauss
	//int gShaderImplType;	// 0 = vertex shader, 1 = pixel shader
	float3 gEyePosW;
	float gNormalLength;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWVP;
};

// input/output struct of vertex shader
struct VS_OUT
{
	float3 posL	:		POSITION0;		// object-space position
	float3 normalL:		NORMAL0;			// object-space normal
	float4 diffuse:		DIFFUSE0;		// diffuse colour
	float4 specular:	SPECULAR0;		// specular colour
};

// output struct of geometry shader
struct GS_OUT
{
	float4 posH		:	SV_POSITION0;	// normalized clip-space position
	float3 posW		:	POSITION0;		// world-space position
	float3 normalW	:	NORMAL0;			// world-space normal
	float4 diffuse	:	DIFFUSE0;		// diffuse colour
	float4 specular	:	SPECULAR0;		// specular colour
};

// vertex shader - point just passes through
VS_OUT VS(VS_OUT vIn)
{
	VS_OUT vOut;
	vOut = vIn;
	return vOut;
}

// geometry shader - converts each point into a normal for debugging
[maxvertexcount(2)]
void GS(point VS_OUT gIn[1], inout LineStream<GS_OUT> normals)
{
	// init the output primitive
	GS_OUT gOut;
	gOut.diffuse = gIn[0].diffuse;
	gOut.specular = gIn[0].specular;

	// compute a line along the normal of length gNormalLength
	gOut.normalW = normalize(mul(gIn[0].normalL, (float3x3)gWorld));
	gOut.posW = mul(float4(gIn[0].posL, 1.0f), gWorld).xyz;
	gOut.posH = mul(float4(gIn[0].posL, 1.0f), gWVP);
	normals.Append(gOut);	// first point - transformed input point

	// second point - transformed input point translated along normal by gNormalLength
	float3 endPoint = gIn[0].posL + gIn[0].normalL * gNormalLength;
	gOut.posW = mul(float4(endPoint, 1.0f), gWorld).xyz;
	gOut.posH = mul(float4(endPoint, 1.0f), gWVP);
	normals.Append(gOut);
}

// helper function to compute the normal of a triangle
float3 computeNormal(float3 p[3])
{
	float3 e0 = p[1] - p[0];
		float3 e1 = p[2] - p[0];
		float3 normal = normalize(cross(e0, e1));
		return normal;
}

// geometry shader - computes each triangle's normal for debugging
[maxvertexcount(2)]
void GS2(triangle VS_OUT gIn[3], inout LineStream<GS_OUT> normals)
{
	// init the output primitive
	GS_OUT gOut;
	gOut.diffuse = gIn[0].diffuse;
	gOut.specular = gIn[0].specular;

	// compute the normal of the input triangle
	float3 pts[3] = { gIn[0].posL, gIn[1].posL, gIn[2].posL };
	float3 normal = computeNormal(pts);

	// compute a line along the normal of length gNormalLength
	float3 startPoint = (gIn[0].posL + gIn[1].posL + gIn[2].posL) / float3(3.0f, 3.0f, 3.0f);
	gOut.normalW = normalize(mul(normal, (float3x3)gWorld));
	gOut.posW = mul(float4(startPoint, 1.0f), gWorld).xyz;
	gOut.posH = mul(float4(startPoint, 1.0f), gWVP);
	normals.Append(gOut);	// first point - transformed input point

	// second point - transformed input point translated along normal by gNormalLength
	float3 endPoint = startPoint + normal * gNormalLength;
		gOut.posW = mul(float4(endPoint, 1.0f), gWorld).xyz;
	gOut.posH = mul(float4(endPoint, 1.0f), gWVP);
	normals.Append(gOut);
}

// pixel shader - shades the point based on the given light and its type
float4 PS(GS_OUT pIn)	: SV_Target
{
	//// normalize the input normal as it would have been interpolated
	//pIn.normalW = normalize(pIn.normalW);

	//// create SurfaceInfo struct and perform Phong shading
	//SurfaceInfo surfInfo = { pIn.posW, pIn.normalW, pIn.diffuse, pIn.specular };
	//float3 litColour = PhongShading(surfInfo, gLight, gEyePosW, gLightType);

	//// return final colour
	//return float4(litColour, 1.0f);

	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

// define the technique and pass
technique10 NormalsTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		/*SetGeometryShader(CompileShader(gs_5_0, GS()));*/
		SetGeometryShader(CompileShader(gs_5_0, GS2()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}