// Misc string utility functions

#ifndef DX_STRING_UTILS_H
#define DX_STRING_UTILS_H

#include "d3dUtil.h"

#include <sstream>

// Utility function to convert a string into a vector
// Source: http://stackoverflow.com/questions/9986091/how-do-you-convert-a-string-into-an-array-of-floats
template<typename T>
std::vector<T> stringToVector(const char *pszString)
{
	// ensure that T is of type floating point because that's the only type that is supported as of this writing (Feb-16-2014)
	static_assert(std::is_floating_point<T>::value, "stringToVector currently only supports floating-point types");

	// TODO: consider additional static asserts that may be required

	std::vector<T> tmp;
	std::istringstream val(pszString);
	std::copy((std::istream_iterator<T>(val)), std::istream_iterator<T>(), std::back_inserter(tmp));
	return tmp;
}

// Utility function to convert a string into a DXVector3
void stringToThreeVector(const char *pszString, DXVector3 &result);

// Utility function to convert a string into a DXVector2
void stringToTwoVector(const char *pszString, DXVector2 &result);

// Utility function to convert a string into a DXColor
void stringToColor(const char *pszString, DXColor &result);

// Utility function to convert a string into a DXVector4
DXVector4 stringToFourVector(const char *pszString);

// Utility function to convert a string into a DXVector3
DXVector3 stringToThreeVector(const char *pszString);

// Utility function to convert a string into a DXVector2
DXVector2 stringToTwoVector(const char *pszString);

// Utility function to convert a string into a DXColor
DXColor stringToColor(const char *pszString);

// Utility function to convert a string into a DXVector4
void stringToFourVector(const char *pszString, DXVector4 &result);

// Utility function to convert a DXVector3 into a narrow string (surrounded by '()' and delimited by ',')
std::string threeVectorToNarrowString(const DXVector3 &input);

// Utility function to convert a DXVector3 into a wide string (surrounded by '()' and delimited by ',')
std::wstring threeVectorToWideString(const DXVector3 &input);

// Utility function to convert a DXVector2 into a narrow string (surrounded by '()' and delimited by ',')
std::string twoVectorToNarrowString(const DXVector2 &input);

// Utility function to convert a DXVector2 into a wide string (surrounded by '()' and delimited by ',')
std::wstring twoVectorToWideString(const DXVector2 &input);

// Utility function to convert a DXColor into a narrow string (surrounded by '()' and delimited by ',')
std::string colorToNarrowString(const DXColor &input);

// Utility function to convert a DXColor into a wide string (surrounded by '()' and delimited by ',')
std::wstring colorToWideString(const DXColor &input);

// Convenience function to perform a case sensitive comparison of two strings
inline bool isStrictlyEqual(const char *psz1, const char *psz2)	{ return strcmp(psz1, psz2) == 0; }

// Convenience function to perform a case insensitive comparison of two narrow strings
inline bool isEqual(const char *psz1, const char *psz2)	{ return _stricmp(psz1, psz2) == 0; }

#endif	// DX_STRING_UTILS_H