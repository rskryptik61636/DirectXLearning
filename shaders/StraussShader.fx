/* Strauss Lighting model implementation
Author - Rohit Shrinath
*/

#include "lighthelper.fx"

struct StraussParams
{
	float smoothness;
	float metalness;
	float transparency;
};

// function which returns the ambient component
float3 CalcAmbientComponent(float3 ambientColour, float diffuseReflectivity)
{
	return diffuseReflectivity * ambientColour;
}

// function which returns the diffuse component
float3 CalcDiffuseComponent(float3 diffuseColour, float3 lightDir, float3 surfaceNormal, 
							float diffuseReflectivity, float reflectivityMultiplier)
{
	float diffuseFactor = dot(surfaceNormal, normalize(lightDir)) * diffuseReflectivity * reflectivityMultiplier;
	return diffuseFactor * diffuseColour;

	//float3 finalColor;
	//finalColor.x = dot(surfaceNormal, normalize(lightDir)) * diffuseReflectivity * reflectivityMultiplier * diffuseColour.x;
}

// function which calculates the Fresnel Reflectance Approximation
float CalcFresnelReflectanceApproximation(float angleOfIncidence, float approxConstant)
{
	float numerator, denominator;
	numerator = ( (approxConstant*approxConstant) - (angleOfIncidence-approxConstant) * (angleOfIncidence-approxConstant) ) / 
				( (angleOfIncidence-approxConstant)*(angleOfIncidence-approxConstant) * (approxConstant*approxConstant) );
	denominator =	( (approxConstant*approxConstant) - (1.0-approxConstant)*(1.0-approxConstant) ) / 
					( (1.0-approxConstant)*(1.0-approxConstant) * (approxConstant*approxConstant) );

	return numerator / denominator;
}

// function which calculates the Geometric Attenuation Approximation
float CalcGeometricAttenuationApproximation(float viewAngle, float approxConstant)
{
	float numerator, denominator;
	numerator = ( (viewAngle-approxConstant)*(viewAngle-approxConstant) - (1.0-approxConstant)*(1.0-approxConstant) ) / 
				( (1.0-approxConstant)*(1.0-approxConstant) * (viewAngle-approxConstant)*(viewAngle-approxConstant) );
	denominator =	( (approxConstant*approxConstant) - (1.0-approxConstant)*(1.0-approxConstant) ) / 
					( (1.0-approxConstant)*(1.0-approxConstant) * (approxConstant*approxConstant) );
	return numerator / denominator;
}

// function which calculates the specular component of lighting
float3 CalcSpecularComponent(	float3 diffuseColour, float3 lightDir, float3 surfaceNormal, float3 viewVector, 
								float metal, float specularExponent, float diffuseReflectivity, float specularReflectivity )
{
	float3 finalColor, diffuse;
	float3 highlightVector = normalize( reflect( lightDir, surfaceNormal ) );	//highlightVector = highlight vector
	
	float kj = 0.1;
	float alpha = dot( normalize(lightDir), surfaceNormal ) / 1.5708;	// alpha = angle of incidence 
	float gamma = dot( viewVector, surfaceNormal )  / 1.5708;			// gamma = view angle
	float fresnelApproxFuncConstant = 1.12; 		//fresnelApproxFuncConstant = Fresnel approximation function constant
	float attenApproxFuncConstant = 1.01;			//attenApproxFuncConstant = geometric attenuation approximation function constant

	//specularReflectAdjust = specular reflectivity adjustment
	float specularReflectAdjust =	CalcFresnelReflectanceApproximation(alpha, fresnelApproxFuncConstant) * 
									CalcGeometricAttenuationApproximation(alpha, attenApproxFuncConstant) * 
									CalcGeometricAttenuationApproximation(gamma, attenApproxFuncConstant);	
	float adjustedSpecReflectivity = min( 1.0, specularReflectivity + (specularReflectivity + kj) * specularReflectAdjust );	//adjustedSpecReflectivity = adjusted specular reflectivity
	
	float3 Cw = float3(1.0,1.0,1.0);									//Cw = pure white
	diffuse = diffuseColour;											//diffuse = surface color // float3( gl_FrontMaterial.diffuse );
	float3 CSpecular = Cw - metal * ( 1.0 - CalcFresnelReflectanceApproximation(alpha, fresnelApproxFuncConstant) ) * ( Cw - diffuse );	//CSpecular = specular color 
	float specReflectivity = pow ( -dot( viewVector, highlightVector ), specularExponent ) * adjustedSpecReflectivity;	//specReflectivity = specular reflectivity
	
	finalColor = CSpecular * specReflectivity;													//finalColor is set to the specular lighting contribution
	return finalColor;
}

float3 StraussShading(SurfaceInfo surfInfo, Light lightSource, StraussParams sParams, float eyePos, int lightType )
{
	float diffuseReflectivity = (1.0f - sParams.smoothness*sParams.smoothness*sParams.smoothness) * (1.0f - sParams.transparency);
	float diffReflectMultiplier = 1.0f - sParams.metalness*sParams.smoothness;
	float specularExponent = 3.0f / (1.0f - sParams.smoothness);
	float specularReflectivity = 1.0f - sParams.transparency - diffuseReflectivity;

	float3 diffuseColourComp = lightSource.diffuse.xyz * surfInfo.diffuse.xyz;
	float3 surfaceNormal = surfInfo.normal;

	// compute light vector based on lightType
	float3 lightVec;
	if(lightType == 0)	// Parallel light
	{
		lightVec = -lightSource.dir;
	}
	else if(lightType == 1 || lightType == 2)	// Point light and Spot light
	{
		lightVec = lightSource.position - surfInfo.position;	// light vector is calculated opposite to its direction
									// lightVec = light_position - surface_point
		// no lighting if lightVec is beyond the range of light lightSource
		if(length(lightVec) > lightSource.range)
			return float3(0.0f,0.0f,0.0f);
		lightVec = normalize(lightVec);
	}

	// compute the diffuse component
	float3 diffuseComp = CalcDiffuseComponent(diffuseColourComp,lightVec,surfaceNormal,
											  diffuseReflectivity, diffReflectMultiplier);
	diffuseComp = max(float3(0.0f,0.0f,0.0f), diffuseComp);	// ensure diffuseComp doesn't fall below 0

	// compute the specular component
	float3 viewVector = normalize(eyePos - surfInfo.position);
	float3 specularComp = CalcSpecularComponent(diffuseColourComp, lightVec, surfaceNormal, viewVector, 
												sParams.metalness, specularExponent, diffuseReflectivity, 
												specularReflectivity);
	specularComp = max( float3(0.0f,0.0f,0.0f), specularComp);

	// compute the ambient component and the final colour
	float3 ambientColourComp = lightSource.ambient.xyz;
	float3 ambientComp = CalcAmbientComponent(ambientColourComp, diffuseReflectivity);

	float3 finalColour = diffuseColourComp*ambientComp + diffuseColourComp*(diffuseComp + specularComp);
	return finalColour;
}