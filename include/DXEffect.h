// header file for the DXEffect class
#ifndef DX_EFFECT_H
#define DX_EFFECT_H

#include "d3dUtil.h"

#include "Light.h"

#include "StructuredBuffer.h"

// auto link against DXEffects lib
#ifndef DXEFFECTS_LIB_INTERNAL

#ifdef _DEBUG

#pragma message("Linking against DXEffectsd.lib")
#pragma comment(lib, "DXEffectsd.lib")

#else

#pragma message("Linking against DXEffects.lib")
#pragma comment(lib, "DXEffects.lib")

#endif	// _DEBUG

#endif	// DXEFFECTS_LIB_INTERNAL

// typedefs for simplicity
typedef D3D11_SHADER_BUFFER_DESC ShaderBufferDesc;
typedef D3D11_SHADER_VARIABLE_DESC ShaderVariableDesc;
typedef D3D11_SHADER_TYPE_DESC ShaderTypeDesc;
typedef D3D11_SHADER_INPUT_BIND_DESC ShaderInputBindDesc;
typedef ID3D11ShaderReflectionConstantBuffer* ShaderConstantBufferReflectorPtr;
typedef ID3D11ShaderReflectionVariable* ShaderVariableReflectorPtr;
typedef ID3D11ShaderReflectionType* ShaderTypeReflectorPtr;

// Base struct for shader variable descriptions.
struct ShaderVar
{
	// Default ctor
	ShaderVar() : strName(""), bindDesc()
	{}

	// Dtor
	virtual ~ShaderVar()	{}

	std::string strName;	// name of the shader variable
	ShaderInputBindDesc bindDesc;	// variable register binding info
};	// end of struct ShaderVar

// Basic shader constant buffer variable info element 
struct ShaderConstantBufferVariableInfoElement
{
	// Default ctor
	ShaderConstantBufferVariableInfoElement() : /*strVarName(""),*/ dataBuffer(), varDesc(), typeDesc()
	{}

	//std::string strVarName;
	std::vector<BYTE> dataBuffer;
	ShaderVariableDesc varDesc;
	ShaderTypeDesc typeDesc;
};	// end of struct ShaderConstantBufferVariableInfoElement

// Array to hold info of all shader constant buffer variables
typedef std::map<std::string, ShaderConstantBufferVariableInfoElement> ShaderConstantBufferVariableInfo;
//typedef std::vector<ShaderConstantBufferVariableInfoElement> ShaderConstantBufferVariableInfo;

// Constant buffer info
struct ShaderConstantBuffer : public ShaderVar
{
	// Buffer type
	enum BufferType
	{
		BT_CONSTANT,
		BT_STRUCTURED
	};

	// Default ctor
	ShaderConstantBuffer() : ShaderVar(), varsInfo(), pBuffer(), eBufferType(BT_CONSTANT)
	{}

	ShaderConstantBufferVariableInfo varsInfo;	// info about the constant buffer variables
	BufferPtr pBuffer;							// constant buffer
	BufferType eBufferType;

};	// end of struct ShaderConstantBuffer

// Structured buffer info
template<typename T>
struct ShaderStructuredBuffer : public ShaderVar
{
	// Default ctor
	ShaderStructuredBuffer() : ShaderVar(), pBuffer()	{}

	std::unique_ptr<StructuredBuffer<T>> pBuffer;

};	// end of struct ShaderStructuredBuffer

// Shader resource info
struct ShaderResource : public ShaderVar
{
	// Default ctor
	ShaderResource() : ShaderVar(), texturePath(), pResourceView(), pResource()
	{}

	const wpath texturePath;				// texture path
	ShaderResourceViewPtr pResourceView;	// shader resource view
	ResourcePtr pResource;					// texture resource

};	// end of struct ShaderResource

// Shader sampler state info
struct ShaderSamplerState : public ShaderVar
{
	// Default ctor
	ShaderSamplerState() : ShaderVar(), pSamplerState()	{}

	SamplerStatePtr pSamplerState;	// shader sampler state
};	// end of struct ShaderSamplerState

// Base class for Direct3D Effects.
// Contains common functions used by all Effects with pure virtual functions
// that each specific Effect will have to implement.
class DXEffect
{
public:

	// Param ctor
	explicit DXEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext);

	// Dtor
	virtual ~DXEffect();

	// Causes the effect to be applied. (virtual, may be implemented by derived class)
	virtual void apply();

	// Initialization function which creates all the shaders and initializes
	// all shader constant buffers, resources and sampler states.
	virtual void init();

	// Returns a string which contains effect info
	virtual std::wstring getEffectInfo();

	// Accessor function to access the input layout
	const InputLayoutPtr& getInputLayout()	const	{ return m_pInputLayout; }	

	// Accessor functions for the vertex shader's byte code buffer pointer and size.
	// WARNING: should only be called after the init() function has been invoked.
	virtual LPCVOID getVSBufferPointer() const = 0;
	virtual const SIZE_T getVSBufferSize() const = 0;

	// Unbinds all the shaders, resources and samplers that were bound to the pipeline by this effect.
	virtual void cleanup();
	
protected:	

	// All the constant buffers across all shaders are set. (pure virtual, must be implemented by derived class)
	virtual void setShaderConstantBuffers() = 0;

	// All the shader textures(resources) are set. (pure virtual, must be implemented by derived class)
	virtual void setShaderResources() = 0;

	// All the shader samplers are set. (pure virtual, must be implemented by derived class)
	virtual void setShaderSamplers() = 0;

	// All the shaders are set. (pure virtual, must be implemented by derived class)
	virtual void setShaders() = 0;

	// Creates all the shaders required for the effect, basically calls to create*Shader. (pure virtual, must be implemented by derived class)
	virtual void createShaders() = 0;

	// Initializes the input layout for the effect. (pure virtual, must be implemented by derived class)
	virtual void initInputLayout() = 0;

	// Creates and initializes the constant buffers of all shaders. (pure virtual, must be implemented by derived class)
	virtual void initShaderConstantBuffers() = 0;

	// Initializes the resources of all shaders. (pure virtual, must be implemented by derived class)
	virtual void initShaderResources() = 0;

	// Initializes the sampler states of all shaders. (pure virtual, must be implemented by derived class)
	virtual void initShaderSamplers() = 0;

	// Utility class to read the shader's compiled bytecode into the given buffer
	// source: http://gamedev.stackexchange.com/questions/49197/loading-a-vertex-shader-compiled-by-visual-studio-2012
	HRESULT getShaderByteCode(const wpath strShaderPath, BlobPtr &shaderBuf);

	// Utility function to construct an input layout using reflection
	// Source: http://takinginitiative.wordpress.com/2011/12/11/directx-1011-basic-shader-reflection-automatic-input-layout-creation/
	void createInputLayoutFromShaderInfo(const BlobPtr &pShaderBlob, InputLayoutPtr &pInputLayout);

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

	// Creates a vertex shader given the absolute path to its compiled bytecode file.
	void createVertexShader(const wpath strShaderPath, BlobPtr &pShaderByteCode, VertexShaderPtr &pVertexShader);

	// Creates a pixel shader given the absolute path to its compiled bytecode file.
	void createPixelShader(const wpath strShaderPath, BlobPtr &pShaderByteCode, PixelShaderPtr &pPixelShader);

	// Creates a geometry shader given the absolute path to its compiled bytecode file.
	void createGeometryShader(const wpath strShaderPath, BlobPtr &pShaderByteCode, GeometryShaderPtr &pGeometryShader);

	// Creates a hull shader given the absolute path to its compiled bytecode file.
	void createHullShader(const wpath strShaderPath, BlobPtr &pShaderByteCode, HullShaderPtr &pHullShader);

	// Creates a domain shader given the absolute path to its compiled bytecode file.
	void createDomainShader(const wpath strShaderPath, BlobPtr &pShaderByteCode, DomainShaderPtr &pDomainShader);

	// Creates a compute shader given the absolute path to its compiled bytecode file.
	void createComputeShader(const wpath strShaderPath, BlobPtr &pShaderByteCode, ComputeShaderPtr &pComputeShader);

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

	// Utility function to construct the absolute path to a shader file based on the current build configuration
	wpath constructShaderPath(const wpath &shaderRoot, const wpath &shaderFile);

	// Utility function to display the state of an effect
	// WARNING: should be enclosed within SpriteBatch::Begin() ... SpriteBatch::End()
	void displayEffectState(const SpriteBatchPtr &pSpriteBatch, const SpriteFontPtr &pSpriteFont, const wchar_t *pszEffectName,
		const bool bState, const DXVector2 &offset, const DXColor onColor = GREEN, const DXColor offColor = RED,
		const wchar_t *pszOnText = L"On", const wchar_t *pszOffText = L"Off");

protected:

	// D3D device
	DevicePtr m_pDevice;

	// D3D device immediate rendering context
	DeviceContextPtr m_pDeviceContext;

	// Shader root dir
	wpath m_shaderRoot;

	// input layout
	InputLayoutPtr m_pInputLayout;
	
};	// end of class DXEffect

// Utility function to set a specified constant buffer member array's data buffer with the given data
template<typename T>
void DXEffect::setConstantBufferVariableArray(const std::vector<T> &contents, const int nMaxSize, const std::string &strArrayIndex, ShaderConstantBuffer &constantBuffer)
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

	const std::size_t nPaddedSize = sizeof(T)+nPad;	// size of a T instance inclusive of padding
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
void DXEffect::createStructuredBuffer(const BlobPtr &pShaderBlob, const std::string &bufName, const UINT nElements, const UINT bindFlags,
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
void DXEffect::setStructuredBuffer(const std::vector<T> &elements, const UINT nElements, ShaderStructuredBuffer<T> &structuredBuffer)
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
void DXEffect::setConstantBufferVariableTypedDatum(const T &datum, const std::string &strIndex, ShaderConstantBuffer &constantBuffer)
{
	setConstantBufferVariableData(reinterpret_cast<const BYTE*>(&datum), sizeof(datum), strIndex, constantBuffer);
}

#endif	// DX_EFFECT_H