// Copyright 2010 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.

// Modified by Rohit S to work with the DXLearning framework (Mar-15-2014)

// Modified by Rohit S to added accessor functions which return the texture dims (Apr-12-2014)

#pragma once

//#include <d3d11.h>
//#include <vector>
#include "d3dUtil.h"

class Texture2D
{
public:
    // Construct a Texture2D
    Texture2D(const DevicePtr &pDevice,
              int width, int height, DXGI_FORMAT format,
              UINT bindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
              int mipLevels = 1);

    // Construct a Texture2DMS
    Texture2D(const DevicePtr &pDevice,
              int width, int height, DXGI_FORMAT format,
              UINT bindFlags,
              const DXGI_SAMPLE_DESC& sampleDesc);

    // Construct a Texture2DArray
    Texture2D(const DevicePtr &pDevice,
              int width, int height, DXGI_FORMAT format,
              UINT bindFlags,
              int mipLevels, int arraySize);

    // Construct a Texture2DMSArray
    Texture2D(const DevicePtr &pDevice,
              int width, int height, DXGI_FORMAT format,
              UINT bindFlags,
              int arraySize, const DXGI_SAMPLE_DESC& sampleDesc);

    ~Texture2D();

	ID3D11Texture2D* GetTexture() { return m_pTexture.p; }

	ID3D11RenderTargetView* GetRenderTarget(std::size_t arrayIndex = 0) { return mRenderTargetElements[arrayIndex].p; }

    // Treat these like render targets for now... i.e. only access a slice
	ID3D11UnorderedAccessView* GetUnorderedAccess(std::size_t arrayIndex = 0) { return mUnorderedAccessElements[arrayIndex].p; }

    // Get a full view of the resource
	ID3D11ShaderResourceView* GetShaderResource() { return m_pShaderResource.p; }

    // Get a view to the top mip of a single array element
	ID3D11ShaderResourceView* GetShaderResource(std::size_t arrayIndex) { return mShaderResourceElements[arrayIndex].p; }

	// Get the texture width and height
	inline int GetWidth() const	{ return mWidth; }
	inline int GetHeight() const	{ return mHeight; }

private:
    void InternalConstruct(const DevicePtr &pDevice,
                           int width, int height, DXGI_FORMAT format,
                           UINT bindFlags, int mipLevels, int arraySize,
                           int sampleCount, int sampleQuality,
                           D3D11_RTV_DIMENSION rtvDimension,
                           D3D11_UAV_DIMENSION uavDimension,
                           D3D11_SRV_DIMENSION srvDimension);

    // Not implemented
    Texture2D(const Texture2D&);
    Texture2D& operator=(const Texture2D&);

    Texture2DPtr m_pTexture;
    ShaderResourceViewPtr m_pShaderResource;

    // One per array element
    std::vector<RenderTargetViewPtr> mRenderTargetElements;
    std::vector<UnorderedAccessViewPtr> mUnorderedAccessElements;
    std::vector<ShaderResourceViewPtr> mShaderResourceElements;

	// Texture dims.
	int mWidth, mHeight;
};
typedef std::unique_ptr<Texture2D> CTexture2DPtr;


// Currently always float 32 as this one works best with sampling
// Optionally supports adding 8-bit stencil, but SRVs will only reference the 32-bit float part
class Depth2D
{
public:
    // Construct a Texture2D depth buffer
    Depth2D(const DevicePtr &pDevice,
            int width, int height,
            UINT bindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,
            bool stencil = false);

    // Construct a Texture2DMS depth buffer
    Depth2D(const DevicePtr &pDevice,
            int width, int height,
            UINT bindFlags,
            const DXGI_SAMPLE_DESC& sampleDesc,
            bool stencil = false);

    // Construct a Texture2DArray depth buffer
    Depth2D(const DevicePtr &pDevice,
            int width, int height,
            UINT bindFlags,
            int arraySize,
            bool stencil = false);

    // Construct a Texture2DMSArray depth buffer
    Depth2D(const DevicePtr &pDevice,
            int width, int height,
            UINT bindFlags,
            int arraySize, const DXGI_SAMPLE_DESC& sampleDesc,
            bool stencil = false);

    ~Depth2D();

	ID3D11Texture2D* GetTexture() { return m_pTexture.p; }
	ID3D11DepthStencilView* GetDepthStencil(std::size_t arrayIndex = 0) { return m_depthStencilElements[arrayIndex].p; }
	ID3D11ShaderResourceView* GetShaderResource() { return m_pShaderResource.p; }

private:
    void InternalConstruct(const DevicePtr &pDevice,
                           int width, int height,
                           UINT bindFlags, int arraySize,
                           int sampleCount, int sampleQuality,
                           D3D11_DSV_DIMENSION dsvDimension,
                           D3D11_SRV_DIMENSION srvDimension,
                           bool stencil);

    // Not implemented
    Depth2D(const Depth2D&);
    Depth2D& operator=(const Depth2D&);

    Texture2DPtr m_pTexture;
    ShaderResourceViewPtr m_pShaderResource;
    // One per array element
    std::vector<DepthStencilViewPtr> m_depthStencilElements;
};
typedef std::unique_ptr<Depth2D> CDepth2DPtr;
