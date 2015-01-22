//=============================================================================
// lighthelper.fx by Frank Luna (C) 2008 All Rights Reserved.
//
// Structures and functions for lighting calculations.
//=============================================================================


struct Light
{
	float3 position;
	float3 dir;
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float3 att;
	float  spotPower;
	float  range;
};

struct SurfaceInfo
{
	float3 position;
    float3 normal;
    float4 diffuse;
    float4 spec;
};

float3 ParallelLight(SurfaceInfo v, Light L, float3 eyePos)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
 
	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -L.dir;
	
	// Add the ambient term.
	litColor += v.diffuse * L.ambient;	
	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot(lightVec, v.normal);

	// modify diffuseFactor for toon shading
	if( diffuseFactor <= 0.0f )
		diffuseFactor = 0.4f;
	else if( diffuseFactor > 0.0f && diffuseFactor <= 0.5f )
		diffuseFactor = 0.6f;
	else if( diffuseFactor > 0.5f && diffuseFactor <= 1.0f )
		diffuseFactor = 1.0f;

	[branch]
	//if( diffuseFactor > 0.0f )
	//{
		float specPower  = max(v.spec.a, 1.0f);
		float3 toEye     = normalize(eyePos - v.position);
		float3 R         = reflect(-lightVec, v.normal);
		float specFactor = pow(max(dot(R, toEye), 0.0f), specPower);

		// modify specular factor for toon shading
		if( specFactor >= 0.0f && specFactor <= 0.1f )
			specFactor = 0.0f;
		else if( specFactor > 0.1f && specFactor <= 0.8f )
			specFactor = 0.5f;
		else if( specFactor > 0.8f && specFactor <= 1.0f )
			specFactor = 0.8f;
					
		// diffuse and specular terms
		litColor += diffuseFactor * v.diffuse * L.diffuse;
		litColor += specFactor * v.spec * L.spec;
	//}
	
	return litColor;
}

float3 PointLight(SurfaceInfo v, Light L, float3 eyePos)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
	
	// The vector from the surface to the light.
	float3 lightVec = L.position - v.position;
		
	// The distance from surface to light.
	float d = length(lightVec);
	
	if( d > L.range )
		return float3(0.0f, 0.0f, 0.0f);
		
	// Normalize the light vector.
	lightVec /= d; 
	
	// Add the ambient light term.
	litColor += v.diffuse * L.ambient;	
	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot(lightVec, v.normal);

	// modify diffuseFactor for toon shading
	if( diffuseFactor <= 0.0f )
		diffuseFactor = 0.4f;
	else if( diffuseFactor > 0.0f && diffuseFactor <= 0.5f )
		diffuseFactor = 0.6f;
	else if( diffuseFactor > 0.5f && diffuseFactor <= 1.0f )
		diffuseFactor = 1.0f;

	[branch]
	//if( diffuseFactor > 0.0f )
	//{
		float specPower  = max(v.spec.a, 1.0f);
		float3 toEye     = normalize(eyePos - v.position);
		float3 R         = reflect(-lightVec, v.normal);
		float specFactor = pow(max(dot(R, toEye), 0.0f), specPower);

		// modify specular factor for toon shading
		if( specFactor >= 0.0f && specFactor <= 0.1f )
			specFactor = 0.0f;
		else if( specFactor > 0.1f && specFactor <= 0.8f )
			specFactor = 0.5f;
		else if( specFactor > 0.8f && specFactor <= 1.0f )
			specFactor = 0.8f;
	
		// diffuse and specular terms
		litColor += diffuseFactor * v.diffuse * L.diffuse;
		litColor += specFactor * v.spec * L.spec;
	//}
	
	// attenuate
	return litColor / dot(L.att, float3(1.0f, d, d*d));
}

float3 Spotlight(SurfaceInfo v, Light L, float3 eyePos)
{
	float3 litColor = PointLight(v, L, eyePos);
	
	// The vector from the surface to the light.
	float3 lightVec = normalize(L.position - v.position);
	
	float s = pow(max(dot(-lightVec, L.dir), 0.0f), L.spotPower);
	
	// Scale color by spotlight factor.
	return litColor*s;
}

 
 