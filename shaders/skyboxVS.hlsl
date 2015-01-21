// Vertex shader for the skybox effect

#include "skyboxCommon.hlsli"

// Constants per object
cbuffer cbPerObject
{
	float4x4 gWVP;	// world-view-projection matrix
};

SkyboxVSOut skyboxVS( SkyboxVSIn vIn )
{
	// Declare the output param.
	SkyboxVSOut vOut;

	// Transform the vertex position to homogenous clip space but set the z to 1.0 so that it always ends up on the far plane.
	vOut.posH = mul(float4(vIn.posL, 1.0f), gWVP).xyww;

	// Set the tex coord as the local space position which should be a point on a normalized unit sphere.
	vOut.texC = vIn.posL;

	return vOut;
}