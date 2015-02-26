// Various utilities for shaders - header file
#ifndef DX_SHADER_UTILS_H
#define DX_SHADER_UTILS_H

#include "d3dUtil.h"

#include "StructuredBuffer.h"

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

// Loads the texture from the given path
HRESULT loadTexture(const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext, const wpath texturePath, ID3D11Resource **pResource, ID3D11ShaderResourceView **pResourceView);

#endif	// DX_SHADER_UTILS_H