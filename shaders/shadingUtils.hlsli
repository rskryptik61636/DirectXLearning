// Shading and lighting utilities.

#include "lighthelper.fx"

// Accumulates the effect of all the given lights.
float3 accummulateLighting(
	StructuredBuffer<Light> parallelLights,
	uint nParallelLights,
	StructuredBuffer<Light> pointLights,
	uint nPointLights,
	StructuredBuffer<Light> spotLights,
	uint nSpotLights,
	SurfaceInfo surfaceInfo, 
	float3 eyePosW)
{
	//// Determine the total no. of parallel, point and spot lights.
	//uint nParallelLights, nPointLights, nSpotLights, dummy;
	//parallelLights.GetDimensions(nParallelLights, dummy);
	//pointLights.GetDimensions(nPointLights, dummy);
	//spotLights.GetDimensions(nSpotLights, dummy);

	// Initialize the lit colour to the ambient lighting contribution.
	float3 litColour;
	if (nParallelLights > 0)
		litColour = parallelLights[0].ambient.rgb * surfaceInfo.diffuse.rgb;
	else if (nPointLights > 0)
		litColour = pointLights[0].ambient.rgb * surfaceInfo.diffuse.rgb;
	else if (nSpotLights > 0)
		litColour = spotLights[0].ambient.rgb * surfaceInfo.diffuse.rgb;
	else
		return float3(0.0f, 0.0f, 0.0f);	// Nothing to do since there are no lights.
	//litColour = float3(0.0f, 0.0f, 0.0f);

	// Accummulate the lighting contributions of all the different types of lights in the scene.
	[loop]
	for (uint i = 0; i < nParallelLights; ++i)
		litColour += ParallelLight(surfaceInfo, parallelLights[i], eyePosW);

	[loop]
	for (uint j = 0; j < nPointLights; ++j)
		litColour += PointLight(surfaceInfo, pointLights[j], eyePosW);

	[loop]
	for (uint k = 0; k < nSpotLights; ++k)
		litColour += SpotLight(surfaceInfo, spotLights[j], eyePosW);

	return litColour;
}