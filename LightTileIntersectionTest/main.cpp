#include <iostream>

// Check to see if the sphere overlaps the AABB.
// Source: http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php?print=1
bool AABBOverlapsSphere(float aabbMin[3], float aabbMax[3], float radius, float center[3])
{
	//return true;

	// NOTE: quick hack
#if 0
	float aabbCenter[3] = (aabbMin + aabbMax) / 2.0f;
	float aabbExtent = distance(aabbMax, aabbMin); //(aabbMax - aabbMin) / 2.0f;
	//float dist = distance(center, aabbCenter); //sqrt(pow(center.x - aabbCenter.x, 2.0f) + pow(center.y - aabbCenter.y, 2.0f) + pow(center.z - aabbCenter.z, 2.0f));

	//return dist <= (radius + aabbExtent);
	return distance(center, aabbCenter) <= (radius + aabbExtent) /*|| (radius+aabbExtent) <= distance(center, aabbCenter)*/;
#endif // 0


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
	for (unsigned i = 0; i < 3; ++i)
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

/// Converts the given float into an integer bit representation maintaing relative order.
int asint_ordered(float f)
{
	int i = *reinterpret_cast<int*>(&f);
	if (i < 0)
		i = (1 << 31) - i;
	return i;
}

int main(int argc, char *argv[])
{
	/*for(std::size_t i = 0; i < 10; ++i)
		std::cout << i%6 << " ";
	std::cout << std::endl;*/

	while (true)
	{
		float f;
		std::cout << "Enter a floating point number: ";
		std::cin >> f;
		int i = asint_ordered(f);
		std::cout << "Int representation = " << i << std::endl; 
	}

#if 0
	// Enter the params.
	float aabbMin[3], aabbMax[3], center[3], radius;
	std::cout << "Enter the AABB min (x,y,z): ";
	std::cin >> aabbMin[0] >> aabbMin[1] >> aabbMin[2];

	std::cout << "Enter the AABB max (x,y,z): ";
	std::cin >> aabbMax[0] >> aabbMax[1] >> aabbMax[2];

	std::cout << "Enter the light's position (x,y,z): ";
	std::cin >> center[0] >> center[1] >> center[2];

	std::cout << "Enter the light's radius: ";
	std::cin >> radius;

	// Display the intersection result.
	const bool intersects = AABBOverlapsSphere(aabbMin, aabbMax, radius, center);
	std::cout << (intersects ? "Success!!!" : "Failure!!!") << std::endl;
#endif // 0


	return EXIT_SUCCESS;
}