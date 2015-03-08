#include "DXShaders.h"

/**********************************************************************
* Start of shader base classes
**********************************************************************/

// Ctor.
/*explicit*/ DXShaderBase::DXShaderBase(const wpath shaderPath, const DevicePtr &pDevice)
	: m_shaderPath(shaderPath), m_pDevice(pDevice)
{
	// Get the immediate device context.
	m_pDevice->GetImmediateContext(&m_pDeviceContext.p);

	// Get the shader's byte code.
	HR(getShaderByteCode(m_shaderPath, m_pShaderByteCode));
}

// Dtor.
/*virtual*/ DXShaderBase::~DXShaderBase()
{}

// Utility class to read the shader's compiled bytecode into the given buffer
// source: http://gamedev.stackexchange.com/questions/49197/loading-a-vertex-shader-compiled-by-visual-studio-2012
HRESULT DXShaderBase::getShaderByteCode(const wpath strShaderPath, BlobPtr &shaderBuf)
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

// Creates a constant buffer of the specified size.
void DXShaderBase::createConstantBuffer(const BlobPtr &pShaderBlob, const std::string &bufName, ShaderConstantBuffer &constantBuffer)
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
	for (UINT i = 0; i < constBufDesc.Variables; ++i)
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
void DXShaderBase::populateConstantBufferData(ShaderConstantBuffer &constantBuffer)
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
void DXShaderBase::createResource(const wpath texturePath, const std::string &strResourceName, const BlobPtr &pShaderBlob, ShaderResource &resource)
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

// Utility function to set a specified constant buffer variable's data buffer with the given matrix
void DXShaderBase::setConstantBufferVariableDataWithMatrix(const DXMatrix &matrix, const std::string &strIndex, ShaderConstantBuffer &constantBuffer, const bool bTranspose /*= true*/)
{
	const std::size_t nSize = bTranspose ? sizeof(matrix.Transpose()) : sizeof(matrix);	// size of the matrix in bytes
	const BYTE *pSrc = reinterpret_cast<const BYTE*>(bTranspose ? &matrix.Transpose() : &matrix);
	setConstantBufferVariableData(pSrc, nSize, strIndex, constantBuffer);
}

// Utility function to set a specified constant buffer variable's data buffer with the given data
void DXShaderBase::setConstantBufferVariableData(const BYTE* pSrc, const std::size_t nSize, const std::string &strIndex, ShaderConstantBuffer &constantBuffer)
{
	assert(nSize == constantBuffer.varsInfo[strIndex].varDesc.Size);	// ensure that the input data buffer is of the correct size
	constantBuffer.varsInfo[strIndex].dataBuffer.resize(nSize);	// resize the corresponding constant buffer variable's data buffer
	BYTE *pDst = constantBuffer.varsInfo[strIndex].dataBuffer.data();	// set the destination pointer
	CopyMemory(pDst, pSrc, nSize);	// copy the input data buffer into the constant buffer variable's data buffer
}

// Utility function to set Lights in the given shader's constant buffer
void DXShaderBase::setLights(const std::vector<Light> &lights, const int nMaxLights, const std::string &strLightsIndex, const std::string &strNumLightsIndex, ShaderConstantBuffer &constantBuffer)
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
void DXShaderBase::setStructuredLights(const std::vector<SLight> &lights, const UINT nLights, ShaderStructuredBuffer<SLight> &lightBuffer)
{
	setStructuredBuffer<SLight>(lights, nLights, lightBuffer);
}

// Utility function to set matrices in the given shader's structured buffer
void DXShaderBase::setStructuredMatrices(const std::vector<DXMatrix> &matrices, const UINT nMatrices, ShaderStructuredBuffer<DXMatrix> &matrixBuffer, const bool bTranspose /*= true*/)
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

// TODO: Remove when done testing.
#if 0
// Utility function to copy a matrix into a mapped subresource.
void DXShaderBase::copyMatrixToMappedSubresource(const ShaderConstantBuffer &buf, const std::string &varName, const DXMatrix &matrix, D3D11_MAPPED_SUBRESOURCE &dst)
{
	copyDataToMappedSubresource<DXMatrix>(
		buf,
		varName,
		&(matrix.Transpose()),
		dst);
}
#endif // 0

// Ctor.
/*explicit*/ DXVertexShader::DXVertexShader(const wpath &shaderPath, const DevicePtr &pDevice)
	: DXShaderBase(shaderPath, pDevice)
{
	// Create the vertex shader.
	HR(m_pDevice->CreateVertexShader(m_pShaderByteCode->GetBufferPointer(), m_pShaderByteCode->GetBufferSize(), 0, &m_pShader.p));

	// Create the input layout.
	createInputLayout();
}

// Dtor.
/*virtual*/ DXVertexShader::~DXVertexShader()	
{}

// Utility function to construct an input layout using reflection
// Source: http://takinginitiative.wordpress.com/2011/12/11/directx-1011-basic-shader-reflection-automatic-input-layout-creation/
void DXVertexShader::createInputLayout()
{
	// Reflect shader info
	ShaderReflectionPtr pReflector;
	HR(D3DReflect(m_pShaderByteCode->GetBufferPointer(),						// pointer to shader bytecode buffer
		m_pShaderByteCode->GetBufferSize(),							// size in bytes of shader bytecode buffer
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
		m_pShaderByteCode->GetBufferPointer(),	// pointer to shader bytecode buffer
		m_pShaderByteCode->GetBufferSize(),		// size of shader bytecode buffer in bytes
		&m_pInputLayout.p));				// input vertex layout interface

	//Free allocation shader reflection memory
	pReflector.Release();
}

// Binds the shader to the pipeline.
void DXVertexShader::bindShader()
{
	m_pDeviceContext->VSSetShader(m_pShader.p, nullptr, 0);
}

// Binds constant buffers to the pipeline.
void DXVertexShader::bindContantBuffers(
	UINT bindSlot,
	UINT nBuffers,
	BufferRawPtr const *ppBuffers)
{
	m_pDeviceContext->VSSetConstantBuffers(bindSlot, nBuffers, ppBuffers);
}

// Binds shader resources to the pipelines.
void DXVertexShader::bindResources(
	UINT bindSlot,
	UINT nResources,
	ShaderResourceViewRawPtr const *ppResources)
{
	m_pDeviceContext->VSSetShaderResources(bindSlot, nResources, ppResources);
}

// Binds samplers to the pipelines.
void DXVertexShader::bindSamplers(
	UINT bindSlot,
	UINT nSamplers,
	SamplerStateRawPtr const *ppSamplers)
{
	m_pDeviceContext->VSSetSamplers(bindSlot, nSamplers, ppSamplers);
}

// Ctor.
/*explicit*/ DXPixelShader::DXPixelShader(const wpath &shaderPath, const DevicePtr &pDevice)
	: DXShaderBase(shaderPath, pDevice)
{
	// Create the pixel shader.
	HR(m_pDevice->CreatePixelShader(m_pShaderByteCode->GetBufferPointer(), m_pShaderByteCode->GetBufferSize(), 0, &m_pShader.p));
}

// Dtor.
/*virtual*/ DXPixelShader::~DXPixelShader()
{}

// Binds the shader to the pipeline.
void DXPixelShader::bindShader()
{
	m_pDeviceContext->PSSetShader(m_pShader.p, nullptr, 0);
}

// Binds constant buffers to the pipeline.
void DXPixelShader::bindContantBuffers(
	UINT bindSlot,
	UINT nBuffers,
	BufferRawPtr const *ppBuffers)
{
	m_pDeviceContext->PSSetConstantBuffers(bindSlot, nBuffers, ppBuffers);
}

// Binds shader resources to the pipelines.
void DXPixelShader::bindResources(
	UINT bindSlot,
	UINT nResources,
	ShaderResourceViewRawPtr const *ppResources)
{
	m_pDeviceContext->PSSetShaderResources(bindSlot, nResources, ppResources);
}

// Binds samplers to the pipelines.
void DXPixelShader::bindSamplers(
	UINT bindSlot,
	UINT nSamplers,
	SamplerStateRawPtr const *ppSamplers)
{
	m_pDeviceContext->PSSetSamplers(bindSlot, nSamplers, ppSamplers);
}

/**********************************************************************
* End of shader base classes
**********************************************************************/

#if 0
/**********************************************************************
* Start of shader implementations
**********************************************************************/

// Param ctor.
/*explicit*/ BasicShadingVS::BasicShadingVS(const wpath &shaderRoot, const DevicePtr &pDevice)
	: DXVertexShader(constructShaderPath(shaderRoot, L"basicShadingVS.cso"), pDevice)
{
	// Create the cbPerObject constant buffer.
	createConstantBuffer(m_pShaderByteCode, "cbPerObject", m_cbPerObject);
}

// Binds the shader as well as all its associated constant buffers, resources and samplers to the pipeline.
void BasicShadingVS::bind(
	const DXMatrix *pWorld,
	const DXMatrix *pWorldInvTrans,
	const DXMatrix *pWvp,
	const DXMatrix *pTexMtx)
{
	// Ensure that all the constants have been passed in.
	assert(
		pWorld &&
		pWorldInvTrans &&
		pWvp &&
		pTexMtx);

	// Bind the shader to the pipeline.
	m_pDeviceContext->VSSetShader(m_pShader.p, nullptr, 0);

	// Map the cbPerObject to system memory.
	m_cbPerObject.map(m_pDeviceContext);

	// Copy the matrices into the mapped subresource.
	m_cbPerObject.setMatrix("gWorld", *pWorld);
	m_cbPerObject.setMatrix("gWorldInvTrans", *pWorldInvTrans);
	m_cbPerObject.setMatrix("gWVP", *pWvp);
	m_cbPerObject.setMatrix("gTexMtx", *pTexMtx);

	// Unmap cbPerObject.
	m_cbPerObject.unmap(m_pDeviceContext);

	// Bind cbPerObject to the pipeline.
	m_pDeviceContext->VSSetConstantBuffers(
		m_cbPerObject.bindPoint(),
		1,
		&m_cbPerObject.pBuffer.p);
}

/**********************************************************************
* End of shader implementations
**********************************************************************/
#endif // 0
