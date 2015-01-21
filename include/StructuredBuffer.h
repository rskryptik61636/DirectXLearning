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

// Modified by Rohit S to work with the DXLearning framework (Mar-16-2014)

// Modified by Rohit S w/ another ctor to accept initial data (Mar-29-2014)

// Modified by Rohit S w/ the ability to use the StructuredBuffer as an Append/ConsumeStructuredBuffer (Mar-31-2014)

#pragma once

//#include <d3d11.h>
//#include <vector>
#include "d3dUtil.h"

// NOTE: Ensure that T is exactly the same size/layout as the shader structure!
template <typename T>
class StructuredBuffer
{
public:
    // Construct a structured buffer
    StructuredBuffer(const DevicePtr &d3dDevice, int elements,
                     UINT bindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
                     bool dynamic = false, bool appendConsume = false);

	// Construct a structured buffer which is initialized with pData
	StructuredBuffer(const DevicePtr &d3dDevice, int elements,		
		const T* pData,
		UINT bindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
		bool dynamic = false, bool appendConsume = false);
    
    ~StructuredBuffer();

    ID3D11Buffer* GetBuffer() { return mBuffer.p; }
    ID3D11UnorderedAccessView* GetUnorderedAccess() { return mUnorderedAccess.p; }
    ID3D11ShaderResourceView* GetShaderResource() { return mShaderResource.p; }

    // Only valid for dynamic buffers
    // TODO: Support NOOVERWRITE ring buffer?
    T* MapDiscard(const DeviceContextPtr &d3dDeviceContext);
    void Unmap(const DeviceContextPtr &d3dDeviceContext);

private:

	// Internal ctor
	void InternalConstruct(const DevicePtr &d3dDevice, int elements,
		const T* pData,
		UINT bindFlags,
		bool dynamic,
		bool appendConsume);

    // Not implemented
    StructuredBuffer(const StructuredBuffer&);
    StructuredBuffer& operator=(const StructuredBuffer&);

    int mElements;
    BufferPtr mBuffer;
    ShaderResourceViewPtr mShaderResource;
    UnorderedAccessViewPtr mUnorderedAccess;
};

//// Structured Buffer smart pointer
//template<typename T>
//typedef std::unique_ptr<StructuredBuffer<T>> StructuredBufferPtr;


template <typename T>
StructuredBuffer<T>::StructuredBuffer(const DevicePtr &d3dDevice, int elements,
	UINT bindFlags, bool dynamic, bool appendConsume /*= false*/)
    : mElements(elements), mShaderResource(0), mUnorderedAccess(0)
{
	// Invoke the internal constructor
	InternalConstruct(d3dDevice, elements, NULL, bindFlags, dynamic, appendConsume);

	// @TODO: remove when done testing
#if 0
	CD3D11_BUFFER_DESC desc(sizeof(T)* elements, bindFlags,
		dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT,
		dynamic ? D3D11_CPU_ACCESS_WRITE : 0,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		sizeof(T));

	HR(d3dDevice->CreateBuffer(&desc, 0, &mBuffer.p));

	if (bindFlags & D3D11_BIND_UNORDERED_ACCESS) {
		HR(d3dDevice->CreateUnorderedAccessView(mBuffer, 0, &mUnorderedAccess.p));
	}

	if (bindFlags & D3D11_BIND_SHADER_RESOURCE) {
		HR(d3dDevice->CreateShaderResourceView(mBuffer, 0, &mShaderResource.p));
	}
#endif // 0

}

// Construct a structured buffer which is initialized with pData
template<typename T>
StructuredBuffer<T>::StructuredBuffer(const DevicePtr &d3dDevice, int elements,
	const T* pData,
	UINT bindFlags /*= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE*/,
	bool dynamic /*= false*/, bool appendConsume /*= false*/)
{
	// Invoke the internal constructor
	InternalConstruct(d3dDevice, elements, pData, bindFlags, dynamic, appendConsume);
}

// Internal ctor
template<typename T>
void StructuredBuffer<T>::InternalConstruct(const DevicePtr &d3dDevice, int elements,
	const T* pData,
	UINT bindFlags,
	bool dynamic,
	bool appendConsume)
{
	CD3D11_BUFFER_DESC desc(sizeof(T)* elements, bindFlags,
		dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT,
		dynamic ? D3D11_CPU_ACCESS_WRITE : 0,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		sizeof(T));

	// Initialize with the given data iff provided
	if (pData != NULL)
	{
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = pData;
		data.SysMemPitch = sizeof(T);

		HR(d3dDevice->CreateBuffer(&desc, &data, &mBuffer.p));
	}	
	else
		HR(d3dDevice->CreateBuffer(&desc, NULL, &mBuffer.p));

	if (bindFlags & D3D11_BIND_UNORDERED_ACCESS) {
		if (appendConsume)	// Bind with the D3D11_BUFFER_UAV_FLAG_APPEND flag so that it can be used as an Append/ConsumeStructuredBuffer.
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			desc.Buffer.FirstElement = 0;
			desc.Buffer.NumElements = elements;
			desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
			HR(d3dDevice->CreateUnorderedAccessView(mBuffer, &desc, &mUnorderedAccess.p));
		}
		else
			HR(d3dDevice->CreateUnorderedAccessView(mBuffer, 0, &mUnorderedAccess.p));
	}

	if (bindFlags & D3D11_BIND_SHADER_RESOURCE) {
		HR(d3dDevice->CreateShaderResourceView(mBuffer, 0, &mShaderResource.p));
	}
}


template <typename T>
StructuredBuffer<T>::~StructuredBuffer()
{
    if (mUnorderedAccess) mUnorderedAccess.Release();
    if (mShaderResource) mShaderResource.Release();
    mBuffer.Release();
}


template <typename T>
T* StructuredBuffer<T>::MapDiscard(const DeviceContextPtr &d3dDeviceContext)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HR(d3dDeviceContext->Map(mBuffer.p, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
    return static_cast<T*>(mappedResource.pData);
}


template <typename T>
void StructuredBuffer<T>::Unmap(const DeviceContextPtr &d3dDeviceContext)
{
    d3dDeviceContext->Unmap(mBuffer.p, 0);
}


// TODO: Constant buffers
