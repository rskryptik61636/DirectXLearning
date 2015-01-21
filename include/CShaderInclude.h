// Implementation of ID3DInclude to facilitate including files in shaders
// Source: http://nikvoss.com/?p=62

#ifndef CSHADERINCLUDE_H
#define CSHADERINCLUDE_H

#include "d3dUtil.h"

class CShaderInclude : public ID3DInclude {
public:
	CShaderInclude(const char* shaderDir, const char* systemDir) : m_ShaderDir(shaderDir), m_SystemDir(systemDir) {}

	HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
	HRESULT __stdcall Close(LPCVOID pData);
private:
	std::string m_ShaderDir;
	std::string m_SystemDir;
};

#endif	// CSHADERINCLUDE_H