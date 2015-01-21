// Common global variables and functions used by the tiled deferred shaders.

#include "deferredShadingCommonPS.hlsli"

#include "Bits.fx"

// NOTE: Going with 16x16 tiles for now. Can be updated as necessary.
#define TILE_DIMS 16
#define TILE_SIZE TILE_DIMS*TILE_DIMS

// Group shared variables used to compute the world space AABB of the current tile.
groupshared int sMinX, sMaxX, sMinY, sMaxY, sMinZ, sMaxZ;

// TODO: Parallel light support needs to be implemented.

// Group shared variables used to store the number of indices of the shortlisted point lights which will influence the current tile.
#define MAX_POINT_LIGHTS 1024
groupshared uint sShortlistedPointLights[MAX_POINT_LIGHTS];
groupshared uint sNumShortlistedPointLights;

// Group shared variables used to store the number of indices of the shortlisted spot lights which will influence the current tile.
#define MAX_SPOT_LIGHTS 1024
groupshared uint sShortlistedSpotLights[MAX_SPOT_LIGHTS];
groupshared uint sNumShortlistedSpotLights;

// Output writable texture which will store the lighting computation results.
RWTexture2D<float4> gOutput	: register(u0);

// Check to see if the sphere overlaps the AABB.
// Source: http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php?print=1
bool AABBOverlapsSphere(float3 aabbMin, float3 aabbMax, float radius, float3 center)
{
	//return true;

	// NOTE: quick hack
#if 0
	float3 aabbCenter = (aabbMin + aabbMax) / 2.0f;
		float aabbExtent = distance(aabbMax, aabbMin); //(aabbMax - aabbMin) / 2.0f;
	//float dist = distance(center, aabbCenter); //sqrt(pow(center.x - aabbCenter.x, 2.0f) + pow(center.y - aabbCenter.y, 2.0f) + pow(center.z - aabbCenter.z, 2.0f));

	//return dist <= (radius + aabbExtent);
	return distance(center, aabbCenter) <= (radius + aabbExtent) /*|| (radius+aabbExtent) <= distance(center, aabbCenter)*/;
#endif // 0

	// NOTE: original version for a hollow box and a hollow sphere. Source: http://tog.acm.org/resources/GraphicsGems/gems/BoxSphere.c
#if 0
	float dmin = 0;
	float dmax = 0;
	bool face = false;
	for (int i = 0; i < 3; i++) {
		float a = pow(center[i] - aabbMin[i], 2.0f);
		float b = pow(center[i] - aabbMax[i], 2.0f);
		dmax += max(a, b);
		if (center[i] < aabbMin[i]) {
			face = true;
			dmin += a;
		}
		else if (center[i] > aabbMax[i]) {
			face = true;
			dmin += b;
		}
		else if (min(a, b) <= radius) face = true;
	}
	if (face && (dmin <= radius) && (radius <= dmax)) return true;
	return false;
#endif	// 0


	// NOTE: Slightly tuned version of the original algorithm. Source: http://www.idt.mdh.se/personal/tla/publ/sb.pdf
#if 0
	float d = 0;
	for (uint i = 0; i < 3; ++i)
	{
		float s = max(aabbMin[i] - center[i], 0) + max(center[i] - aabbMax[i], 0);
		if (s >= radius)	return false;
		d += pow(s, 2.0f);
	}
	return d <= pow(radius, 2.0f);
#endif // 0


	// NOTE: original algorithm
#if 1
	float s, d = 0;

	//find the square of the distance
	//from the sphere to the box
	for (uint i = 0; i < 3; ++i)
	{
		float c = center[i];
		float a1 = aabbMin[i], a2 = aabbMax[i];
		if (c < a1)
		{
			s = center[i] - aabbMin[i];
			d += pow(s, 2.0f); //s*s;
		}

		else if (c > a2)
		{
			s = center[i] - aabbMax[i];
			d += pow(s, 2.0f); //s*s;
		}

	}

	return d <= pow(radius, 2.0f); //radius*radius;  
#endif // 0

}

// Shortlists the lights which intersect the current tile.
// NOTE: Based off of Andrew Lauritzen's 2010 SIGGRAPH course on Programmable Shading: http://software.intel.com/en-us/articles/deferred-rendering-for-current-and-future-rendering-pipelines/
void shortlistTileLights(float3 posW, float groupIndex, uint nPointLights, uint nSpotLights)
{
	// Initialize the group shared variables if this is the first thread in the group.
	if (groupIndex == 0)
	{
		sMinX = 0x7F7FFFFF;      // Max float
		sMaxX = -200000;
		sMinY = 0x7F7FFFFF;      // Max float
		sMaxY = -200000;
		sMinZ = 0x7F7FFFFF;      // Max float
		sMaxZ = -200000;

		sNumShortlistedPointLights = 0;
		sNumShortlistedSpotLights = 0;
	}

	// @TODO: Check if we really need this.
	// Impose a memory barrier so that all threads in the group finish.
	GroupMemoryBarrierWithGroupSync();

	// Update the sMin(X/Y/Z) and sMax(X/Y/Z) values if necessary.
	// NOTE: Need to use asuint as Interlocked(Min/Max) can currently work only on int and uint types.
	InterlockedMin(sMinX, asint_ordered(posW.x));
	InterlockedMax(sMaxX, asint_ordered(posW.x));

	InterlockedMin(sMinY, asint_ordered(posW.y));
	InterlockedMax(sMaxY, asint_ordered(posW.y));

	InterlockedMin(sMinZ, asint_ordered(posW.z));
	InterlockedMax(sMaxZ, asint_ordered(posW.z));

	// Impose a memory barrier so that all threads in the group finish.
	GroupMemoryBarrierWithGroupSync();

	// Store the min and max corners of the world space AABB of the current tile.
	float3 aabbMin = float3(asfloat_ordered(sMinX), asfloat_ordered(sMinY), asfloat_ordered(sMinZ));
	float3 aabbMax = float3(asfloat_ordered(sMaxX), asfloat_ordered(sMaxY), asfloat_ordered(sMaxZ));

	// Shortlist all the point lights whose bounding spheres intersect the current tile's world space AABB.
	[loop]
	for (uint lightIndex = groupIndex; lightIndex < nPointLights; lightIndex += TILE_SIZE)
	{
		[branch]
		if (AABBOverlapsSphere(aabbMin, aabbMax, gPointLights[lightIndex].range, gPointLights[lightIndex].pos) == true)
		{
			// Increment the no. of shortlisted point lights and add the current light index to the list.
			uint listIndex;
			InterlockedAdd(sNumShortlistedPointLights, 1, listIndex);
			sShortlistedPointLights[listIndex] = lightIndex;
		}
	}

	// Shortlist all the spot lights whose bounding spheres intersect the current tile's world space AABB.
	// @TODO: Bounding sphere AABB intersection test is not accurate for spot lights. Fix this.
	[loop]
	for (uint lightIndex2 = groupIndex; lightIndex2 < nSpotLights; lightIndex2 += TILE_SIZE)
	{
		[branch]
		if (AABBOverlapsSphere(aabbMin, aabbMax, gSpotLights[lightIndex2].range, gSpotLights[lightIndex2].pos) == true)
		{
			// Increment the no. of shortlisted spot lights and add the current light index to the list.
			uint listIndex2;
			InterlockedAdd(sNumShortlistedSpotLights, 1, listIndex2);
			sShortlistedSpotLights[listIndex2] = lightIndex2;
		}
	}

	// Impose a memory barrier so that all threads in the group finish.
	GroupMemoryBarrierWithGroupSync();
}

// Convenience function to compute the group index given the group thread ID and the tile size.
// Apparently a compiler bug on Fermi causes SV_GroupIndex to return the incorrect group index,
// which is why it has to be computed manually.
// TODO: Verify this is still the case as the demo was written 4 years ago.
float getGroupIndex(float3 groupThreadId)
{
	return groupThreadId.z * TILE_SIZE + groupThreadId.y * TILE_DIMS + groupThreadId.x;
}