// source file of the MirrorApp demo

#include "MirrorApp.h"

#include "ShaderCommon.h"

// Parameterized ctor, initializes the MirrorApp demo
MirrorApp::MirrorApp(HINSTANCE hInstance, const std::string strSceneFilePath) 
: DXApp(hInstance, strSceneFilePath), 
m_eCurrScene(SCENE_DEFAULT) 
//m_eCurrScene(SCENE_EXP1), m_bEnableWallDepth(false)
//m_eCurrScene(SCENE_EXP2)
//m_eCurrScene(SCENE_EXP3)
//m_eCurrScene(SCENE_EXP4)
{}

// Dtor
MirrorApp::~MirrorApp()
{}

// Define resources such as textures, blend/rasterizer/depth-stencil states and so on
void MirrorApp::createResources()
{
	// load the required textures
	const wpath textureRoot = L"N:\\DirectXLearning\\textures\\";	// @TODO: needs to be loaded from a root config file
	
	const wpath crateTex = textureRoot / wpath(L"WoodCrate02.dds");
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice, crateTex.file_string().c_str(), 0, &m_pCrateRV.p));

	const wpath floorTex = textureRoot / wpath(L"checkboard.dds");
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice, floorTex.file_string().c_str(), 0, &m_pFloorRV.p));

	const wpath mirrorTex = textureRoot / wpath(L"ice.dds");
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice, mirrorTex.file_string().c_str(), 0, &m_pMirrorRV.p));

	const wpath specTex = textureRoot / wpath(L"defaultspec.dds");
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice, specTex.file_string().c_str(), 0, &m_pSpecRV.p));

	const wpath wallTex = textureRoot / wpath(L"brick01.dds");
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice, wallTex.file_string().c_str(), 0, &m_pWallRV.p));

	// init parallel light
	m_parallelLight.dir = DXVector3(0.57735f, -0.57735f, 0.57735f);
	m_parallelLight.ambient = DXColor(0.4f, 0.4f, 0.4f, 1.0f);
	m_parallelLight.diffuse = WHITE; // DXColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_parallelLight.specular = WHITE; // DXColor(1.0f, 1.0f, 1.0f, 1.0f);

	// create the resources based on the current scene
	switch (m_eCurrScene)
	{
	case SCENE_DEFAULT:
		createResourcesDefaultScene();
		break;

	case SCENE_EXP1:
		createResourcesExpOne();
		break;

	case SCENE_EXP2:
		createResourcesDefaultScene(false);	// disable front facing CCW in the reflected crate rasterizer state
		break;

	case SCENE_EXP3:
		createResourcesDefaultScene(true, false);	// disable the stencil test for the reflected crate
		break;

	case SCENE_EXP4:
		createResourcesExpFour();
		break;
	}
}

// Define scene objects which need to be rendered
void MirrorApp::createObjects()
{
	// create the room
	const float roomScale = 1.0f;
	m_pRoom.reset(new RoomV1());
	m_pRoom->init(D3DApp::md3dDevice, roomScale);

	// create the wooden crate
	const float crateScale = 1.0f;
	m_pCrate.reset(new TextureBox());
	m_pCrate->init(D3DApp::md3dDevice, crateScale);

	// the vertex buffer consists of the four points [(-1,-1,0), (-1,+1,0), (+1,+1,0), (+1,-1,0)]
	// and the given color
	const DXColor color(BLACK);
	std::vector<SimpleVertex> vertexBuffer(4);
	vertexBuffer[0] = SimpleVertex(DXVector3(-1, -1, 0), color);
	vertexBuffer[1] = SimpleVertex(DXVector3(-1, 1, 0), color);
	vertexBuffer[2] = SimpleVertex(DXVector3(1, 1, 0), color);
	vertexBuffer[3] = SimpleVertex(DXVector3(1, -1, 0), color);

	// init the depth complexity quad with the set points
	m_pDepthQuad.reset(new SimpleQuad(vertexBuffer));
	m_pDepthQuad->init(md3dDevice, 1.0f);
}

// Meat of the drawScene method, this is where the scene rendering is implemented
void MirrorApp::drawObjects()
{
	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDeviceContext->OMSetDepthStencilState(0, 0);
	float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	md3dDeviceContext->OMSetBlendState(0, blendFactors, 0xffffffff);
	md3dDeviceContext->IASetInputLayout(m_pVertexLayout.p);
	md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// switch based on the currently selected scene
	switch (m_eCurrScene)
	{
	case SCENE_DEFAULT:
		drawObjectsDefaultScene();
		break;

	case SCENE_EXP1:
		drawObjectsExpOne();
		break;

	case SCENE_EXP2:
	case SCENE_EXP3:
		drawObjectsDefaultScene();
		break;

	case SCENE_EXP4:
		drawObjectsExpFour();
		break;
	}
}

// Define to build the shaders which will be used by the application
void MirrorApp::buildShaders()
{
	// define the vertex and pixel shader paths (@TODO: these should be specified in the scene description file)
	const wpath shaderRoot(L"N:\\DirectXLearning\\shaders\\compiled");

	// init the TexEffect
	m_pTexEffect.reset(new TexEffect(shaderRoot, md3dDevice, md3dDeviceContext));
	m_pTexEffect->init();

	// define the vertex and pixel shader paths of the color shader and load them into memory	
	m_pColorEffect.reset(new ColorEffect(shaderRoot, md3dDevice, md3dDeviceContext));
	m_pColorEffect->init();

	// @TODO: figure this out once we have a better idea of how to init shader constant buffers dynamically
#if 0
	std::vector<ShaderConstantBuffer1> vsConstBufs;
	DXApp::getShaderConstantBuffers(m_pTexVSBuf.p, vsConstBufs);
	//m_pTexVSConstBufs.resize(vsConstBufs.size());
	//for (std::size_t i = 0; i < m_pTexVSConstBufs.size(); ++i)
	//{
	//	// init the current shader constant buffer
	//	D3D11_BUFFER_DESC bufDesc;
	//	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//	//bufDesc.ByteWidth = 

	//	// @TODO: add implementation here
	//}
#endif	// 0
}

// Define to build the vertex layout which will be bound to the input assembly stage
void MirrorApp::buildVertexLayouts()
{
	// set the vertex buffer layout from TexEffect
	m_pVertexLayout = m_pTexEffect->getInputLayout();

	// set the color vertex layout from ColorEffect
	m_pColorVertexLayout = m_pColorEffect->getInputLayout();

}

// @TODO: remove when done testing
#if 0

// Define to set the constant buffer in the vertex shader during the scene render
// @TODO: modify the impl to use the actual buffer size and offsets of each variable in the buffer
void MirrorApp::setVSConstBuf(const DXMatrix &texMtx, const DXMatrix &worldMtx, const DXMatrix &wvpMtx)
{
	// set the contents of the vertex shader constant buffer
	D3D11_MAPPED_SUBRESOURCE constBuf;
	HR(md3dDeviceContext->Map(m_pTexVSConstBuf.p, 0, D3D11_MAP_WRITE_DISCARD, 0, &constBuf));	// map the constant buffer to constBuf

	// @TODO: remove when done testing
#if 0
	TexVSBuffer *pTexVSBuffer = reinterpret_cast<TexVSBuffer*>(constBuf.pData);	// reinterpret constBuf's buffer pointer as a TexVSBuffer object so that it can be populated

	// the matrices need to be transposed before they can be stored in the constant buffer
	pTexVSBuffer->gTexMtx = texMtx.Transpose();
	pTexVSBuffer->gWorld = worldMtx.Transpose();
	pTexVSBuffer->gWVP = wvpMtx.Transpose();
#endif 0

	// set each of the variables in constant buffer as per the info in the constant buffer variable info list
	for (std::size_t i = 0; i < m_TexVSConstBufInfoList.size(); ++i)
	{
		const std::ptrdiff_t nOffset = static_cast<std::ptrdiff_t>(m_TexVSConstBufInfoList[i].second.StartOffset);
		const SIZE_T nSize = static_cast<SIZE_T>(m_TexVSConstBufInfoList[i].second.Size);
		BYTE* pDst = reinterpret_cast<BYTE*>(constBuf.pData) + nOffset;
		DXMatrix dstMtx;
		if (m_TexVSConstBufInfoList[i].first == "gWorld")
		{
			dstMtx = worldMtx.Transpose();
		}
		else if (m_TexVSConstBufInfoList[i].first == "gWVP")
		{
			dstMtx = wvpMtx.Transpose();
		}
		else if (m_TexVSConstBufInfoList[i].first == "gTexMtx")
		{
			dstMtx = texMtx.Transpose();
		}

		_ASSERTE(nSize == sizeof(dstMtx));
		CONST VOID* pSrc = reinterpret_cast<CONST VOID*>(&dstMtx);
		CopyMemory(reinterpret_cast<PVOID>(pDst), pSrc, nSize);
	}

	md3dDeviceContext->Unmap(m_pTexVSConstBuf.p, 0);	// unmap the constant buffer
	//pTexVSBuffer = NULL;	// we're done with this pointer (TODO: remove when done testing)
	md3dDeviceContext->VSSetConstantBuffers(m_TexVSConstBufBindDesc.BindPoint, 1, &m_pTexVSConstBuf.p);	// set the constant buffer in the vertex shader
}

// Define to set the constant buffer in the pixel shader during the scene render
// @TODO: modify the impl to use the actual buffer size and offsets of each variable in the buffer
void MirrorApp::setPSConstBuf(const Light &light, const DXVector3 &eyePosW, const float gCurrTime)
{
	// set the contents of the pixel shader constant buffer
	D3D11_MAPPED_SUBRESOURCE constBuf;
	HR(md3dDeviceContext->Map(m_pTexPSConstBuf.p, 0, D3D11_MAP_WRITE_DISCARD, 0, &constBuf));	// map the constant buffer to constBuf

	// @TODO: remove when done testing
#if 0
	TexPSBuffer *pTexPSBuffer = reinterpret_cast<TexPSBuffer*>(constBuf.pData);	// reinterpret constBuf's buffer pointer as a TexPSBuffer object so that it can be populated
	pTexPSBuffer->gLight = light;
	pTexPSBuffer->gEyePosW = eyePosW;
	pTexPSBuffer->gCurrTime = gCurrTime;
#endif	// 0

	// set each of the variables in constant buffer as per the info in the constant buffer variable info list
	for (std::size_t i = 0; i < m_TexPSConstBufInfoList.size(); ++i)
	{
		const std::ptrdiff_t nOffset = static_cast<std::ptrdiff_t>(m_TexPSConstBufInfoList[i].second.StartOffset);
		const SIZE_T nSize = static_cast<SIZE_T>(m_TexPSConstBufInfoList[i].second.Size);
		BYTE* pDst = reinterpret_cast<BYTE*>(constBuf.pData) + nOffset;
		//DXMatrix dstMtx;
		if (m_TexPSConstBufInfoList[i].first == "gLight")
		{
			_ASSERTE(nSize == sizeof(light));
			CONST VOID* pSrc = reinterpret_cast<CONST VOID*>(&light);
			CopyMemory(reinterpret_cast<PVOID>(pDst), pSrc, nSize);
		}
		else if (m_TexPSConstBufInfoList[i].first == "gEyePosW")
		{
			_ASSERTE(nSize == sizeof(eyePosW));
			CONST VOID* pSrc = reinterpret_cast<CONST VOID*>(&eyePosW);
			CopyMemory(reinterpret_cast<PVOID>(pDst), pSrc, nSize);
		}
		else if (m_TexPSConstBufInfoList[i].first == "gCurrTime")
		{
			_ASSERTE(nSize == sizeof(gCurrTime));
			CONST VOID* pSrc = reinterpret_cast<CONST VOID*>(&gCurrTime);
			CopyMemory(reinterpret_cast<PVOID>(pDst), pSrc, nSize);
		}
	}
	
	md3dDeviceContext->Unmap(m_pTexPSConstBuf.p, 0);	// unmap the constant buffer
	//pTexPSBuffer = NULL;	// we're done with this pointer	(TODO: remove when done testing
	md3dDeviceContext->PSSetConstantBuffers(m_TexPSConstBufBindDesc.BindPoint, 1, &m_pTexPSConstBuf.p);	// set the constant buffer in the pixel shader
}

// Updates the vertex shader's constant buffer and its resources
void MirrorApp::updateVertexShader(const DXMatrix &texMtx, const DXMatrix &worldMtx, const DXMatrix &wvpMtx)
{
	// set the contents of the vertex shader constant buffer
	D3D11_MAPPED_SUBRESOURCE constBuf;
	HR(md3dDeviceContext->Map(m_pTexVSConstBuf.p, 0, D3D11_MAP_WRITE_DISCARD, 0, &constBuf));	// map the constant buffer to constBuf
	TexVSBuffer *pTexVSBuffer = reinterpret_cast<TexVSBuffer*>(constBuf.pData);	// reinterpret constBuf's buffer pointer as a TexVSBuffer object so that it can be populated

	// the matrices need to be transposed before they can be stored in the constant buffer
	pTexVSBuffer->gTexMtx = texMtx.Transpose();
	pTexVSBuffer->gWorld = worldMtx.Transpose();
	pTexVSBuffer->gWVP = wvpMtx.Transpose();

	md3dDeviceContext->Unmap(m_pTexVSConstBuf.p, 0);	// unmap the constant buffer
	pTexVSBuffer = NULL;	// we're done with this pointer
	md3dDeviceContext->VSSetConstantBuffers(m_TexVSConstBufBindDesc.BindPoint, 1, &m_pTexVSConstBuf.p);	// set the constant buffer in the vertex shader
}

// Updates the pixel shader's constant buffer and its resources
void MirrorApp::updatePixelShader(const Light &light, const DXVector3 &eyePosW, const float gCurrTime,
	const UINT diffuseMapBindPoint, const ShaderResourceViewPtr &pDiffuseMapRV,
	const UINT specMapBindPoint, const ShaderResourceViewPtr &pSpecMapRV,
	const UINT samplerStateBindPoint, const SamplerStatePtr &pSamplerState)
{
	// set the contents of the pixel shader constant buffer
	D3D11_MAPPED_SUBRESOURCE constBuf;
	HR(md3dDeviceContext->Map(m_pTexPSConstBuf.p, 0, D3D11_MAP_WRITE_DISCARD, 0, &constBuf));	// map the constant buffer to constBuf
	TexPSBuffer *pTexPSBuffer = reinterpret_cast<TexPSBuffer*>(constBuf.pData);	// reinterpret constBuf's buffer pointer as a TexPSBuffer object so that it can be populated
	pTexPSBuffer->gLight = light;
	pTexPSBuffer->gEyePosW = eyePosW;
	pTexPSBuffer->gCurrTime = gCurrTime;
	md3dDeviceContext->Unmap(m_pTexPSConstBuf.p, 0);	// unmap the constant buffer
	pTexPSBuffer = NULL;	// we're done with this pointer
	md3dDeviceContext->PSSetConstantBuffers(m_TexPSConstBufBindDesc.BindPoint, 1, &m_pTexPSConstBuf.p);	// set the constant buffer in the pixel shader

	// set the gDiffuse texture in the pixel shader
	md3dDeviceContext->PSSetShaderResources(diffuseMapBindPoint, 1, &pDiffuseMapRV.p);

	// set the gSpecMap texture in the pixel shader
	md3dDeviceContext->PSSetShaderResources(specMapBindPoint, 1, &pSpecMapRV.p);

	// set the sampler state in the pixel shader
	md3dDeviceContext->PSSetSamplers(samplerStateBindPoint, 1, &pSamplerState.p);
}

// Define to set the constant buffer in the color vertex shader during the scene render
void MirrorApp::setColorVSConstBuf(const DXMatrix &wvpMtx)
{
	// set the contents of the vertex shader constant buffer
	D3D11_MAPPED_SUBRESOURCE constBuf;
	HR(md3dDeviceContext->Map(m_pColorVSConstBuf.p, 0, D3D11_MAP_WRITE_DISCARD, 0, &constBuf));	// map the constant buffer to constBuf

	// @TODO: remove when done testing
#if 0
	TexVSBuffer *pTexVSBuffer = reinterpret_cast<TexVSBuffer*>(constBuf.pData);	// reinterpret constBuf's buffer pointer as a TexVSBuffer object so that it can be populated

	// the matrices need to be transposed before they can be stored in the constant buffer
	pTexVSBuffer->gTexMtx = texMtx.Transpose();
	pTexVSBuffer->gWorld = worldMtx.Transpose();
	pTexVSBuffer->gWVP = wvpMtx.Transpose();
#endif 0

	// set each of the variables in constant buffer as per the info in the constant buffer variable info list
	for (std::size_t i = 0; i < m_ColorVSConstBufInfoList.size(); ++i)
	{
		const std::ptrdiff_t nOffset = static_cast<std::ptrdiff_t>(m_ColorVSConstBufInfoList[i].second.StartOffset);
		const SIZE_T nSize = static_cast<SIZE_T>(m_ColorVSConstBufInfoList[i].second.Size);
		BYTE* pDst = reinterpret_cast<BYTE*>(constBuf.pData) + nOffset;
		DXMatrix dstMtx;
		if (m_ColorVSConstBufInfoList[i].first == "gWVP")
		{
			dstMtx = wvpMtx.Transpose();
		}

		_ASSERTE(nSize == sizeof(dstMtx));
		CONST VOID* pSrc = reinterpret_cast<CONST VOID*>(&dstMtx);
		CopyMemory(reinterpret_cast<PVOID>(pDst), pSrc, nSize);
	}

	md3dDeviceContext->Unmap(m_pColorVSConstBuf.p, 0);	// unmap the constant buffer
	//pTexVSBuffer = NULL;	// we're done with this pointer (TODO: remove when done testing)
	md3dDeviceContext->VSSetConstantBuffers(m_ColorVSConstBufBindDesc.BindPoint, 1, &m_pColorVSConstBuf.p);	// set the constant buffer in the vertex shader
}

#endif	// 0

// default scene resource creation
void MirrorApp::createResourcesDefaultScene(const bool bCCWFront, const bool bRefCrateStencil)
{
	// define the mirror's depth-stencil state
	// refer the following for default values: http://msdn.microsoft.com/en-us/library/windows/desktop/ff476110%28v=vs.85%29.aspx
	D3D11_DEPTH_STENCIL_DESC mirrorDSSDesc;
	ZeroMemory(&mirrorDSSDesc, sizeof(mirrorDSSDesc));			// clear the memory
	mirrorDSSDesc.DepthEnable = TRUE;							// enable the depth test
	mirrorDSSDesc.DepthFunc = D3D11_COMPARISON_LESS;			// standard comparison test, fragments with a lower depth value get written to the depth buffer
	mirrorDSSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// all bits enabled

	mirrorDSSDesc.StencilEnable = TRUE;								// enable the stencil test
	mirrorDSSDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;	// 0xff
	mirrorDSSDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;	// 0xff

	// stencil test params for front facing polygons
	mirrorDSSDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	// the stencil test always passes
	mirrorDSSDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;	// replace the current value on the stencil buffer with stencilRef if the stencil test passes
	// this will always be the case since we just set the stencil test to always pass
	mirrorDSSDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;	// keep the current value on the stencil buffer if the stencil test fails
	// will not ever happen since we just set the stencil test to always pass
	mirrorDSSDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;		// same as the above case

	// stencil test params for back facing polygons (irrelevant as we're not rendering back facing polygons, keep same as for front face)
	mirrorDSSDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	mirrorDSSDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	mirrorDSSDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDSSDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	// create the depth-stencil state
	HR(md3dDevice->CreateDepthStencilState(&mirrorDSSDesc, &m_pDrawMirrorDSS.p));

	// define the mirror's blend state
	D3D11_BLEND_DESC mirrorBlendDesc;
	ZeroMemory(&mirrorBlendDesc, sizeof(mirrorBlendDesc));	// clear the memory
	mirrorBlendDesc.AlphaToCoverageEnable = FALSE;	// default value
	mirrorBlendDesc.IndependentBlendEnable = FALSE;	// default value

	// blend settings for the 1st render target (which is what we're dealing with)
	mirrorBlendDesc.RenderTarget[0].BlendEnable = TRUE;	// enable blending
	mirrorBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;	// add the source and dest contributions
	mirrorBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;	// use the user defined blend factor for the source
	mirrorBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;	// use the inverse of the user defined blend factor for the destination
	mirrorBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;	// add the source and dest alpha contributions
	mirrorBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;	// source alpha factor = 1
	mirrorBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;	// destination alpha factor = 0
	mirrorBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// create the mirror's blend state
	HR(md3dDevice->CreateBlendState(&mirrorBlendDesc, &m_pReflectBS.p));

	// define the mirror reflection's depth-stencil state
	// the depth and stencil tests are setup such that the reflected crate is visible through the mirror
	D3D11_DEPTH_STENCIL_DESC reflectDSSDesc;
	ZeroMemory(&reflectDSSDesc, sizeof(reflectDSSDesc));	// clear the memory

	// the depth test will always pass and no writes will be made to the depth buffer
	// as the crate needs to be visible through the mirror even though it is actually
	// behind it in the scene
	reflectDSSDesc.DepthEnable = TRUE;	// enable the depth test
	reflectDSSDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;	// set the depth test to always pass
	reflectDSSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;	// write mask = 0

	// the stencil test will pass iff the stencilRef value == the current value on the stencil buffer
	reflectDSSDesc.StencilEnable = bRefCrateStencil ? TRUE : FALSE;	// enable the stencil test
	reflectDSSDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;	// default value (1)
	reflectDSSDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;	// default value (1)

	// front face settings, same as for the regular mirror depth-stencil state except in the case of the stencil test passing,
	// wherein the current stencil buffer value is retained if it is equal to the stencilRef value
	reflectDSSDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	reflectDSSDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	reflectDSSDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	reflectDSSDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	// back face settings, irrelevant again
	reflectDSSDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	reflectDSSDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	reflectDSSDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	reflectDSSDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	// create the mirror reflection's depth stencil state
	HR(md3dDevice->CreateDepthStencilState(&reflectDSSDesc, &m_pDrawReflectionDSS.p));

	// define the rasterizer state for the reflected crate
	// the rasterizer state considers those polygons with CCW winding order to be front facing
	// this is done because the normals of the reflected crate will still be facing the same way
	// as their counterparts on the original crate since the reflection transformation will not
	// affect the normals
	D3D11_RASTERIZER_DESC reflectRSDesc;
	ZeroMemory(&reflectRSDesc, sizeof(reflectRSDesc));
	reflectRSDesc.CullMode = D3D11_CULL_BACK;
	reflectRSDesc.FillMode = D3D11_FILL_SOLID;
	reflectRSDesc.FrontCounterClockwise = bCCWFront ? TRUE : FALSE;

	// create the rasterizer state for the reflected crate
	HR(md3dDevice->CreateRasterizerState(&reflectRSDesc, &m_pCullCWRS.p));
}

// default scene draw objects
void MirrorApp::drawObjectsDefaultScene()
{
	// const matrices for the frame
	const DXMatrix cratePos(DXMatrix::CreateTranslation(DXVector3(0.0f, 1.0f, -4.0f)));
	const DXMatrix roomPos(DXMatrix::Identity());
	const DXMatrix viewProj(m_pCamera->view() * m_pCamera->proj());

	// define the world matrix, texture matrix and the WVP matrix for the crate
	DXMatrix currWorld(roomPos);
	DXMatrix currTex(DXMatrix::Identity());
	DXMatrix currWVP = currWorld * viewProj;

	// set the TexEffect parameters
	m_pTexEffect->setWorld(currWorld);
	m_pTexEffect->setWVP(currWVP);
	m_pTexEffect->setTexMtx(currTex);

	m_pTexEffect->setLight(m_parallelLight);
	m_pTexEffect->setEyePosW(m_pCamera->eyePosW());
	m_pTexEffect->setCurrTime(mTimer.getGameTime());

	// set the resources for the room floor
	m_pTexEffect->setDiffuseMap(m_pFloorRV);
	m_pTexEffect->setSpecMap(m_pSpecRV);

	// apply the effect and draw the floor
	m_pTexEffect->apply();
	m_pRoom->drawFloor();

	// set the resources for the room wall
	m_pTexEffect->setDiffuseMap(m_pWallRV);

	// apply the effect and draw the wall
	m_pTexEffect->apply();
	m_pRoom->drawWall();

	// update currWorld and currWVP for the crate
	currWorld = cratePos;
	currWVP = currWorld * viewProj;

	// update the TexEffect parameters
	m_pTexEffect->setWorld(currWorld);
	m_pTexEffect->setWVP(currWVP);

	m_pTexEffect->setDiffuseMap(m_pCrateRV);

	// apply the TexEffect
	m_pTexEffect->apply();

	// draw the crate
	m_pCrate->draw();

	// setup the vertex and pixel shader to draw the mirror
	currWorld = roomPos;
	currWVP = currWorld * viewProj;

	// set the mirror's regular depth-stencil state
	const UINT stencilRef(1);
	md3dDeviceContext->OMSetDepthStencilState(m_pDrawMirrorDSS.p, stencilRef);

	// update the TexEffect parameters for the mirror
	m_pTexEffect->setWorld(currWorld);
	m_pTexEffect->setWVP(currWVP);
	m_pTexEffect->setDiffuseMap(m_pMirrorRV);

	// apply the TexEffect and draw the mirror
	m_pTexEffect->apply();
	m_pRoom->drawMirror();

	// reset the depth-stencil state
	md3dDeviceContext->OMSetDepthStencilState(0, 0);

	// set the current world transformation to reflect about the mirror
	const DXPlane reflectPlane(0.0f, 0.0f, 1.0f, 0.0f);	// xy plane
	const DXMatrix reflector(DXMatrix::CreateReflection(reflectPlane));
	currWorld = cratePos * reflector;	// first set the crate in its start position and then reflect
	currWVP = currWorld * viewProj;

	// update the TexEffect world and WVP matrices
	m_pTexEffect->setWorld(currWorld);
	m_pTexEffect->setWVP(currWVP);

	// update the parallel light's dir to also reflect
	const DXVector3 oriLightDir = m_parallelLight.dir;
	m_parallelLight.dir = DXVector3::TransformNormal(m_parallelLight.dir, reflector);

	// update the TexEffect light
	m_pTexEffect->setLight(m_parallelLight);

	// set the mirror reflection's blend state
	const FLOAT blendFactors[4] = { 0.65f, 0.65f, 0.65f, 1.0f };
	const UINT sampleMask(0xffffffff);
	md3dDeviceContext->OMSetBlendState(m_pReflectBS.p, blendFactors, sampleMask);

	// set the mirror reflection's depth-stencil state
	md3dDeviceContext->OMSetDepthStencilState(m_pDrawReflectionDSS.p, stencilRef);

	// set the reflected crate's rasterizer state
	md3dDeviceContext->RSSetState(m_pCullCWRS.p);

	// update the TexEffect diffuse map and apply the effect
	m_pTexEffect->setDiffuseMap(m_pCrateRV);
	m_pTexEffect->apply();

	// draw the reflected crate
	m_pCrate->draw();

	// reset the reflected crate's rasterizer state
	md3dDeviceContext->RSSetState(0);

	// reset the depth-stencil state
	md3dDeviceContext->OMSetDepthStencilState(0, 0);

	// reset the blend state
	md3dDeviceContext->OMSetBlendState(0, blendFactors, sampleMask);

	// reset the parallel light dir
	m_parallelLight.dir = oriLightDir;
}

// experiment 1 resource creation
void MirrorApp::createResourcesExpOne()
{
	// create a depth-stencil state with just the depth test disabled and the stencil test disabled
	D3D11_DEPTH_STENCIL_DESC wallDSSDesc;
	ZeroMemory(&wallDSSDesc, sizeof(wallDSSDesc));
	wallDSSDesc.DepthEnable = m_bEnableWallDepth ? TRUE : FALSE; // depth test disabled/enabled
	wallDSSDesc.DepthFunc = D3D11_COMPARISON_LESS;				// standard comparison func
	wallDSSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// depth write mask all bits set
	wallDSSDesc.StencilEnable = FALSE;							// stencil test disabled
	wallDSSDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;		// default value (doesn't matter)
	wallDSSDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;	// default value (doesn't matter)
	HR(md3dDevice->CreateDepthStencilState(&wallDSSDesc, &m_pDrawMirrorDSS.p));

	// create a depth stencil state with just the depth test enabled and the stencil test disabled
	D3D11_DEPTH_STENCIL_DESC crateDSSDesc;
	ZeroMemory(&crateDSSDesc, sizeof(crateDSSDesc));
	crateDSSDesc.DepthEnable = TRUE;	// enable the depth test
	crateDSSDesc.DepthFunc = D3D11_COMPARISON_LESS;	// standard comparison func
	crateDSSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// depth write mask all bits set
	crateDSSDesc.StencilEnable = FALSE;	// stencil test disabled
	crateDSSDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;	// default value (doesn't matter)
	crateDSSDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;	// default value (doesn't matter)
	HR(md3dDevice->CreateDepthStencilState(&crateDSSDesc, &m_pDrawReflectionDSS.p));

	// create the rasterizer state for the reflected crate which accounts for the reverse winding order
	D3D11_RASTERIZER_DESC crateRSDesc;
	ZeroMemory(&crateRSDesc, sizeof(crateRSDesc));
	crateRSDesc.FillMode = D3D11_FILL_SOLID;
	crateRSDesc.CullMode = D3D11_CULL_BACK;
	crateRSDesc.FrontCounterClockwise = TRUE;
	HR(md3dDevice->CreateRasterizerState(&crateRSDesc, &m_pCullCWRS.p));
}

// experiment 1 draw objects
void MirrorApp::drawObjectsExpOne()
{
	// define frame constants
	const DXPlane reflectPlane(0.0f, 0.0f, 1.0f, 0.0f);	// x-y plane
	const DXMatrix reflector(DXMatrix::CreateReflection(reflectPlane));
	const DXMatrix cratePos(DXMatrix::CreateTranslation(0.0f, 1.0f, -4.0f) * reflector);
	const DXMatrix roomPos(DXMatrix::Identity());
	const DXMatrix texMtx(DXMatrix::Identity());
	const DXMatrix viewProj(m_pCamera->view() * m_pCamera->proj());
	const UINT stencilRef(1);
	const DXVector3 oriLightDir = m_parallelLight.dir;

	// setup the vertex and pixel shaders for the wall
	DXMatrix currWorld(roomPos), currWVP(currWorld * viewProj);

	// set the TexEffect parameters
	m_pTexEffect->setWorld(currWorld);
	m_pTexEffect->setWVP(currWVP);
	m_pTexEffect->setTexMtx(texMtx);

	m_pTexEffect->setLight(m_parallelLight);
	m_pTexEffect->setEyePosW(m_pCamera->eyePosW());
	m_pTexEffect->setCurrTime(mTimer.getGameTime());

	// @TODO: remove when done testing
#if 0
	// init the vertex and pixel shaders
	md3dDeviceContext->VSSetShader(m_pTexVS.p, NULL, 0);
	md3dDeviceContext->PSSetShader(m_pTexPS.p, NULL, 0);
	md3dDeviceContext->PSSetSamplers(m_TexSamplerBindDesc.BindPoint, 1, &m_pTexPSSampler.p);

	setVSConstBuf(texMtx, currWorld, currWVP);
	setPSConstBuf(m_parallelLight, m_pCamera->eyePosW(), mTimer.getGameTime());
#endif	// 0

	// set the depth-stencil state for the wall
	md3dDeviceContext->OMSetDepthStencilState(m_pDrawMirrorDSS.p, stencilRef);

	// set the resources for the room floor
	m_pTexEffect->setDiffuseMap(m_pFloorRV);
	m_pTexEffect->setSpecMap(m_pSpecRV);

	// apply the effect and draw the floor
	m_pTexEffect->apply();
	m_pRoom->drawFloor();

	// set the resources for the room wall
	m_pTexEffect->setDiffuseMap(m_pWallRV);

	// apply the effect and draw the wall
	m_pTexEffect->apply();
	m_pRoom->drawWall();

	// draw the wall
	//m_pRoom->drawRoom(m_pWallRV, m_TexDiffuseMapBindDesc.BindPoint, m_pFloorRV, m_TexDiffuseMapBindDesc.BindPoint);
	
	// reset the depth-stencil state
	md3dDeviceContext->OMSetDepthStencilState(0, 0);

	// setup the vertex and pixel shaders for the crate
	currWorld = cratePos;
	currWVP = currWorld * viewProj;

	// update the TexEffect world and WVP matrices
	m_pTexEffect->setWorld(currWorld);
	m_pTexEffect->setWVP(currWVP);

	//setVSConstBuf(texMtx, currWorld, currWVP);	// @TODO: remove when done testing

	m_parallelLight.dir = DXVector3::TransformNormal(m_parallelLight.dir, reflector);	// reflect the light dir so that the reflected crate is illuminated
	m_pTexEffect->setLight(m_parallelLight);
	
	//setPSConstBuf(m_parallelLight, m_pCamera->eyePosW(), mTimer.getGameTime());	// @TODO: remove when done testing

	// set the depth-stencil state for the crate
	md3dDeviceContext->OMSetDepthStencilState(m_pDrawReflectionDSS.p, stencilRef);

	// set the rasterizer state for the reflected crate
	md3dDeviceContext->RSSetState(m_pCullCWRS.p);

	// update the TexEffect diffuse map and apply the effect
	m_pTexEffect->setDiffuseMap(m_pCrateRV);
	m_pTexEffect->apply();

	// draw the reflected crate
	m_pCrate->draw();
	//m_pCrate->drawBox(m_TexDiffuseMapBindDesc.BindPoint, m_pCrateRV, m_TexSpecMapBindDesc.BindPoint, m_pSpecRV);	// @TODO: remove when done testing

	// reset the rasterizer state
	md3dDeviceContext->RSSetState(0);

	// reset the depth stencil state
	md3dDeviceContext->OMSetDepthStencilState(0, 0);

	// reset the light dir
	m_parallelLight.dir = oriLightDir;
}

// experiment 4 resource creation
void MirrorApp::createResourcesExpFour()
{
	// setup a depth-stencil state which increments the stencil buffer value for every fragment rendered
	D3D11_DEPTH_STENCIL_DESC bufDesc;
	ZeroMemory(&bufDesc, sizeof(bufDesc));
	bufDesc.DepthEnable = TRUE;	// enable the depth test
	bufDesc.DepthFunc = D3D11_COMPARISON_LESS;	// default comparison function
	bufDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// all bits enabled for writing

	bufDesc.StencilEnable = TRUE;	// enable the stencil test
	bufDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	bufDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	// front face params
	bufDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	// always pass the stencil test
	bufDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;	// increment the stencil buffer value (this will always happen since the test always passes)
	bufDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; // keep the current stencil buffer value (irrelevant)
	bufDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;	// same as depth fail op

	// back face params (irrelevant as we're not rendering back faces, keep same as params for front face)
	bufDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	// always pass the stencil test
	bufDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;	// increment the stencil buffer value (this will always happen since the test always passes)
	bufDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; // keep the current stencil buffer value (irrelevant)
	bufDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;	// same as depth fail op

	// create the depth-stencil state
	HR(md3dDevice->CreateDepthStencilState(&bufDesc, &m_pDrawMirrorDSS.p));

	// setup a depth-stencil state which passes the stencil test iff the stencilRef value is equal to the current value on the stencil buffer
	// reuse the previous buffer description since there is very little difference between the two descriptions
	bufDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	bufDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	bufDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;	// again, back face params are irrelevant
	bufDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	// create the depth-stencil state
	HR(md3dDevice->CreateDepthStencilState(&bufDesc, &m_pDrawReflectionDSS.p));
}

// experiment 4 draw objects
void MirrorApp::drawObjectsExpFour()
{
	// const matrices for the frame
	const DXMatrix cratePos(DXMatrix::CreateTranslation(DXVector3(0.0f, 1.0f, -4.0f)));
	const DXMatrix roomPos(DXMatrix::Identity());
	const DXMatrix viewProj(m_pCamera->view() * m_pCamera->proj());

	// define the world matrix, texture matrix and the WVP matrix for the room
	DXMatrix currWorld(roomPos);
	DXMatrix currTex(DXMatrix::Identity());
	DXMatrix currWVP = currWorld * viewProj;

	// set the TexEffect parameters
	m_pTexEffect->setWorld(currWorld);
	m_pTexEffect->setWVP(currWVP);
	m_pTexEffect->setTexMtx(currTex);

	m_pTexEffect->setLight(m_parallelLight);
	m_pTexEffect->setEyePosW(m_pCamera->eyePosW());
	m_pTexEffect->setCurrTime(mTimer.getGameTime());

	// @TODO: remove when done testing
#if 0
	// bind the vertex and pixel shaders to the pipeline
	md3dDeviceContext->VSSetShader(m_pTexVS.p, NULL, 0);
	md3dDeviceContext->PSSetShader(m_pTexPS.p, NULL, 0);

	// set the sampler state in the pixel shader
	md3dDeviceContext->PSSetSamplers(m_TexSamplerBindDesc.BindPoint, 1, &m_pTexPSSampler.p);

	// update the vertex shader constant buffer
	setVSConstBuf(currTex, currWorld, currWVP);

	// set the contents of the pixel shader constant buffer
	setPSConstBuf(m_parallelLight, m_pCamera->eyePosW(), mTimer.getGameTime());
#endif	// 0

	// set the incrementing depth-stencil state
	UINT stencilRef(1);	// dummy value, wont be used by the depth-stencil state
	md3dDeviceContext->OMSetDepthStencilState(m_pDrawMirrorDSS.p, stencilRef);

	// set the resources for the room floor
	m_pTexEffect->setDiffuseMap(m_pFloorRV);
	m_pTexEffect->setSpecMap(m_pSpecRV);

	// apply the effect and draw the floor
	m_pTexEffect->apply();
	m_pRoom->drawFloor();

	// set the resources for the room wall
	m_pTexEffect->setDiffuseMap(m_pWallRV);

	// apply the effect and draw the wall
	m_pTexEffect->apply();
	m_pRoom->drawWall();

	// draw the room
	//m_pRoom->drawRoom(m_pWallRV, m_TexDiffuseMapBindDesc.BindPoint, m_pFloorRV, m_TexDiffuseMapBindDesc.BindPoint);

	// update the world and WVP matrix for the crate
	currWorld = cratePos;
	currWVP = currWorld * viewProj;

	// update the TexEffect world and WVP matrices
	m_pTexEffect->setWorld(currWorld);
	m_pTexEffect->setWVP(currWVP);

	// update the vertex shader constant buffer
	//setVSConstBuf(currTex, currWorld, currWVP);	// @TODO: remove when done testing

	// no need to update the pixel shader's constant buffer at this point

	// update the TexEffect diffuse map and apply the effect
	m_pTexEffect->setDiffuseMap(m_pCrateRV);
	m_pTexEffect->apply();

	// draw the crate
	m_pCrate->draw();
	//m_pCrate->drawBox(m_TexDiffuseMapBindDesc.BindPoint, m_pCrateRV, m_TexSpecMapBindDesc.BindPoint, m_pSpecRV);	// @TODO: remove when done testing

	// reset the depth-stencil state
	md3dDeviceContext->OMSetDepthStencilState(0, 0);

	// set the input layout for the color shader
	md3dDeviceContext->IASetInputLayout(m_pColorVertexLayout.p);

	// update the WVP matrix for the color shader
	// set to identity since the quad points are specified in homogenous clip space
	currWVP = DXMatrix::Identity();
	m_pColorEffect->setWVP(currWVP);
	m_pColorEffect->apply();

	// draw quads for all depth-complexities ranging from 1 to 10
	// each quad will be colored i/10
	for (UINT i = 1; i <= 10; ++i)
	{
		// set the depth complexity counting depth-stencil state
		// passing stencilRef as 'i' will ensure that the i'th quad will correspond to the current depth complexity
		md3dDeviceContext->OMSetDepthStencilState(m_pDrawReflectionDSS.p, i);

		// draw a quad corresponding to current depth complexity
		const float colorFactor(i / 10.0f);
		const DXColor color(colorFactor, colorFactor, colorFactor, 1.0f);
		drawFullScreenQuad(color);
	}

	// reset the depth-stencil state
	md3dDeviceContext->OMSetDepthStencilState(0, 0);
}

// helper function to draw a full screen quad of the given colour
void MirrorApp::drawFullScreenQuad(const DXColor color)
{
	// update the depth quad's color and draw it
	m_pDepthQuad->updateColor(color);
	m_pDepthQuad->draw();
}