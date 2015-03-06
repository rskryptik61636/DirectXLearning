// Math utilities

#include "Bits.fx"

static const float PI = 3.14159265f;

// Helper function to pick a random number from a texture containing random numbers 
// using the given selection criteria r1 and r2 which should be in the [0,1] range.
float4 randomNum(Texture1D<float4> randomNums, SamplerState randomSampler, float r1, float r2)
{
	// Compute a random sampling offset based on the selection criteria.
	float offset = saturate(r1 + r2);

	// Return the random number which is located at the computed offset in the texture.
	return randomNums.SampleLevel(randomSampler, offset, 0);
}

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