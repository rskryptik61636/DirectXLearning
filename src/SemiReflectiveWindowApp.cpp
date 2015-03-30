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
		world = DXMatrix::CreateTranslation(DXVector3(-2, 0, -2)) * DXMatrix::CreateScale(2.0f);
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