#ifndef DX_SHADERS_H
#define DX_SHADERS_H

#include "DXShaderUtils.h"

#include "Light.h"

/**********************************************************************
* Start of shader base classes
**********************************************************************/

// Base class for all shaders. Contains all commonly used functionality.
class DXShaderBase
{
public:

	// Ctor.
	explicit DXShaderBase(const wpath shaderPath, const DevicePtr &pDevice);

	// Dtor.
	virtual ~DXShaderBase();

	// Accessor function for the shader's byte code.
	const BlobPtr& byteCode() const
	{
		return m_pShaderByteCode;
	}

protected:

	// Utility class to read the shader's compiled bytecode into the given buffer
	// source: http://gamedev.stackexchange.com/questions/49197/loading-a-vertex-shader-compiled-by-visual-studio-2012
	HRESULT getShaderByteCode(const wpath strShaderPath, BlobPtr &shaderBuf);

	// Creates a constant buffer of the specified size.
	void createConstantBuffer(const BlobPtr &pShaderBlob, const std::string &bufName, ShaderConstantBuffer &constantBuffer);

	// Populates a constant buffer's data buffer with its variables' data buffer contents.
	void populateConstantBufferData(ShaderConstantBuffer &constantBuffer);

	// Creates a structured buffer of the required size to contain the specified no. of elements as well as with the given bind flags.
	// NOTE: added pInitData to specify init data, should contain nElements (Mar-29-2014)
	template<typename T>
	void createStructuredBuffer(const BlobPtr &pShaderBlob, const std::string &bufName, const UINT nElements, const UINT bindFlags,
		ShaderStructuredBuffer<T> &structuredBuffer, const bool bIsDynamic = true, const T* pInitData = NULL, const bool bAppendConsume = false);

	// Creates a resource view
	void createResource(const wpath texturePath, const std::string &strResourceName, const BlobPtr &pShaderBlob, ShaderResource &resource);

	// Utility function to set a specified constant buffer variable's data buffer with the given matrix
	void setConstantBufferVariableDataWithMatrix(const DXMatrix &matrix, const std::string &strIndex, ShaderConstantBuffer &constantBuffer, const bool bTranspose = true);

	// Utility function to set a specified constant buffer variable's data buffer with the given data
	void setConstantBufferVariableData(const BYTE* pSrc, const std::size_t nSize, const std::string &strIndex, ShaderConstantBuffer &constantBuffer);

	// Utility function to set a specified constant buffer variable's data buffer with the given typed datum
	template<typename T>
	void setConstantBufferVariableTypedDatum(const T &datum, const std::string &strIndex, ShaderConstantBuffer &constantBuffer);

	// Utility function to set a specified constant buffer member array's data buffer with the given data
	template<typename T>
	void setConstantBufferVariableArray(const std::vector<T> &contents, const int nMaxSize, const std::string &strArrayIndex, ShaderConstantBuffer &constantBuffer);

	// Utility function to set Lights in the given shader's constant buffer
	void setLights(const std::vector<Light> &lights, const int nMaxLights, const std::string &strLightsIndex, const std::string &strNumLightsIndex, ShaderConstantBuffer &constantBuffer);

	// Utility function to set Lights in the given shader's structured buffer
	// NOTE: lights.size() == nLights for this to work.
	void setStructuredLights(const std::vector<SLight> &lights, const UINT nLights, ShaderStructuredBuffer<SLight> &lightBuffer);

	// Utility function to set matrices in the given shader's structured buffer
	void setStructuredMatrices(const std::vector<DXMatrix> &matrices, const UINT nMatrices, ShaderStructuredBuffer<DXMatrix> &matrixBuffer, const bool bTranspose = true);

	// Utility function to set a specified structured buffer's data buffer with the given elements
	template<typename T>
	void setStructuredBuffer(const std::vector<T> &elements, const UINT nElements, ShaderStructuredBuffer<T> &structuredBuffer);
	
protected:

	// D3D device
	DevicePtr m_pDevice;

	// D3D device immediate rendering context
	DeviceContextPtr m_pDeviceContext;

	// Shader byte code.
	BlobPtr m_pShaderByteCode;

	// Shader file path.
	wpath m_shaderPath;
};


// Utility function to set a specified constant buffer member array's data buffer with the given data
template<typename T>
void DXShaderBase::setConstantBufferVariableArray(const std::vector<T> &contents, const int nMaxSize, const std::string &strArrayIndex, ShaderConstantBuffer &constantBuffer)
{
	const std::size_t nContents = contents.size();
	assert(nContents <= nMaxSize);	// ensure that the no. of elements in 'contents' is within the maximum allowed range

	// NOTE: The array of T objects in the shader follows certain packing rules.
	// Each array element is padded such that it is aligned with a 16-byte boundary
	// -since each register is a vec4. All elements save for the last one.
	//
	// This is probably to save on an unnecessary extra allocation in case the next
	// -variable (which is also the last) in the constant buffer fits inside the
	// -memory space of the last array element.
	//
	// In order to account for this, all n-1 elements of the T array must be
	// -padded accordingly.

	// no. of padding bytes required to align a T instance with a 16 byte boundary
	const std::size_t nPad = static_cast<std::size_t>(ceilf(static_cast<float>(sizeof(T)) / 16.0f)) * 16 - sizeof(T);

	const std::size_t nPaddedSize = sizeof(T) + nPad;	// size of a T instance inclusive of padding
	std::vector<BYTE> allElements((nMaxSize - 1 > 0 ? nMaxSize - 1 : 0) * nPaddedSize + sizeof(T), 0);	// create the buffer of appropriate size
	std::ptrdiff_t nOffset = 0;	// pointer offset for each array element
	if (nContents > 0)
	{
		for (std::size_t i = 0; i < nContents - 1; ++i)
		{
			CopyMemory(allElements.data() + nOffset,	// destination address
				reinterpret_cast<const BYTE*>(&contents[i]),	// source address
				sizeof(T));	// no. of bytes to be copied (not including padding since we're copying just the T instance)
			nOffset += nPaddedSize;	// increment the pointer offset
		}
		CopyMemory(allElements.data() + nOffset, reinterpret_cast<const BYTE*>(&contents[nContents - 1]), sizeof(T));	// copy the last element w/o padding
	}

	// set the contents buffer in the constant buffer
	const BYTE *pSrc1 = reinterpret_cast<const BYTE*>(allElements.data());
	const std::size_t nSize1 = allElements.size()/* * sizeof(T)*/;
	setConstantBufferVariableData(pSrc1, nSize1, strArrayIndex, constantBuffer);
}

// Creates a structured buffer of the required size to contain the specified no. of elements as well as with the given bind flags.
template<typename T>
void DXShaderBase::createStructuredBuffer(const BlobPtr &pShaderBlob, const std::string &bufName, const UINT nElements, const UINT bindFlags,
	ShaderStructuredBuffer<T> &structuredBuffer, const bool bIsDynamic /*= true*/, const T* pInitData /*= NULL*/, const bool bAppendConsume /*= false*/)
{
	// reflect the shader and get the structured buffer bufName's info
	ShaderReflectionPtr pReflector;
	HR(D3DReflect(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&pReflector.p)));

	ShaderBufferDesc structuredBufDesc;
	ShaderConstantBufferReflectorPtr pConstBuf = pReflector->GetConstantBufferByName(bufName.c_str());
	HR(pConstBuf->GetDesc(&structuredBufDesc));

	// init the constant buffer with the binding info
	ShaderInputBindDesc structuredBufBindDesc;
	HR(pReflector->GetResourceBindingDescByName(bufName.c_str(), &structuredBufBindDesc));

	// init the constant buffer instance
	structuredBuffer.strName = bufName;
	structuredBuffer.bindDesc = structuredBufBindDesc;

	// close the reflection interface
	pReflector.Release();

	// instantiate the structured buffer iff the no. of elements is > 0
	if (nElements > 0)
	{
		//const bool bIsDynamic(true);	// NOTE: revise if necessary (now a default param with the same value, 28-Mar-2014)
		structuredBuffer.pBuffer.reset(new StructuredBuffer<T>(m_pDevice, nElements, pInitData, bindFlags, bIsDynamic, bAppendConsume));
	}
	else
		structuredBuffer.pBuffer.reset();
}

// Utility function to set a specified structured buffer's data buffer with the given elements
template<typename T>
void DXShaderBase::setStructuredBuffer(const std::vector<T> &elements, const UINT nElements, ShaderStructuredBuffer<T> &structuredBuffer)
{
	// Ensure that the elements vector is of the right size.
	assert(elements.size() == nElements);

	// Update the structured buffer iff there is atleast one element
	if (nElements > 0)
	{
		T* pBuffer = structuredBuffer.pBuffer->MapDiscard(m_pDeviceContext);
		for (std::size_t i = 0; i < elements.size(); ++i)
			pBuffer[i] = elements[i];
		structuredBuffer.pBuffer->Unmap(m_pDeviceContext);
	}
}

// Utility function to set a specified constant buffer variable's data buffer with the given typed datum
template<typename T>
void DXShaderBase::setConstantBufferVariableTypedDatum(const T &datum, const std::string &strIndex, ShaderConstantBuffer &constantBuffer)
{
	setConstantBufferVariableData(reinterpret_cast<const BYTE*>(&datum), sizeof(datum), strIndex, constantBuffer);
}

// Vertex shader base class.
class DXVertexShader : public DXShaderBase
{
public:

	// Ctor.
	explicit DXVertexShader(const wpath &shaderPath, const DevicePtr &pDevice);

	// Dtor.
	virtual ~DXVertexShader();

	// Accessor function to access the input layout
	const InputLayoutPtr& inputLayout()	const	{ return m_pInputLayout; }

	// Accessor function for the raw pixel shader.
	VertexShaderRawPtr pShader() const
	{
		return m_pShader.p;
	}

protected:

	// Utility function to construct an input layout using reflection
	// Source: http://takinginitiative.wordpress.com/2011/12/11/directx-1011-basic-shader-reflection-automatic-input-layout-creation/
	void createInputLayout();

	//// Creates a vertex shader given the absolute path to its compiled bytecode file.
	//void createVertexShader(const wpath strShaderPath, BlobPtr &pShaderByteCode, VertexShaderPtr &pVertexShader);

protected:

	// Shader.
	VertexShaderPtr m_pShader;

	// Input layout.
	InputLayoutPtr m_pInputLayout;

};	// end of class DXVertexShader
typedef std::unique_ptr<DXVertexShader> DXVertexShaderPtr;

// Pixel shader base class.
class DXPixelShader : public DXShaderBase
{
public:

	// Ctor.
	explicit DXPixelShader(const wpath &shaderPath, const DevicePtr &pDevice);

	// Dtor.
	virtual ~DXPixelShader();

	// Accessor function for the raw pixel shader.
	PixelShaderRawPtr pShader() const
	{
		return m_pShader.p;
	}


protected:

	// TODO: Add implementation here.

protected:

	// Shader.
	PixelShaderPtr m_pShader;

};	// end of class DXPixelShader
typedef std::unique_ptr<DXPixelShader> DXPixelShaderPtr;

// TODO: Add classes for the remaining types of shaders.

/**********************************************************************
* End of shader base classes
**********************************************************************/

#endif	// DX_SHADERS_H