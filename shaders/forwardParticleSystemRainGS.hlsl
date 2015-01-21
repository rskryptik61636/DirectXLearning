// Geometry shader for rain particle systems.

#include "particleSystemCommon.hlsli"

// Constant buffer containing per object constants.
cbuffer cbPerObject	: register(b0)
{
	float4x4 gViewProj;	// camera view-projection matrix

	float3 gSize;	// size of the particle
}

[maxvertexcount(2)]
void forwardParticleSystemRainGS(
	point ForwardVSOut input[1],
	inout LineStream< ForwardGSOut > output
)
{
	// Let the first point be the same as the input.
	ForwardGSOut point1;
	point1.posW = input[0].posW;
	point1.posH = mul(float4(point1.posW, 1.0f), gViewProj);
	//point1.posH = mul(float4(input[0].posW, 1.0f), gViewProj);	// @TODO: remove when done testing
	point1.colour = input[0].colour;
	point1.texC = float2(0.0f, 0.0f);
	output.Append(point1);

	// Let the second point be the input offset by the particle size.
	ForwardGSOut point2;
	point2.posW = input[0].posW + gSize;
	point2.posH = mul(float4(point2.posW, 1.0f), gViewProj);
	//point2.posH = mul(float4(input[0].posW.xyz + gSize, 1.0f), gViewProj);	// @TODO: remove when done testing
	point2.colour = input[0].colour;
	point2.texC = float2(1.0f, 1.0f);
	output.Append(point2);

	/*for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.pos = input[i];
		output.Append(element);
	}*/
}