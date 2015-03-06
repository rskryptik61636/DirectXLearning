// Pixel shader which is used in the second pass of the shadow mapping effect. Simple shadow mapping implementation, no filtering involved.

#include "renderShadowMapCommon.hlsli"

#include "lighthelper.fx"

// Constant buffer for lighting (considering only one spot light for now)
cbuffer cbPerFrame	: register(b0)
{
	Light gSpotLights[MAX_SPOT_LIGHTS];	// spot lights
	int nSpotLights;					// no. of actual spot lights in the array

	float3 gEyePosW;	// eye position in world space
};

Texture2D<float4> gDiffuseMap	: register(t0);	// Diffuse lighting texture map
Texture2D<float4> gSpecularMap	: register(t1);		// Specular lighting texture map
Texture2D<float4> gNormalMap	: register(t2);	// Normal map
Texture2DArray<float> gShadowMap	: register(t3);	// Shadow map

SamplerState gTriLinearSam	: register(s0);	// Linear filtering for minification, magnification and mipmapping
SamplerState gShadowSam	: register(s1);	// Used for sampling the shadow map

float4 renderShadowMapSimplePS(VS_OUT pIn) : SV_TARGET
{
	// Compute the diffuse colour from the diffuse lighting texture map
	// TODO: break out the ComputeBlinnPhongShading function into subcomponents which compute
	//		 the diffuse and specular components separately so that we can call them here
	float4 diffuse = gDiffuseMap.Sample(gTriLinearSam, pIn.texC);

	// Compute the lighting contribution of all the spot lights (only working with one to start with)
	float3 litColor = (gSpotLights[0].ambient.rgb * diffuse.rgb); //float3(0.0f, 0.0f, 0.0f);
	
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
			// Compare the depth of the light's texture space position against its corresponding depth value in the shadow map,
			// and proceed with the lighting calculations iff the point is not in shadow.
			float pointDepth = currTex.z, lightDepth = gShadowMap.SampleLevel(gShadowSam, float3(currTex.xy, i), 0).r;
			if (pointDepth > lightDepth)	// in shadow, return the ambient colour
			{
				//litColor += (gSpotLights[i].ambient.rgb * diffuse.rgb);
				continue;
			}				

			// Compute the final pixel colour
			SurfaceInfo material = { pIn.posW, normalW, diffuse, specular };

			litColor += SpotLight(material, gSpotLights[i], gEyePosW);
		}		
	}	

	return float4(litColor.rgb, 1.0f);
}