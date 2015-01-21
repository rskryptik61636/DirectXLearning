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