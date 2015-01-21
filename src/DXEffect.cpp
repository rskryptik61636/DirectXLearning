// Source file of the DXEffect class

#include "DXEffect.h"

// Param ctor
DXEffect::DXEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext) 
: m_shaderRoot(shaderRoot), m_pDevice(pDevice), m_pDeviceContext(pDeviceContext)
{
	// NOTE: BAD idea to call init here as the object will not have been fully constructed
}

// Dtor
DXEffect::~DXEffect()	{}

// Causes the effect to be applied. (virtual, may be implemented by derived class)
void DXEffect::apply()
{
	// set the shaders
	setShaders();

	// set the shader constant buffers
	setShaderConstantBuffers();

	// set the shader resources
	setShaderResources();

	// set the shader samplers
	setShaderSamplers();
}

// Initialization function which creates all the shaders and initializes
// all shader constant buffers, resources and sampler states.
void DXEffect::init()
{
	createShaders();	// create all the shaders

	initInputLayout();	// initialize the input layout

	initShaderConstantBuffers();	// initialize all the shader contant buffers

	initShaderResources();	// initialize all the shader resources

	initShaderSamplers();	// initialize all the shader samplers
}

// Returns a string which contains effect info
std::wstring DXEffect::getEffectInfo()	{ return L""; }

// Utility class to read the shader's compiled bytecode into the given buffer
// source: http://gamedev.stackexchange.com/questions/49197/loading-a-vertex-shader-compiled-by-visual-studio-2012
HRESULT DXEffect::getShaderByteCode(const wpath strShaderPath, BlobPtr &shaderBuf)
{
	std::ifstream shaderStream;
	std::size_t bufSize;
	//char* vs_data;

	shaderStream.open(strShaderPath.file_string(), std::ifstream::in | std::ifstream::binary);
	if (shaderStream.good())
	{
		shaderStream.seekg(0, std::ios::end);
		bufSize = size_t(shaderStream.tellg());
		//shaderBuf.resize(bufSize);
		HRESULT hr = D3DCreateBlob(bufSize, &shaderBuf.p);
		if (hr != S_OK)
			return hr;

		shaderStream.seekg(0, std::ios::beg);
		shaderStream.read((char*)shaderBuf->GetBufferPointer(), bufSize);
		shaderStream.close();
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}

// Utility function to construct an input layout using reflection
// Source: http://takinginitiative.wordpress.com/2011/12/11/directx-1011-basic-shader-reflection-automatic-input-layout-creation/
void DXEffect::createInputLayoutFromShaderInfo(const BlobPtr &pShaderBlob, InputLayoutPtr &pInputLayout)
{
	// Reflect shader info
	ShaderReflectionPtr pReflector;
	HR(D3DReflect(pShaderBlob->GetBufferPointer(),						// pointer to shader bytecode buffer
		pShaderBlob->GetBufferSize(),							// size in bytes of shader bytecode buffer
		IID_ID3D11ShaderReflection,								// GUID of the shader reflection interface
		reinterpret_cast<void**>(&pReflector.p)));	// pointer to shader reflection interface

	// Get shader info
	D3D11_SHADER_DESC shaderDesc;
	pReflector->GetDesc(&shaderDesc);

	// Read input layout description from shader info
	UINT byteOffset = 0;
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (UINT i = 0; i< shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		pReflector->GetInputParameterDesc(i, &paramDesc);

		// fill out input element desc
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = byteOffset;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		// determine DXGI format
		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			byteOffset += 4;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			byteOffset += 8;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			byteOffset += 12;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			byteOffset += 16;
		}

		//save element desc
		inputLayoutDesc.push_back(elementDesc);
	}

	// Try to create Input Layout
	HR(m_pDevice->CreateInputLayout(inputLayoutDesc.data(),			// base address of input layout desc array
		inputLayoutDesc.size(),				// no. of elements in input layout desc array
		pShaderBlob->GetBufferPointer(),	// pointer to shader bytecode buffer
		pShaderBlob->GetBufferSize(),		// size of shader bytecode buffer in bytes
		&pInputLayout.p));				// input vertex layout interface

	//Free allocation shader reflection memory
	pReflector.Release();
}

// Creates a constant buffer of the specified size.
void DXEffect::createConstantBuffer(const BlobPtr &pShaderBlob, const std::string &bufName, ShaderConstantBuffer &constantBuffer)
{
	// reflect the shader and get the constant buffer bufName's info
	ShaderReflectionPtr pReflector;
	HR(D3DReflect(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&pReflector.p)));

	ShaderBufferDesc constBufDesc;
	ShaderConstantBufferReflectorPtr pConstBuf = pReflector->GetConstantBufferByName(bufName.c_str());
	HR(pConstBuf->GetDesc(&constBufDesc));

	// init the constant buffer with the binding info
	ShaderInputBindDesc constBufBindDesc;
	HR(pReflector->GetResourceBindingDescByName(bufName.c_str(), &constBufBindDesc));

	// init the constant buffer instance
	constantBuffer.strName = bufName;
	constantBuffer.bindDesc = constBufBindDesc;

	// get the info of all the constant buffer variables
	for(UINT i = 0; i < constBufDesc.Variables; ++i)
	{
		// get the variable info
		ShaderVariableReflectorPtr pVar = pConstBuf->GetVariableByIndex(i);
		ShaderVariableDesc varDesc;
		HR(pVar->GetDesc(&varDesc));
		const std::string strVarName(varDesc.Name);	// get the variable's name which will be the key for the current entry
		constantBuffer.varsInfo[strVarName].varDesc = varDesc;

		// get the type info
		ShaderTypeReflectorPtr pType = pVar->GetType();
		ShaderTypeDesc typeDesc;
		HR(pType->GetDesc(&typeDesc));
		constantBuffer.varsInfo[strVarName].typeDesc = typeDesc;
	}

	// release the reflection interface
	pReflector.Release();

	D3D11_BUFFER_DESC bufDesc;
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// bind as a constant buffer
	bufDesc.ByteWidth = constBufDesc.Size;						// buffer size
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// we're going to be mapping this buffer to app memory every time its contents need to be updated
	bufDesc.MiscFlags = 0;							// no misc flags
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;				// it is going to be updated on the fly
	HR(m_pDevice->CreateBuffer(&bufDesc, 0, &constantBuffer.pBuffer.p));	// create the buffer
}

// Populates a constant buffer's data buffer with its variables' data buffer contents.
void DXEffect::populateConstantBufferData(ShaderConstantBuffer &constantBuffer)
{
	// map the constant buffer to a mapped subresource object
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HR(m_pDeviceContext->Map(constantBuffer.pBuffer.p, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource));

	// copy each of the constant buffer's variable's data buffers into its appropriate location within the mapped subresource's data buffer
	for (ShaderConstantBufferVariableInfo::iterator i = constantBuffer.varsInfo.begin(); i != constantBuffer.varsInfo.end(); ++i)
	{
		const std::ptrdiff_t nOffset = static_cast<std::ptrdiff_t>(i->second.varDesc.StartOffset);	// base address byte offset
		const SIZE_T nSize = static_cast<SIZE_T>(i->second.varDesc.Size);	// size of the current variable's data buffer in bytes
		BYTE *pDst = reinterpret_cast<BYTE*>(mappedSubresource.pData) + nOffset;	// destination address
		const BYTE *pSrc = i->second.dataBuffer.data();	// source address
		HR(i->second.dataBuffer.size() == nSize);	// ensure that the data buffer being copied is of the right size
		CopyMemory(pDst, pSrc, nSize);	// copy the memory from the source to the destination address
	}
	
	// unmap the subresource
	m_pDeviceContext->Unmap(constantBuffer.pBuffer.p, 0);
}

// Creates a resource view
void DXEffect::createResource(const wpath texturePath, const std::string &strResourceName, const BlobPtr &pShaderBlob, ShaderResource &resource)
{
	// determine whether the texture is a .dds file or not and load the texture accordingly
	const bool bTextureIsDDS = texturePath.extension() == L".dds";
	if (bTextureIsDDS)
	{
		HR(DirectX::CreateDDSTextureFromFile(m_pDevice, texturePath.file_string().c_str(), &resource.pResource.p, &resource.pResourceView.p));
	}
	else
	{
		HR(DirectX::CreateWICTextureFromFile(m_pDevice, m_pDeviceContext, texturePath.file_string().c_str(), &resource.pResource.p, &resource.pResourceView.p));
	}

	// reflect the shader and get the resource's bind desc
	ShaderReflectionPtr pReflector;
	HR(D3DReflect(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&pReflector.p)));
	pReflector->GetResourceBindingDescByName(strResourceName.c_str(), &resource.bindDesc);
	pReflector.Release();

}

// Creates a vertex shader given the absolute path to its compiled bytecode file.
void DXEffect::createVertexShader(const wpath strShaderPath, BlobPtr &pShaderByteCode, VertexShaderPtr &pVertexShader)
{
	// get the shader byte code from the given path
	HR(getShaderByteCode(strShaderPath, pShaderByteCode));

	// create the vertex shader
	HR(m_pDevice->CreateVertexShader(pShaderByteCode->GetBufferPointer(), pShaderByteCode->GetBufferSize(), 0, &pVertexShader.p));
}

// Creates a pixel shader given the absolute path to its compiled bytecode file.
void DXEffect::createPixelShader(const wpath strShaderPath, BlobPtr &pShaderByteCode, PixelShaderPtr &pPixelShader)
{
	// get the shader byte code from the given path
	HR(getShaderByteCode(strShaderPath, pShaderByteCode));

	// create the pixel shader
	HR(m_pDevice->CreatePixelShader(pShaderByteCode->GetBufferPointer(), pShaderByteCode->GetBufferSize(), 0, &pPixelShader.p));
}

// Creates a geometry shader given the absolute path to its compiled bytecode file.
void DXEffect::createGeometryShader(const wpath strShaderPath, BlobPtr &pShaderByteCode, GeometryShaderPtr &pGeometryShader)
{
	// get the shader byte code from the given path
	HR(getShaderByteCode(strShaderPath, pShaderByteCode));

	// create the geometry shader
	HR(m_pDevice->CreateGeometryShader(pShaderByteCode->GetBufferPointer(), pShaderByteCode->GetBufferSize(), 0, &pGeometryShader.p));
}

// Creates a hull shader given the absolute path to its compiled bytecode file.
void DXEffect::createHullShader(const wpath strShaderPath, BlobPtr &pShaderByteCode, HullShaderPtr &pHullShader)
{
	// get the shader byte code from the given path
	HR(getShaderByteCode(strShaderPath, pShaderByteCode));

	// create the hull shader
	HR(m_pDevice->CreateHullShader(pShaderByteCode->GetBufferPointer(), pShaderByteCode->GetBufferSize(), 0, &pHullShader.p));
}

// Creates a domain shader given the absolute path to its compiled bytecode file.
void DXEffect::createDomainShader(const wpath strShaderPath, BlobPtr &pShaderByteCode, DomainShaderPtr &pDomainShader)
{
	// get the shader byte code from the given path
	HR(getShaderByteCode(strShaderPath, pShaderByteCode));

	// create the domain shader
	HR(m_pDevice->CreateDomainShader(pShaderByteCode->GetBufferPointer(), pShaderByteCode->GetBufferSize(), 0, &pDomainShader.p));
}

// Creates a compute shader given the absolute path to its compiled bytecode file.
void DXEffect::createComputeShader(const wpath strShaderPath, BlobPtr &pShaderByteCode, ComputeShaderPtr &pComputeShader)
{
	// get the shader byte code from the given path
	HR(getShaderByteCode(strShaderPath, pShaderByteCode));

	// create the compute shader
	HR(m_pDevice->CreateComputeShader(pShaderByteCode->GetBufferPointer(), pShaderByteCode->GetBufferSize(), 0, &pComputeShader.p));
}

// Utility function to set a specified constant buffer variable's data buffer with the given matrix
void DXEffect::setConstantBufferVariableDataWithMatrix(const DXMatrix &matrix, const std::string &strIndex, ShaderConstantBuffer &constantBuffer, const bool bTranspose /*= true*/)
{
	const std::size_t nSize = bTranspose ? sizeof(matrix.Transpose()) : sizeof(matrix);	// size of the matrix in bytes
	const BYTE *pSrc = reinterpret_cast<const BYTE*>(bTranspose ? &matrix.Transpose() : &matrix);
	setConstantBufferVariableData(pSrc, nSize, strIndex, constantBuffer);		
}

// Utility function to set a specified constant buffer variable's data buffer with the given data
void DXEffect::setConstantBufferVariableData(const BYTE* pSrc, const std::size_t nSize, const std::string &strIndex, ShaderConstantBuffer &constantBuffer)
{
	assert(nSize == constantBuffer.varsInfo[strIndex].varDesc.Size);	// ensure that the input data buffer is of the correct size
	constantBuffer.varsInfo[strIndex].dataBuffer.resize(nSize);	// resize the corresponding constant buffer variable's data buffer
	BYTE *pDst = constantBuffer.varsInfo[strIndex].dataBuffer.data();	// set the destination pointer
	CopyMemory(pDst, pSrc, nSize);	// copy the input data buffer into the constant buffer variable's data buffer
}

// Utility function to set Lights in the given shader's constant buffer
void DXEffect::setLights(const std::vector<Light> &lights, const int nMaxLights, const std::string &strLightsIndex, const std::string &strNumLightsIndex, ShaderConstantBuffer &constantBuffer)
{	
	// set the lights array in the constant buffer
	setConstantBufferVariableArray<Light>(lights, nMaxLights, strLightsIndex, constantBuffer);

	// set the no. of lights in the constant buffer
	const int nLights = lights.size();	// no. of lights
	const BYTE *pSrc2 = reinterpret_cast<const BYTE*>(&nLights);
	const std::size_t nSize2 = sizeof(nLights);
	setConstantBufferVariableData(pSrc2, nSize2, strNumLightsIndex, constantBuffer);
}

// Utility function to set Lights in the given shader's structured buffer
// NOTE: lights.size() == nLights for this to work.
void DXEffect::setStructuredLights(const std::vector<SLight> &lights, const UINT nLights, ShaderStructuredBuffer<SLight> &lightBuffer)
{
	setStructuredBuffer<SLight>(lights, nLights, lightBuffer);
}

// Utility function to set matrices in the given shader's structured buffer
void DXEffect::setStructuredMatrices(const std::vector<DXMatrix> &matrices, const UINT nMatrices, ShaderStructuredBuffer<DXMatrix> &matrixBuffer, const bool bTranspose /*= true*/)
{
	if (bTranspose)	// transpose the matrices iff required
	{
		std::vector<DXMatrix> tmp(matrices.size());
		for (std::size_t i = 0; i < tmp.size(); ++i)
			tmp[i] = matrices[i].Transpose();
		setStructuredBuffer<DXMatrix>(tmp, nMatrices, matrixBuffer);
	}
	else
		setStructuredBuffer<DXMatrix>(matrices, nMatrices, matrixBuffer);
}

// Utility function to construct the absolute path to a shader file based on the current build configuration
wpath DXEffect::constructShaderPath(const wpath &shaderRoot, const wpath &shaderFile)
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

// Unbinds all the shaders, resources and samplers that were bound to the pipeline by this effect.
void DXEffect::cleanup()	{}

// Utility function to display the state of an effect
// WARNING: should be enclosed within SpriteBatch::Begin() ... SpriteBatch::End()
void DXEffect::displayEffectState(const SpriteBatchPtr &pSpriteBatch, const SpriteFontPtr &pSpriteFont, const wchar_t *pszEffectName,
	const bool bState, const DXVector2 &offset, const DXColor onColor /*= GREEN*/, const DXColor offColor /*= RED*/,
	const wchar_t *pszOnText /*= L"On"*/, const wchar_t *pszOffText /*= L"Off"*/)
{
	pSpriteFont->DrawString(pSpriteBatch.get(),
		std::wstring(pszEffectName + std::wstring(bState ? pszOnText : pszOffText)).c_str(),
		offset, bState ? onColor : offColor);
}