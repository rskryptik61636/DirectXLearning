// Pixel shader which is used in the second pass of the shadow mapping effect. Simple shadow mapping implementation, no filtering involved.

#include "renderShadowMapCommon.hlsli"

#include "lighthelper.fx"

// Constant buffer for lighting (considering only one spot light for now)
cbuffer cbPerFrame	: register(b0)
{
	Light gSpotLights[MAX_SPOT_LIGHTS];	// spot lights
	int nSpotLights;					// no. of actual spot lights in the array

	float3 gEyePosW;	// eye position in world space

	float2 nShadowMapSize;	// (width,height) of the shadow map

	float nTapSize;	// tap size (1, 16)
};

Texture2D<float4> gDiffuseMap	: register(t0);	// Diffuse lighting texture map
Texture2D<float4> gSpecularMap	: register(t1);		// Specular lighting texture map
Texture2D<float4> gNormalMap	: register(t2);	// Normal map
Texture2DArray<float> gShadowMap	: register(t3);	// Shadow map

SamplerState gTriLinearSam	: register(s0);	// Linear filtering for minification, magnification and mipmapping
SamplerComparisonState gShadowSam	: register(s1);	// Used for sampling the shadow map and performing PCF filtering

float4 renderShadowMapPCFPS(VS_OUT pIn) : SV_TARGET
{
	// Compute the diffuse colour from the diffuse lighting texture map
	// TODO: break out the ComputeBlinnPhongShading function into subcomponents which compute
	//		 the diffuse and specular components separately so that we can call them here
	float4 diffuse = gDiffuseMap.Sample(gTriLinearSam, pIn.texC);

	// Compute the lighting contribution of all the spot lights (only working with one to start with)
	float3 litColor = (gSpotLights[0].ambient.rgb * diffuse.rgb); // float3(0.0f, 0.0f, 0.0f);
	
	[loop]	// Tells HLSL to give preference to the for loop's constructs: http://msdn.microsoft.com/en-us/library/bb313978%28v=xnagamestudio.31%29.aspx
	for (int i = 0; i < nSpotLights; ++i)
	{
		// Normalize the light's texture space position as it would have been modified during interpolation
		float4 currTex = pIn.posLightT[i];
		currTex.xyz /= currTex.w;

		// Compute the specular colour from the specular lighting texture map and map the alpha value from [0,1] to [0,256]
		float4 specular = gSpecularMap.Sample(gTriLinearSam, pIn.texC);
		specular.a *= 256.0f;

		// Compute the perturbed normal from the normal map
		float3 normalW = ComputePerturbedNormalFromNormalMap(gNormalMap, gTriLinearSam, pIn.texC, pIn.normalW, pIn.tangentW);

		// Only shade the current point if it lies within the light's clip volume.
		if (inRange(currTex.x, currTex.y))
		{
			// Extract the depth value from the shadow map.
			float pointDepth = currTex.z;

			// Compute the PCF shadow interpolation factor based on the no. of taps
			float shadowFactor;
			if (nTapSize == 1.0f)
			{
				shadowFactor = gShadowMap.SampleCmpLevelZero(gShadowSam, float3(currTex.xy, i), currTex.z);
			}
			else if (nTapSize == 16.0f)
			{
				shadowFactor = 0.0f;
				for (float y = -1.5f; y <= 1.5f; y += 1.0f)
				{
					for (float x = -1.5f; x <= 1.5f; x += 1.0f)
					{
						shadowFactor += gShadowMap.SampleCmpLevelZero(gShadowSam, float3(currTex.xy + pcfOffset(x, y, nShadowMapSize), i), currTex.z);
					}
				}
				shadowFactor /= 16.0f;
			}

			// Compute the final pixel colour using the computed shadow factor
			SurfaceInfo material = { pIn.posW, normalW, diffuse, specular };

			litColor += /*(gSpotLights[i].ambient.rgb * diffuse.rgb) +*/ shadowFactor * Spotlight(material, gSpotLights[i], gEyePosW);
		}		
	}	

	return float4(litColor.rgb, 1.0f);
}

//// Sample the points at (x,y); (x+dx,y); (x,y+dy); (x+dx,y+dy)
//// where dx and dy are determined by the pcfOffset helper function.
//float2 filterStep = pcfOffset(pIn.posLightT.x, pIn.posLightT.y);
//float s0 = gShadowMap.Sample(gShadowSam, pIn.posLightT.xy + pcfOffset(0, 0)).r;
//float s1 = gShadowMap.Sample(gShadowSam, pIn.posLightT.xy + pcfOffset(filterStep.x, 0)).r;
//float s2 = gShadowMap.Sample(gShadowSam, pIn.posLightT.xy + pcfOffset(0, filterStep.y)).r;
//float s3 = gShadowMap.Sample(gShadowSam, pIn.posLightT.xy + pcfOffset(filterStep.x, filterStep.y)).r;

//// Compute the results of the shadow test at each of the sampled points
//float r0 = s0 > pointDepth;
//float r1 = s1 > pointDepth;
//float r2 = s2 > pointDepth;
//float r3 = s3 > pointDepth;

//// Compute the bilinear interpolation factor which will be used to interpolate between
//// the shadow test results.
//float2 interp = frac(filterStep);

//// Lerp between the results and see what we finally end up with
//// TODO: add this to the if condition once we have seen what it does
//float shadowFactor = lerp(lerp(r0, r1, interp.x), lerp(r2, r3, interp.x), interp.y);