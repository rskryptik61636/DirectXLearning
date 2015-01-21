//=============================================================================
// lighthelper.fx by Frank Luna (C) 2008 All Rights Reserved.
//
// Structures and functions for lighting calculations.
//=============================================================================

// Modified by Rohit S to use the Phong-Blinn shading model (Feb-27-2014)

struct Light
{
	float3 pos;
	float3 dir;
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float3 att;
	float  spotPower;
	float  range;

	// added for animation
	float radius;
	float angle;
	float animationSpeed;
};

struct SurfaceInfo
{
	float3 pos;
    float3 normal;
    float4 diffuse;
    float4 spec;
};

// NOTE: It is to be noted that all vectors are computed as (src - dst) as opposed to the standard
//		 convention of (dst - src). This is in order to ensure that the vectors are computed in the same
//		 manner as the surface normal vector which points outwards from the surface.

// NOTE: The ambient lighting contribution is not taken into computed by any of the lighting functions
//		 and it is up to the caller to take it into consideration. This is done primarily because the
//		 ambient lighting is computed at full intensity and is not really influenced by the surface
//		 or the light source in any way.

// Compute Phong-Blinn shading as described in: http://takinginitiative.wordpress.com/2010/08/30/directx-10-tutorial-8-lighting-theory-and-hlsl/
float3 ComputePhongBlinnShading(SurfaceInfo material, Light L, float3 lightVector, float3 eyePos)
{
	// Compute the diffuse lighting term: dot(light_dir, surface_normal)
	// NOTE: The article states that it is very important to clamp the diffuse and specular terms to [0,1] as they
	//		 might fall outside that range.
	float diffuseFactor = saturate(dot(lightVector, material.normal));
	if (diffuseFactor == 0.0f)	// Leave early if there is no diffuse lighting.
		return float3(0.0f, 0.0f, 0.0f);

	// Compute the halfway vector (vector addition of light direction and view vector)
	float3 viewVector = normalize(eyePos - material.pos);
	float3 halfwayVector = normalize(lightVector + viewVector);

	// Compute the specular lighting term: dot(halfway_vector, surface_normal)^specular_exponent
	float specularExponent = max(material.spec.a, 1.0f);	// Need to ensure that the specular exponent does not fall below 1
	float specularFactor = pow(saturate(dot(halfwayVector, material.normal)), specularExponent);

	// Final result = diffuse_term * material_diffuse_color * light_diffuse_color + specular_term * material_specular_color * light_specular color
	return (diffuseFactor * material.diffuse.rgb * L.diffuse.rgb) + (specularFactor * material.spec.rgb * L.spec.rgb);
}

float3 ParallelLight(SurfaceInfo material, Light L, float3 eyePos)
{	
	// Return the computed color directly as they're nothing else that needs to be done
	float3 lightVector = -L.dir;
	return /*(material.diffuse.rgb * L.ambient.xyz) +*/ ComputePhongBlinnShading(material, L, lightVector, eyePos);
}

float3 PointLight(SurfaceInfo material, Light L, float3 eyePos)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f); //= material.diffuse.xyz * L.ambient.rgb; // default to the ambient light color
	
	// The vector from the surface to the light.
	float3 lightVector = L.pos - material.pos;
		
	// The distance from surface to light.
	float d = length(lightVector);
	
	// Return the ambient color if the point is out of range
	if (d > L.range)
		return litColor;

	// Normalize the light vector.
	lightVector /= d; 

	// Add the Phong-Blinn shading
	litColor += ComputePhongBlinnShading(material, L, lightVector, eyePos);
		
	// Attenuate
	return litColor / dot(L.att, float3(1.0f, d, d*d));
}

float3 Spotlight(SurfaceInfo material, Light L, float3 eyePos)
{
	float3 litColor = PointLight(material, L, eyePos);
	
	// The vector from the surface to the light.
	float3 lightVec = normalize(material.pos - L.pos);
	
	// Compute the spotlight factor
	float s = pow(saturate(dot(lightVec, L.dir)), L.spotPower);
	
	// Scale color by spotlight factor.
	return litColor*s;
}


// Helper function to compute the perturbed normal from a normal map
float3 ComputePerturbedNormalFromNormalMap(Texture2D gNormalMap, SamplerState gSamplerState, float2 texC, float3 normalW, float3 tangentW)
{
	// sample the normal map and map from [0,1] to [-1,1]
	float3 normalSample = float4(2.0f * gNormalMap.Sample(gSamplerState, texC) - 1.0f).xyz;

	// construct an orthonormal basis TBN from normalW and tangentW and
	// transform the normal sample to world space
	float3 N = normalize(normalW);	// normal may not be a unit vector after interpolation
	float3 T = normalize(normalize(tangentW) - dot(N, normalize(tangentW)) * N);
	float3 B = cross(N, T);
	float3x3 TBN = float3x3(T, B, N);
	return normalize(mul(normalSample, TBN));
}

// Helper function to determine whether a given (x,y) value is in the [0,1] range
bool inRange(float x, float y)
{
	return x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f;
}

// Helper function to perform a window-to-viewport-esque mapping as seen in http://www.siggraph.org/education/materials/HyperGraph/viewing/view2d/pwint.htm
float windowToViewport(float winVal, float winMin, float winMax, float viewMin, float viewMax)
{
	return (viewMax - viewMin) / (winMax - winMin) * (winVal - winMin) + viewMin;
}

// Helper function to compute the PCF offset.
float2 pcfOffset(float x, float y, float2 nShadowMapSize)
{
	return float2(x * 1.0f / nShadowMapSize.x, y * 1.0f / nShadowMapSize.y);
}