// Source file of the SemiReflectiveWindowApp class.

#include "SemiReflectiveWindowApp.h"

#include "SamplerStateMaker.h"

// param ctor
// TODO: Initialize additional members as necessary
SemiReflectiveWindowApp::SemiReflectiveWindowApp(HINSTANCE hInstance, const std::string strSceneFilePath) : 
	DXApp(hInstance, strSceneFilePath), 
	m_pRoom(new RoomV1()),
	m_pBox(new Box()),
	m_bModelsLoaded(false)
{
	//::CoInitialize(nullptr);	// Need to call this to initialize WICTextureLoader in order to load textures.
}

// dtor
SemiReflectiveWindowApp::~SemiReflectiveWindowApp()	
{
	// Join with the models loader thread.
	m_modelsLoader.join();	
}

// initializes the DirectX application's objects and resources
void SemiReflectiveWindowApp::initApp()
{
	DXApp::initApp();

	// NOTE: This is what goes on in DXApp::initApp(). Left here as a quick reference.
#if 0
	mClearColor = BLACK; // DXColor(0.0f, 0.0f, 0.0f, 1.0f);	// set clear color to black

	// init the SceneBuilder object
	if (sceneFileAvailable())
	{
		m_pSceneBuilder.reset(new SceneBuilder(m_strSceneFilePath));
	}
	m_strSceneName = m_pSceneBuilder->getSceneName();

	// init the camera
	initCamera();

	// build shader effects, vertex layouts and initialize all resources
	buildShaders();
	buildVertexLayouts();
	createResources();

	// create and initialize scene objects
	createObjects();
#endif // 0

	// TODO: Add implementation here.

}

// called when the client window is resized
void SemiReflectiveWindowApp::onResize()
{
	DXApp::onResize();

	// TODO: Add implementation here.	
}

// called when Windows events have to be processed and there is something that the application might have to take care off
void SemiReflectiveWindowApp::updateScene(float dt)
{
	DXApp::updateScene(dt);

	// TODO: Add implementation here.
}

// renders the scene
void SemiReflectiveWindowApp::drawScene()
{
	DXApp::drawScene();

	// TODO: Add implementation here.
}

// Define resources such as textures, blend/rasterizer/depth-stencil states and so on
void SemiReflectiveWindowApp::createResources()
{
	// load the required textures
	//const wpath textureRoot = L"N:\\DirectXLearning\\textures\\";	// @TODO: needs to be loaded from a root config file
	const wpath textureRoot(m_pSceneBuilder->getTextureRootW());

	const wpath crateTex = textureRoot / wpath(L"WoodCrate02.dds");
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice.p, crateTex.file_string().c_str(), 0, &m_pCrateRV.p));

	const wpath floorTex = textureRoot / wpath(L"checkboard.dds");
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice.p, floorTex.file_string().c_str(), 0, &m_pFloorRV.p));

	const wpath mirrorTex = textureRoot / wpath(L"ice.dds");
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice.p, mirrorTex.file_string().c_str(), 0, &m_pMirrorRV.p));

	const wpath specTex = textureRoot / wpath(L"defaultspec.dds");
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice.p, specTex.file_string().c_str(), 0, &m_pSpecRV.p));

	const wpath wallTex = textureRoot / wpath(L"brick01.dds");
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice.p, wallTex.file_string().c_str(), 0, &m_pWallRV.p));

	// init parallel lights
	ParallelLightParams parallelDefaultParams;
	m_pSceneBuilder->buildParallelLights<SLight>(
		parallelDefaultParams,
		m_parallelLights);

	// Create the structured buffer for the parallel lights.
	const bool bDynamic(true);
	m_sbParallelLights.reset(
		new ShaderStructuredBuffer<SLight>(
		"gParallelLights",
		md3dDevice,
		md3dDeviceContext,
		m_ppsBasic->byteCode(),
		m_parallelLights.size(),
		D3D11_BIND_SHADER_RESOURCE,
		bDynamic,
		m_parallelLights.data()));
	
	// Create a trilinear sampler state.
	SamplerStateMaker samplerMaker;
	samplerMaker.makeTriLinear(md3dDevice, m_pSampler);

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
	HR(md3dDevice->CreateDepthStencilState(&mirrorDSSDesc, &m_pdssMirror.p));

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
	HR(md3dDevice->CreateBlendState(&mirrorBlendDesc, &m_pbsMirror.p));

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
	reflectDSSDesc.StencilEnable = TRUE;	// enable the stencil test
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
	HR(md3dDevice->CreateDepthStencilState(&reflectDSSDesc, &m_pdssReflectedCrate.p));

	// define the rasterizer state for the reflected crate
	// the rasterizer state considers those polygons with CCW winding order to be front facing
	// this is done because the normals of the reflected crate will still be facing the same way
	// as their counterparts on the original crate since the reflection transformation will not
	// affect the normals
	D3D11_RASTERIZER_DESC reflectRSDesc;
	ZeroMemory(&reflectRSDesc, sizeof(reflectRSDesc));
	reflectRSDesc.CullMode = D3D11_CULL_BACK;
	reflectRSDesc.FillMode = D3D11_FILL_SOLID;
	reflectRSDesc.FrontCounterClockwise = TRUE;

	// create the rasterizer state for the reflected crate
	HR(md3dDevice->CreateRasterizerState(&reflectRSDesc, &m_prsReflectedCrate.p));

	// TODO: Add implementation here.
}

// Define scene objects which need to be rendered.
void SemiReflectiveWindowApp::createObjects()
{
	// Launch the model loading thread.
	m_modelsLoader = std::thread(&SemiReflectiveWindowApp::loadModels, this);
	//loadModels();
}

// Meat of the drawScene method, this is where the scene rendering is implemented.
void SemiReflectiveWindowApp::drawObjects()
{
	if(m_bModelsLoaded)
	{
		// Set the input layout and primitive topology.
		md3dDeviceContext->IASetInputLayout(m_pVertexLayout.p);
		md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Set the default depth stencil state.
		md3dDeviceContext->OMSetDepthStencilState(0, 0);

		// Setup to draw the wall.

		// Set the vertex shader.
		m_pvsBasic->bindShader();

		// Set the vertex shader constants.
		const DXMatrix viewProj(m_pCamera->view() * m_pCamera->proj());
		DXMatrix world(DXMatrix::Identity()), wvp(world * viewProj), tex(DXMatrix::Identity());
		m_pcbPerObject->map();
		m_pcbPerObject->setMatrix("gWorld", world);
		m_pcbPerObject->setMatrix("gWorldInvTrans", world);
		m_pcbPerObject->setMatrix("gWVP", wvp);
		m_pcbPerObject->setMatrix("gTexMtx", tex);
		m_pcbPerObject->unmap();
		//const std::array<BufferRawPtr, 1> ppBuffers = { m_pcbPerObject->buffer() };
		std::array<BufferRawPtr, 1> ppBuffers = { m_pcbPerObject->buffer() };
		m_pvsBasic->bindContantBuffers(
			m_pcbPerObject->bindPoint(),
			ppBuffers.size(),
			ppBuffers.data());

		// Set the pixel shader.
		m_ppsBasic->bindShader();

		// Set the pixel shader constants.
		const DXVector3 eyePosW(m_pCamera->eyePosW());
		m_pcbPerFrame->map();
		m_pcbPerFrame->setDatum<std::size_t>("nParallelLights", m_parallelLights.size());
		m_pcbPerFrame->setDatum<std::size_t>("nPointLights", 0);
		m_pcbPerFrame->setDatum<std::size_t>("nSpotLights", 0);
		m_pcbPerFrame->setDatum<DXVector3>("gEyePosW", eyePosW);
		m_pcbPerFrame->unmap();

		ppBuffers[0] = m_pcbPerFrame->buffer();
		m_ppsBasic->bindContantBuffers(
			m_pcbPerFrame->bindPoint(),
			ppBuffers.size(),
			ppBuffers.data());

		// Set the pixel shader resources.
		std::array<ShaderResourceViewRawPtr, 5> ppResources = {
			m_sbParallelLights->srv(),
			nullptr,
			nullptr,
			m_pWallRV.p,
			m_pSpecRV.p };
		m_ppsBasic->bindResources(
			0,
			ppResources.size(),
			ppResources.data());

		// Set the pixel shader sampler states.
		std::array<SamplerStateRawPtr, 1> ppSamplers = { m_pSampler.p };
		m_ppsBasic->bindSamplers(
			0,
			ppSamplers.size(),
			ppSamplers.data());

		// Draw the wall.
		m_pRoom->drawWall();

		// Set the wall's texture only and re-bind the pixel shader resources.
		m_ppsBasic->bindResources(
			3,
			1,
			&m_pFloorRV.p);

		// Draw the floor.
		m_pRoom->drawFloor();

		// Set the box's vertex and index buffers.
		m_pBox->setIndexAndVertexBuffers();

		// Set the world transform for the box.
		DXMatrix crateWorld = DXMatrix::CreateTranslation(DXVector3(0, 1, -4)) * DXMatrix::CreateScale(2.0f);
		world = crateWorld;
		wvp = world * viewProj;
		m_pcbPerObject->map();
		m_pcbPerObject->setMatrix("gWorld", world);
		m_pcbPerObject->setMatrix("gWorldInvTrans", world.Invert().Transpose());
		m_pcbPerObject->setMatrix("gWVP", wvp);
		m_pcbPerObject->setMatrix("gTexMtx", tex);
		m_pcbPerObject->unmap();
		//const std::array<BufferRawPtr, 1> ppBuffers = { m_pcbPerObject->buffer() };
		//std::array<BufferRawPtr, 1> ppBuffers = { m_pcbPerObject->buffer() };
		ppBuffers[0] = m_pcbPerObject->buffer();
		m_pvsBasic->bindContantBuffers(
			m_pcbPerObject->bindPoint(),
			ppBuffers.size(),
			ppBuffers.data());

		// Set the box's texture only and re-bind the pixel shader resources.
		m_ppsBasic->bindResources(
			3,
			1,
			&m_pCrateRV.p);

		// Draw the box.
		m_pBox->draw();

		// Draw Sydney.
		std::vector<DXMatrix> transformationStack(1, m_models.at("Sydney").getTransform());
		m_models.at("Sydney").pModel->setVertexAndIndexBuffers();
		drawModel(
			m_models.at("Sydney").pModel, 
			m_models.at("Sydney").pModel->rootNode(), 
			viewProj, 
			transformationStack);

		// Set the mirror's depth stencil state.
		md3dDeviceContext->OMSetDepthStencilState(m_pdssMirror.p, 1);

		// Set the transparent blend state.
		/*const*/ FLOAT blendFactors[4] = { 0.25f, 0.25f, 0.25f, 1.0f };
		const UINT sampleMask(0xffffffff);
		md3dDeviceContext->OMSetBlendState(m_pbsMirror.p, blendFactors, sampleMask);

		// Set the mirror's transform.
		world = DXMatrix::Identity();
		wvp = world * viewProj;
		m_pcbPerObject->map();
		m_pcbPerObject->setMatrix("gWorld", world);
		m_pcbPerObject->setMatrix("gWorldInvTrans", world);
		m_pcbPerObject->setMatrix("gWVP", wvp);
		m_pcbPerObject->setMatrix("gTexMtx", tex);
		m_pcbPerObject->unmap();
		//const std::array<BufferRawPtr, 1> ppBuffers = { m_pcbPerObject->buffer() };
		ppBuffers[0] = m_pcbPerObject->buffer();
		m_pvsBasic->bindContantBuffers(
			m_pcbPerObject->bindPoint(),
			ppBuffers.size(),
			ppBuffers.data());

		// Set the mirror's texture only and re-bind the pixel shader resources.
		m_ppsBasic->bindResources(
			3,
			1,
			&m_pMirrorRV.p);

		// Draw the mirror.
		m_pRoom->drawMirror();

		// Reset the blend state.
		md3dDeviceContext->OMSetBlendState(nullptr, blendFactors, sampleMask);

		// Reset the depth stencil state.
		md3dDeviceContext->OMSetDepthStencilState(0, 0);

		// Set the box's vertex and index buffers.
		m_pBox->setIndexAndVertexBuffers();

		// set the current world transformation to reflect about the mirror
		const DXPlane reflectPlane(0.0f, 0.0f, 1.0f, 0.0f);	// xy plane
		const DXMatrix reflector(DXMatrix::CreateReflection(reflectPlane));
		world = crateWorld * reflector;	// first set the crate in its start position and then reflect
		//world = DXMatrix::CreateTranslation(DXVector3(2, 1, 2)) * DXMatrix::CreateScale(2.0f);
		wvp = world * viewProj;

		m_pcbPerObject->map();
		m_pcbPerObject->setMatrix("gWorld", world);
		m_pcbPerObject->setMatrix("gWorldInvTrans", world.Invert().Transpose());
		m_pcbPerObject->setMatrix("gWVP", wvp);
		m_pcbPerObject->setMatrix("gTexMtx", tex);
		m_pcbPerObject->unmap();
		//const std::array<BufferRawPtr, 1> ppBuffers = { m_pcbPerObject->buffer() };
		ppBuffers[0] = m_pcbPerObject->buffer();
		m_pvsBasic->bindContantBuffers(
			m_pcbPerObject->bindPoint(),
			ppBuffers.size(),
			ppBuffers.data());

		// Set the box's texture only and re-bind the pixel shader resources.
		m_ppsBasic->bindResources(
			3,
			1,
			&m_pCrateRV.p);

		// update the parallel light's dir to also reflect
		const DXVector3 oriLightDir = m_parallelLights[0].dir;
		m_parallelLights[0].dir = DXVector3::TransformNormal(m_parallelLights[0].dir, reflector);

		// Update the parallel light's sb and bind to the pipeline.
		m_sbParallelLights->setBuffer(m_parallelLights);
		ppResources[0] = m_sbParallelLights->srv();
		m_ppsBasic->bindResources(0, 1, &ppResources[0]);

		// set the mirror reflection's blend state
		blendFactors[0] = blendFactors[1] = blendFactors[2] = 0.25f;
		md3dDeviceContext->OMSetBlendState(m_pbsMirror.p, blendFactors, sampleMask);

		// set the mirror reflection's depth-stencil state
		md3dDeviceContext->OMSetDepthStencilState(m_pdssReflectedCrate.p, 1);

		// set the reflected crate's rasterizer state
		md3dDeviceContext->RSSetState(m_prsReflectedCrate.p);

		// draw the reflected crate
		m_pBox->draw();

		// reset the reflected crate's rasterizer state
		md3dDeviceContext->RSSetState(0);

		// reset the depth-stencil state
		md3dDeviceContext->OMSetDepthStencilState(0, 0);

		// reset the blend state
		md3dDeviceContext->OMSetBlendState(0, blendFactors, sampleMask);

		// Revert the light's original dir.
		m_parallelLights[0].dir = oriLightDir;
		m_sbParallelLights->setBuffer(m_parallelLights);
	}
	else
	{
		// Draw the labels for the GBuffer textures
		const float windowWidth(mClientWidth), windowHeight(mClientHeight);
		mSpriteBatch->Begin();
		mFont->DrawString(mSpriteBatch.get(), L"Loading models", DXVector2(0.4f*windowWidth, 0.5f*windowHeight - mFont->GetLineSpacing()));	// Bottom left
		mSpriteBatch->End();
	}
}

// Define to build the shaders which will be used by the application.
void SemiReflectiveWindowApp::buildShaders()
{
	// Construct the basic vertex and pixel shaders.
	m_pvsBasic.reset(
		new DXVertexShader(
			constructShaderPath(
				m_pSceneBuilder->getShaderRootW(), 
				wpath(L"basicShadingVS.cso")), 
			md3dDevice) );

	m_ppsBasic.reset(
		new DXPixelShader(
			constructShaderPath(
				m_pSceneBuilder->getShaderRootW(),
				wpath(L"basicShadingPS.cso")),
			md3dDevice) );

	// Create the constant buffers.
	m_pcbPerObject.reset(
		new ShaderConstantBuffer(
			"cbPerObject",
			md3dDevice,
			md3dDeviceContext,
			m_pvsBasic->byteCode() ) );

	m_pcbPerFrame.reset(
		new ShaderConstantBuffer(
			"cbPerFrame",
			md3dDevice,
			md3dDeviceContext,
			m_ppsBasic->byteCode() ) );

	// TODO: Add implementation here.
}

// Define to build the vertex layout which will be bound to the input assembly stage
void SemiReflectiveWindowApp::buildVertexLayouts() 
{
	// Use the vertex layout of the basic shading VS.
	m_pVertexLayout = m_pvsBasic->inputLayout();

	// TODO: Add implementation here.
}

// Functor to load the scene models.
void SemiReflectiveWindowApp::loadModels()
{
	::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);	// Need to call this to initialize WICTextureLoader in order to load textures on this thread.

	// Init the room object.
	m_pRoom->init(md3dDevice.p, 1.0f);

	// Init the box object.
	m_pBox->init(md3dDevice.p, 1.0f);

	// Load the scene's models.
	m_pSceneBuilder->buildBasicModels(m_models);
	for (BasicModelDirectory::iterator i = m_models.begin(); i != m_models.end(); ++i)
	{
		i->second.pModel->init(md3dDevice, 1.0f);
	}

	// We're done.
	m_bModelsLoaded = true;
}

// Recursive function which traverses the model's node tree and draws the meshes at each node.
void SemiReflectiveWindowApp::drawModel(const BasicModelPtr &pModel, const DXModelNode &currNode, const DXMatrix &viewProj, std::vector<DXMatrix> &transformationStack)
{
	// Push the current node's transformation onto the transformation stack.
	transformationStack.push_back(currNode.transformation);

	// Draw all the meshes belonging to the current node.
	const std::vector<UINT> &currMeshes = currNode.meshIndexes;
	for (std::size_t i = 0; i < currMeshes.size(); ++i)
	{
		// Compute the current world transformation matrix by multiplying by all the matrices in the transformation stack
		// in reverse.
		DXMatrix world(DXMatrix::Identity());
		for (std::vector<DXMatrix>::reverse_iterator j = transformationStack.rbegin(); j != transformationStack.rend(); ++j)
		{
			world *= *j;
		}			
		
		// Compute the current WVP matrix and set it.
		const DXMatrix wvp(world * viewProj);
		//m_pTexEffect->setWVP(wvp);

		// Update m_cbPerObject and bind it to the pipeline.
		m_pcbPerObject->map();
		m_pcbPerObject->setMatrix("gWorld", world);
		m_pcbPerObject->setMatrix("gWorldInvTrans", world.Invert().Transpose());
		m_pcbPerObject->setMatrix("gWVP", wvp);
		m_pcbPerObject->setMatrix("gTexMtx", DXMatrix::Identity());
		m_pcbPerObject->unmap();
		
		m_pvsBasic->bindContantBuffers(
			m_pcbPerObject->bindPoint(),
			1,
			&m_pcbPerObject->pBuffer.p);

		// Set the diffuse and specular textures for the pModel.
		const UINT uiMeshIndex(pModel->getMaterialIndex(currMeshes[i]));
		std::array<ShaderResourceViewRawPtr, 2> ppResources = {
			pModel->getDiffuseTexture(uiMeshIndex).p,
			pModel->getSpecularTexture(uiMeshIndex).p ?
			pModel->getSpecularTexture(uiMeshIndex).p :
			m_pSpecRV.p };
		m_ppsBasic->bindResources(
			3,
			ppResources.size(),
			ppResources.data());

		// Draw the current mesh.
		pModel->draw(currMeshes[i]);

		// TODO: Remove when done testing.
#if 0
		// Set the current diffuse and specular textures and apply the effect.
		///*const UINT uiMeshIndex(m_basicModels["Sponza"].pModel->getMaterialIndex(currMeshes[i]));
		//const ShaderResourceViewPtr &pDiffuse(m_basicModels["Sponza"].pModel->getDiffuseTexture(uiMeshIndex));
		//m_pTexEffect->setDiffuseMap(pDiffuse);
		//const ShaderResourceViewPtr &pSpecular(m_basicModels["Sponza"].pModel->getSpecularTexture(uiMeshIndex));
		//pSpecular.p ? m_pTexEffect->setSpecMap(pSpecular) : m_pSpecularMap;*/

		//// Set the current diffuse and specular textures in the normal mapping effect.
		//// Choose to apply either the texture mapping effect or the normal mapping effect
		//// depending on whether the current material has a normal map or not.
		//if (m_pNormalMappingEffectToggle->getState())
		//{
		//	const ShaderResourceViewPtr &pNormal(m_basicModels["Sponza"].pModel->getNormalTexture(uiMeshIndex));
		//	if (pNormal.p)
		//	{
		//		md3dDeviceContext->IASetInputLayout(m_pNormalMappingEffect->getInputLayout());	// set the input layout of the normal mapping effect

		//		m_pNormalMappingEffect->setWorld(world);
		//		m_pNormalMappingEffect->setWorldInvTrans(world.Invert().Transpose());
		//		m_pNormalMappingEffect->setWVP(wvp);

		//		m_pNormalMappingEffect->setDiffuseMap(pDiffuse);
		//		pSpecular.p ? m_pNormalMappingEffect->setSpecularMap(pSpecular) : m_pSpecularMap;

		//		m_pNormalMappingEffect->setNormalMap(pNormal);
		//		m_pNormalMappingEffect->apply();
		//	}
		//	else
		//		m_pTexEffect->apply();
		//}
		//else
		//	m_pTexEffect->apply();  
#endif // 0		
	}

	// Draw the meshes of all the children of the current node.
	for (std::size_t i = 0; i < currNode.childNodes.size(); ++i)
		drawModel(pModel, currNode.childNodes[i], viewProj, transformationStack);

	// Pop the current node's transformation from the stack.
	transformationStack.pop_back();
}

// TODO: Re-implement iff necessary.
#if 0
// initializes the camera
void SemiReflectiveWindowApp::initCamera()
{
	// declare the camera params
	DXVector3 eyePos(0.0f, 20.0f, -50.0f), lookAt(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f);
	float FOVAngle(0.25f * PI);
	float nearPlane(1.0f), farPlane(1000.0f);
	float aspectRatio((float)mClientWidth / (float)mClientHeight);
	float motionFactor(50.0f), rotationFactor(1.0f);

	// read the camera params from the scene file if it has been provided, else use the default params
	if (sceneFileAvailable())
	{
		m_pSceneBuilder->buildCamera(m_pCamera, aspectRatio, eyePos, lookAt, up, FOVAngle, nearPlane, farPlane, motionFactor, rotationFactor);
	}
	else
	{
		m_pCamera.reset(new DXCamera(eyePos, lookAt, up, FOVAngle, aspectRatio, nearPlane, farPlane, motionFactor, rotationFactor));
	}
}

// handles the camera motion
void SemiReflectiveWindowApp::moveCamera(float dt)
{
	// define keyboard input to rotate camera and to zoom in/out
	/*const*/ float motionScaleFactor = m_pCamera->motionFactor();
	/*const*/ float rotFactor = m_pCamera->rotationFactor();

	if (m_runToggler.getState())
	{
		motionScaleFactor *= 4.0f;	// run
		//rotFactor *= 2.0f;
	}

	if (GetAsyncKeyState('A') & 0x8000)	m_pCamera->slide(-motionScaleFactor * dt, 0, 0);	// move left	
	if (GetAsyncKeyState('D') & 0x8000)	m_pCamera->slide(motionScaleFactor * dt, 0, 0);	// move right
	if (GetAsyncKeyState('W') & 0x8000)	m_pCamera->slide(0, 0, motionScaleFactor * dt);		// move forward
	if (GetAsyncKeyState('S') & 0x8000)	m_pCamera->slide(0, 0, -motionScaleFactor * dt);		// move backward
	if (GetAsyncKeyState('Q') & 0x8000)	m_pCamera->slide(0, motionScaleFactor * dt, 0);			// move up
	if (GetAsyncKeyState('E') & 0x8000)	m_pCamera->slide(0, -motionScaleFactor * dt, 0);			// move down

	if (GetAsyncKeyState(VK_NUMPAD4) & 0x8000)	m_pCamera->rotateY(-rotFactor * dt);	// yaw left
	if (GetAsyncKeyState(VK_NUMPAD6) & 0x8000)	m_pCamera->rotateY(rotFactor * dt);	// yaw right
	if (GetAsyncKeyState(VK_NUMPAD8) & 0x8000)	m_pCamera->pitch(-rotFactor * dt);		// pitch up
	if (GetAsyncKeyState(VK_NUMPAD5) & 0x8000)	m_pCamera->pitch(rotFactor * dt);		// pitch down

	// NOTE: Disabling rolls as we don't really need them.
#if 0
	if (GetAsyncKeyState(VK_NUMPAD7) & 0x8000)	m_pCamera->roll(rotFactor * dt);			// roll left
	if (GetAsyncKeyState(VK_NUMPAD9) & 0x8000)	m_pCamera->roll(-rotFactor * dt);			// roll right
#endif	// 0

	// zoom in/out according to the keyboard input
	if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000)	m_pCamera->zoomOut();	// zoom out
	if (GetAsyncKeyState(VK_NUMPAD3) & 0x8000)	m_pCamera->zoomIn();	// zoom in
}
#endif // 0

// Not really useful at this point. Use if deemed necessary.
#if 0
// Convenience overrides for handling mouse input.
void SemiReflectiveWindowApp::OnMouseDown(WPARAM btnState, int x, int y)	// {}
{
	// update the last mouse position
	m_lastMousePos.x = static_cast<float>(x);
	m_lastMousePos.y = static_cast<float>(y);

	// set the D3D window to capture mouse events
	SetCapture(mhMainWnd);
}

void SemiReflectiveWindowApp::OnMouseUp(WPARAM btnState, int x, int y) // {}
{
	// release the D3D window from capturing mouse events
	ReleaseCapture();
}

void SemiReflectiveWindowApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	// only move the camera if the LMB is pressed
	if (btnState & MK_LBUTTON != 0)
	{
		// compute the current x and y offset relative to the last known mouse position
		const float dx = DirectX::XMConvertToRadians(m_pCamera->rotationFactor() * (m_lastMousePos.x - static_cast<float>(x)));
		const float dy = DirectX::XMConvertToRadians(m_pCamera->rotationFactor() * (m_lastMousePos.y - static_cast<float>(y)));

		// pitch the camera by dy and yaw the camera by dx
		m_pCamera->pitch(-dy);
		m_pCamera->rotateY(-dx);
	}
	
	// update the last known mouse position
	m_lastMousePos.x = x;
	m_lastMousePos.y = y;
}
#endif	// 0