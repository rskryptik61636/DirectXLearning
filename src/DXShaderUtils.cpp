// Various utilities for shaders - header file

#include "DXShaderUtils.h"

// Loads the texture from the given path
HRESULT loadTexture(const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext, const wpath texturePath, ID3D11Resource **pResource, ID3D11ShaderResourceView **pResourceView)
{
	// Use the appropriate function based on the texture filename's extension
	HRESULT hr(E_FAIL);
	if (texturePath.extension() == L".dds")
	{
		hr = DirectX::CreateDDSTextureFromFile(pDevice.p, texturePath.file_string().c_str(), pResource, pResourceView);
	}
	else
	{
		hr = DirectX::CreateWICTextureFromFile(pDevice.p, pDeviceContext.p, texturePath.file_string().c_str(), pResource, pResourceView);
	}

	return hr;
}

// Utility function to construct the absolute path to a shader file based on the current build configuration
wpath constructShaderPath(const wpath &shaderRoot, const wpath &shaderFile)
{
	// define the paths of the tex vertex and pixel shaders based on the build configuration
#ifndef _WIN64

#ifdef _DEBUG
	wpath shaderPath(shaderRoot / wpath(L"Win32") / wpath(L"Debug") / shaderFile);
#else
	wpath shaderPath(shaderRoot / wpath(L"Win32") / wpath(L"Release") / shaderFile);
#endif	// _DEBUG

#else

#ifdef _DEBUG
	wpath shaderPath(shaderRoot / wpath(L"x64") / wpath(L"Debug") / shaderFile);
#else
	wpath shaderPath(shaderRoot / wpath(L"x64") / wpath(L"Release") / shaderFile);
#endif	// _DEBUG

#endif	// _WIN64

	return shaderPath;
}

// Param ctor, handles init.
ShaderConstantBuffer::ShaderConstantBuffer(const std::string &bufName, const DevicePtr &pDevice, const DeviceContextPtr& pDC, const BlobPtr &pShaderByteCode) :
	ShaderVar(),
	varsInfo(),
	pBuffer(),
	eBufferType(BT_CONSTANT),
	mappedData(),
	pDeviceContext(pDC),
	bMapped(false)
{
	// reflect the shader and get the constant buffer bufName's info
	ShaderReflectionPtr pReflector;
	HR(D3DReflect(pShaderByteCode->GetBufferPointer(), pShaderByteCode->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&pReflector.p)));

	ShaderBufferDesc constBufDesc;
	ShaderConstantBufferReflectorPtr pConstBuf = pReflector->GetConstantBufferByName(bufName.c_str());
	HR(pConstBuf->GetDesc(&constBufDesc));

	// init the constant buffer with the binding info
	ShaderInputBindDesc constBufBindDesc;
	HR(pReflector->GetResourceBindingDescByName(bufName.c_str(), &constBufBindDesc));

	// init the constant buffer instance
	strName = bufName;
	bindDesc = constBufBindDesc;

	// get the info of all the constant buffer variables
	for (UINT i = 0; i < constBufDesc.Variables; ++i)
	{
		// get the variable info
		ShaderVariableReflectorPtr pVar = pConstBuf->GetVariableByIndex(i);
		ShaderVariableDesc varDesc;
		HR(pVar->GetDesc(&varDesc));
		const std::string strVarName(varDesc.Name);	// get the variable's name which will be the key for the current entry
		varsInfo[strVarName].varDesc = varDesc;

		// get the type info
		ShaderTypeReflectorPtr pType = pVar->GetType();
		ShaderTypeDesc typeDesc;
		HR(pType->GetDesc(&typeDesc));
		varsInfo[strVarName].typeDesc = typeDesc;
	}

	// release the reflection interface
	pReflector.Release();

	D3D11_BUFFER_DESC bufDesc;
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// bind as a constant buffer
	bufDesc.ByteWidth = constBufDesc.Size;						// buffer size
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// we're going to be mapping this buffer to app memory every time its contents need to be updated
	bufDesc.MiscFlags = 0;							// no misc flags
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;				// it is going to be updated on the fly
	HR(pDevice->CreateBuffer(&bufDesc, 0, &pBuffer.p));	// create the buffer
}

// Map the constant buffer to system memory.
void ShaderConstantBuffer::map()
{
	assert(!bMapped);	// Ensure that the constant buffer is not already mapped.
	HR(pDeviceContext->Map(pBuffer.p, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	bMapped = true;	// Set the mapped flag.
}

// Unmap the constant buffer from system memory.
void ShaderConstantBuffer::unmap()
{
	assert(bMapped);	// Ensure that the constant buffer is mapped.
	pDeviceContext->Unmap(pBuffer.p, 0);
	bMapped = false;	// Reset the mapped flag.
}

// Utility function to copy a matrix into a mapped subresource.
// WARNING: Should not be invoked unless the constant buffer has been mapped first.
void ShaderConstantBuffer::setMatrix(const std::string &varName, const DXMatrix &matrix)
{
	setDatum<DXMatrix>(varName, matrix.Transpose());
}