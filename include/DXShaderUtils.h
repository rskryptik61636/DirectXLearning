// Various utilities for shaders - header file
#ifndef DX_SHADER_UTILS_H
#define DX_SHADER_UTILS_H

#include "d3dUtil.h"

// Loads the texture from the given path
HRESULT loadTexture(const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext, const wpath texturePath, ID3D11Resource **pResource, ID3D11ShaderResourceView **pResourceView);

#endif	// DX_SHADER_UTILS_H