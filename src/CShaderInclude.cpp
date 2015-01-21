// Implementation of ID3DInclude to facilitate including files in shaders
// Source: http://nikvoss.com/?p=62

#include "CShaderInclude.h"

#include <fstream>

HRESULT __stdcall CShaderInclude::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes) {
	try {
		std::string finalPath;
		switch (IncludeType) {
		case D3D_INCLUDE_LOCAL:
			finalPath = m_ShaderDir + "\\" + pFileName;
			break;
		case D3D_INCLUDE_SYSTEM:
			finalPath = m_SystemDir + "\\" + pFileName;
			break;
		default:
			assert(0);
		}

		std::ifstream includeFile(finalPath.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

		if (includeFile.is_open()) {
			long long fileSize = includeFile.tellg();
			char* buf = new char[fileSize];
			includeFile.seekg(0, std::ios::beg);
			includeFile.read(buf, fileSize);
			includeFile.close();
			*ppData = buf;
			*pBytes = fileSize;
		}
		else {
			return E_FAIL;
		}
		return S_OK;
	}
	catch (std::exception& e) {
		return E_FAIL;
	}
}

HRESULT __stdcall CShaderInclude::Close(LPCVOID pData) {
	char* buf = (char*)pData;
	delete[] buf;
	return S_OK;
}