// geometry shader for the LitSphere

#include "PhongToonShaders.fx"

cbuffer cbPerFrame
{
	Light gLight;
	int gLightType; 
	//int gShadingType;	// 0 = Phong, 1 = Toon, 2 = Strauss
	//int gShaderImplType;	// 0 = vertex shader, 1 = pixel shader
	float3 gEyePosW;
	float gCurrTime;
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

// vertex shader - just send input to output
VS_OUT VS(VS_OUT vIn)
{
	VS_OUT vOut;
	vOut = vIn;
	return vOut;
}

// helper function to compute the normal of a triangle
float3 computeNormal(float3 p[3])
{
	float3 e0 = p[1] - p[0];
	float3 e1 = p[2] - p[0];
	float3 normal = normalize(cross(e0, e1));
	return normal;
}

// geometry shader - subdivide icosahedron triangles to create a sphere
[maxvertexcount(8)]
void GS(triangle VS_OUT gIn[3], inout TriangleStream<GS_OUT> triStream)
{
	GS_OUT gOut;	// create output variable
	
	// copy over diffuse and specular colours
	gOut.diffuse = gIn[0].diffuse;
	gOut.specular = gIn[0].specular;

	// compute normal
	float3 e0 = gIn[1].posL - gIn[0].posL;
	float3 e1 = gIn[2].posL - gIn[0].posL;
	float3 normal = normalize( cross(e0, e1) );
	gOut.normalW = mul(float4(normal, 0.0f), gWorld);	// transform to world-space

	// test to see if the input triangles themselves come out right
	gOut.posW = mul(float4(gIn[0].posL, 1.0f), gWorld).xyz;
	gOut.posH = mul(float4(gIn[0].posL, 1.0f), gWVP);
	//gOut.normalW = mul(normalize(gIn[0].posL), (float3x3)gWorld);
	triStream.Append(gOut);

	gOut.posW = mul(float4(gIn[1].posL, 1.0f), gWorld).xyz;
	gOut.posH = mul(float4(gIn[1].posL, 1.0f), gWVP);
	//gOut.normalW = mul(normalize(gIn[1].posL), (float3x3)gWorld);
	triStream.Append(gOut);

	gOut.posW = mul(float4(gIn[2].posL, 1.0f), gWorld);
	gOut.posH = mul(float4(gIn[2].posL, 1.0f), gWVP);
	//gOut.normalW = mul(normalize(gIn[2].posL), (float3x3)gWorld);
	triStream.Append(gOut);
	
	//// compute edge-midpoints and project onto a bounding sphere of radius 2
	//float3 m0 = 0.5f * (gIn[0].posL + gIn[1].posL);
	//float3 m1 = 0.5f * (gIn[1].posL + gIn[2].posL);
	//float3 m2 = 0.5f * (gIn[2].posL + gIn[0].posL);
	//m0 = normalize(m0) /** float3(12.0f, 12.0f, 12.0f)*/;
	//m1 = normalize(m1) /** float3(12.0f, 12.0f, 12.0f)*/;
	//m2 = normalize(m2) /** float3(12.0f, 12.0f, 12.0f)*/;

	//// generate sub-triangles vertices list
	//float3 v[6];
	//v[0] = gIn[0].posL;
	//v[1] = m0;
	//v[2] = m2;
	//v[3] = m1;
	//v[4] = gIn[2].posL;
	//v[5] = gIn[1].posL;
	//
	//// split the triangles bottom 3 subtriangles followed by the top subtriangle
	//[unroll]
	//for(int i = 0; i < 5; ++i)
	//{
	//	gOut.posW = mul( float4(v[i], 1.0f), gWorld );	// transform current vertex to world-space
	//	gOut.posH = mul( float4(v[i], 1.0f), gWVP );	// transform current vertex to homogenized clip-space
	//	gOut.normalW = mul(normalize(v[i]), (float3x3)gWorld);
	//	/*gOut.normalW = mul(float4(normalize(v[i]), 0.0f), gWorld).xyz;*/
	//	triStream.Append(gOut);							// append the current vertex to the triangle stream
	//}
	//triStream.RestartStrip();							// restart the triangle strip for the top sub-triangle
	//
	//gOut.posW = mul( float4(v[1], 1.0f), gWorld );		// transform m0 to world-space
	//gOut.posH = mul( float4(v[1], 1.0f), gWVP );		// transform m0 to homogenized clip-space
	//gOut.normalW = mul(normalize(v[1]), (float3x3)gWorld);
	///*gOut.normalW = mul(float4(normalize(v[1]), 0.0f), gWorld).xyz;*/
	//triStream.Append(gOut);								// append m0 to the triangle stream
	//
	//gOut.posW = mul( float4(v[5], 1.0f), gWorld );		// transform gIn[1] to world-space
	//gOut.posH = mul( float4(v[5], 1.0f), gWVP );		// transform gIn[1] to homogenized clip-space
	//gOut.normalW = mul(normalize(v[5]), (float3x3)gWorld);
	//triStream.Append(gOut);								// append gIn[1] to the triangle stream	
	//
	//gOut.posW = mul( float4(v[3], 1.0f), gWorld );		// transform m1 to world-space
	//gOut.posH = mul( float4(v[3], 1.0f), gWVP );		// transform m1 to homogenized clip-space
	//gOut.normalW = mul(normalize(v[3]), (float3x3)gWorld);
	///*gOut.normalW = mul(float4(normalize(v[3]), 0.0f), gWorld).xyz;*/
	//triStream.Append(gOut);								// append m1 to the triangle stream		
}

// geometry shader - explode icosahedron by translating faces along their normals
[maxvertexcount(8)]
void GSExplode(triangle VS_OUT gIn[3], uint primID : SV_PrimitiveID, inout TriangleStream<GS_OUT> triStream)
{
	GS_OUT gOut;	// create output variable

	// copy over diffuse and specular colours
	gOut.diffuse = gIn[0].diffuse;
	gOut.specular = gIn[0].specular;

	// compute normal
	float3 pts[3] = { gIn[0].posL, gIn[1].posL, gIn[2].posL };
	gOut.normalW = computeNormal(pts);
	//float3 e0 = gIn[1].posL - gIn[0].posL;
	//float3 e1 = gIn[2].posL - gIn[0].posL;
	//float3 normal = normalize( cross(e0, e1) );
	//gOut.normalW = mul(float4(normal, 0.0f), gWorld);	// transform to world-space

	// compute the transformed position as a function of the current time
	float3 transFactor = gCurrTime * gOut.normalW;
	float3 currPos = gIn[0].posL + transFactor * primID;
	gOut.posW = mul(float4(currPos, 1.0f), gWorld).xyz;
	gOut.posH = mul(float4(currPos, 1.0f), gWVP);
	//gOut.normalW = mul(normalize(gIn[0].posL), (float3x3)gWorld);
	triStream.Append(gOut);

	currPos = gIn[1].posL + transFactor * primID;
	gOut.posW = mul(float4(currPos, 1.0f), gWorld).xyz;
	gOut.posH = mul(float4(currPos, 1.0f), gWVP);
	//gOut.normalW = mul(normalize(gIn[1].posL), (float3x3)gWorld);
	triStream.Append(gOut);

	currPos = gIn[2].posL + transFactor * primID;
	gOut.posW = mul(float4(gIn[2].posL, 1.0f), gWorld);
	gOut.posH = mul(float4(gIn[2].posL, 1.0f), gWVP);
	//gOut.normalW = mul(normalize(gIn[2].posL), (float3x3)gWorld);
	triStream.Append(gOut);
}

// pixel shader
float4 PS(GS_OUT pIn)	:	SV_Target
{
	// normalize interpolated normal as it may not be accurate
	pIn.normalW = normalize( pIn.normalW );
	
	// create SurfaceInfo struct and perform Phong shading
	SurfaceInfo surfInfo = { pIn.posW, pIn.normalW, pIn.diffuse, pIn.specular };
	float3 litColour = PhongShading(surfInfo, gLight, gEyePosW, gLightType);
	
	// return final colour
	return float4(litColour, 1.0f);
}

// shader technique
technique10 SphereTech
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( CompileShader( gs_4_0, GS() ) );
		//SetGeometryShader(CompileShader(gs_4_0, GSExplode()));
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
	}
}