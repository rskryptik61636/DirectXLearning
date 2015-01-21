// GBuffer generation pixel shader, writes geometry and shading information to the GBuffer.
// Computes the bumped normal for the current pixel by sampling the normal map.

#include "deferredShadingCommon.hlsli"

#include "lighthelper.fx"

// Diffuse colour map
Texture2D gDiffuseMap : register(t0);

// Specular colour map
Texture2D gSpecularMap : register(t1);

// Normal ap
Texture2D gNormalMap : register(t2);

// Sampler state used to sample the textures.
SamplerState gSampler : register(s0);

PS_OUT deferredShadingPS2(VS_OUT pIn)
{
	// Init the output struct
	PS_OUT pOut;

	// Just copy the geometry input params into the output params struct
	pOut.posW = float4(pIn.posW, 1.0f);

	// NOTE: uncomment to help debug errors
	//pOut.normalW = gNormalMap.Sample(gSampler, pIn.texC);

	float4 normalSample = gNormalMap.Sample(gSampler, pIn.texC);

	// set the input normal in the output struct if the normal map was not attached, else compute the perturbed normal.
	if (!any(normalSample))
		pOut.normalW = float4(normalize(pIn.normalW), 1.0f);
	else
	{
		if (normalSample.y == 0.0f && normalSample.z == 0.0f)
			normalSample = float4(normalSample.xxx, 1.0f);
		pOut.normalW = float4(ComputePerturbedNormalFromNormalMap(gNormalMap, gSampler, pIn.texC, normalize(pIn.normalW), normalize(pIn.tangentW)), 1.0f);

		// NOTE: Alternate simplified form of perturbed normal computation, keeping around for reference.
		//float3 normalSample = 2.0f * gNormalMap.Sample(gSampler, pIn.texC).xyz - 1.0f;	// Map the sampled normal from [0,1] to [-1,1]
		//pOut.normalW = float4(normalize(normalSample.x * normalize(pIn.tangentW) + 
		//								normalSample.y * normalize(pIn.binormalW) + 
		//								normalSample.z * normalize(pIn.normalW)), 1.0f);	
	}
		

	
	
	// Compute the colour output params
	pOut.diffuse = gDiffuseMap.Sample(gSampler, pIn.texC);
	pOut.specular = gSpecularMap.Sample(gSampler, pIn.texC);

	return pOut;
}