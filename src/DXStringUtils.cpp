// Misc string utility functions

#include "DXStringUtils.h"

// Utility function to convert a string into a DXVector3
void stringToThreeVector(const char *pszString, DXVector3 &result)
{
	std::vector<float> tmp = stringToVector<float>(pszString);
	assert(tmp.size() == 3);
	result.x = tmp[0];	result.y = tmp[1];	result.z = tmp[2];
}

// Utility function to convert a string into a DXVector2
void stringToTwoVector(const char *pszString, DXVector2 &result)
{
	std::vector<float> tmp = stringToVector<float>(pszString);
	assert(tmp.size() == 2);
	result.x = tmp[0];	result.y = tmp[1];
}

// Utility funciton to convert a string into a DXColor
void stringToColor(const char *pszString, DXColor &result)
{
	std::vector<float> tmp = stringToVector<float>(pszString);
	assert(tmp.size() == 4);
	result.x = tmp[0];	result.y = tmp[1];	result.z = tmp[2];	result.w = tmp[3];
}

// Utility function to convert a string into a DXVector4
void stringToFourVector(const char *pszString, DXVector4 &result)
{
	stringToColor(pszString, result);	// DXColor is a typedef-ed DXVector4
}

// Utility function to convert a string into a DXVector4
DXVector4 stringToFourVector(const char *pszString)
{
	DXVector4 result;
	stringToFourVector(pszString, result);
	return result;
}

// Utility function to convert a string into a DXVector3
DXVector3 stringToThreeVector(const char *pszString)
{
	DXVector3 result;
	stringToThreeVector(pszString, result);
	return result;
}

// Utility function to convert a string into a DXVector2
DXVector2 stringToTwoVector(const char *pszString)
{
	DXVector2 result;
	stringToTwoVector(pszString, result);
	return result;
}

// Utility function to convert a string into a DXColor
DXColor stringToColor(const char *pszString)
{
	DXColor result;
	stringToColor(pszString, result);
	return result;
}

// Utility function to convert a DXVector3 into a narrow string (surrounded by '()' and delimited by ',')
std::string threeVectorToNarrowString(const DXVector3 &input)
{
	return std::string("(" + std::to_string(input.x) + "," + std::to_string(input.y) + "," + std::to_string(input.z) + ")");
}

// Utility function to convert a DXVector3 into a wide string (surrounded by '()' and delimited by ',')
std::wstring threeVectorToWideString(const DXVector3 &input)
{
	return std::wstring(L"(" + std::to_wstring(input.x) + L"," + std::to_wstring(input.y) + L"," + std::to_wstring(input.z) + L")");
}

// Utility function to convert a DXVector2 into a narrow string (surrounded by '()' and delimited by ',')
std::string twoVectorToNarrowString(const DXVector2 &input)
{
	return std::string("(" + std::to_string(input.x) + "," + std::to_string(input.y) + ")");
}

// Utility function to convert a DXVector2 into a wide string (surrounded by '()' and delimited by ',')
std::wstring twoVectorToWideString(const DXVector2 &input)
{
	return std::wstring(L"(" + std::to_wstring(input.x) + L"," + std::to_wstring(input.y) + L")");
}

// Utility function to convert a DXColor into a narrow string (surrounded by '()' and delimited by ',')
std::string colorToNarrowString(const DXColor &input)
{
	return std::string("(" + std::to_string(input.x) + "," + std::to_string(input.y) + "," 
							+ std::to_string(input.z) + "," + std::to_string(input.w) + ")");
}

// Utility function to convert a DXColor into a wide string (surrounded by '()' and delimited by ',')
std::wstring colorToWideString(const DXColor &input)
{
	return std::wstring(L"(" + std::to_wstring(input.x) + L"," + std::to_wstring(input.y) + L","
							+ std::to_wstring(input.z) + L"," + std::to_wstring(input.w) + L")");
}