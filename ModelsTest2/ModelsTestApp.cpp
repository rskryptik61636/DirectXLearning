// Test class to play around with loading models using Assimp - source file

#include "ModelsTestApp.h"

#include <DXShaderUtils.h>

#include <DXStringUtils.h>

#include <Vertex.h>

#include <RenderStateMaker.h>

#include <map>

#include <sstream>

#include <random>

// @TODO: Refactor this into a header file.
#include <nvToolsExt.h>

#ifndef _WIN64
#pragma message("Linking against nvToolsExt32_1.lib")
#pragma comment(lib, "nvToolsExt32_1.lib")
#else
#pragma message("Linking against nvToolsExt64_1.lib")
#pragma comment(lib, "nvToolsExt64_1.lib")
#endif	// !_WIN64

// Parameterized ctor, creates a DirectX app given the hInstance and the path to a scene description file
// @param: hInstance = handle to the Windows app instance
// @param: strSceneFilePath = absolute path to the scene description file (TODO: decide on the scene description file format)
ModelsTestApp::ModelsTestApp(HINSTANCE hInstance, const std::string strSceneFilePath)
: DXApp(hInstance, strSceneFilePath), m_prevViewProj(DXMatrix::Identity()), 
m_fSceneScale(1.0f), m_pGBufferOutput(new ToggleState(L"(G)Buffer output", 'G')),
m_nShadowMapWidth(2048), m_nShadowMapHeight(2048), m_fPointLightsPatternRadius(350.0f),
m_pRainToggler(new ToggleState(L"Rain", 'R'))
{}

// Destructor
ModelsTestApp::~ModelsTestApp()
{}

// Initializes the DirectX application's objects and resources
void ModelsTestApp::initApp()
{
	// @TODO: update as/when required
	DXApp::initApp();
}

// Called when the client window is resized
void ModelsTestApp::onResize()
{
	// Run the base class' version first
	DXApp::onResize();

	// Resize the GBuffer
	if (m_pGBuffer)	m_pGBuffer->recreateBuffers(md3dDevice, mClientWidth, mClientHeight);

	// Resized the tiled output buffer.
	if (m_pDeferredShadingEffect)	m_pDeferredShadingEffect->resizeTiledOutput(mClientWidth, mClientHeight);

}

// Called when Windows events have to be processed and there is something that the application might have to take care off
// @param: dt = delta time interval of the mGameTimer member
void ModelsTestApp::updateScene(float dt)
{
	//// @TODO: update as/when required
	//DXApp::updateScene(dt);

	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.

	static int frameCnt = 0;
	static float t_base = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((mTimer.getGameTime() - t_base) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		// Compute the average draw time in the elapsed period
		const float avgDrawTime = mDrawTime / (float)mDrawCounter;
		const float avgUpdateTime = mUpdateTime / (float)frameCnt;

		// Get the current camera position
		//const DXVector3 cameraPos(m_pCamera->eyePosW());

		std::wostringstream outs;
		outs.precision(6);
		outs << L"FPS: " << fps << std::endl
			<< L"Frame render time: " << mspf << L" ms" << std::endl
			<< L"Cam pos: " << threeVectorToWideString(m_pCamera->eyePosW()) << std::endl;
			//<< m_pDeferredShadingEffect->getEffectInfo();			
		mFrameStats = outs.str();	// @TODO: put back when done recording

		// Append the texture mapping and normal mapping shader status' to the frame stats string.
		//mFrameStats += 

		// Reset the draw stats
		mDrawTime = 0.0f;
		mDrawCounter = 0;

		// Reset the update stats
		mUpdateTime = 0;
		mUpdateCounter = 0;

		// Reset for next average.
		frameCnt = 0;
		t_base += 1.0f;
	}

	// Query the 'run' toggle state
	m_runToggler.toggleState();

	// Move the camera
	moveCamera(dt);

	// Toggle the deferred shading effect's states
	m_pDeferredShadingEffect->toggleStates();

	// Toggle the GBuffer output state
	m_pGBufferOutput->toggleState();

	// Move the point light pattern
	movePointLightsPattern(dt, m_fPointLightsPatternRadius, m_stationaryPointLights.size()-1, m_pointLights.size()-1, m_pointLights);

	// Testing the revolving point light.
	// update the position of the spot light to circle the scene as a function of the current time
	if (m_spotLights.size() > 0)
	{
		const float speedFactor(1.0f), spinRadius(750.0f)/*, height(650.0f)*/;

		for (std::size_t i = 0; i < m_spotLights.size(); ++i)
		{
			const float height(m_spotLights[i].position.y);

			//m_spotLights[i].position = DXVector3(0.0f, -150.0f, -150.0f);
			//m_spotLights[i].position = DXVector3(spinRadius * cosf(mTimer.getGameTime()*speedFactor), height, spinRadius * sinf(mTimer.getGameTime()*speedFactor));

			// update the direction of the spot light to point straight down
			//m_spotLights[0].dir = DXVector3(0, -1, 0);
			/*const DXVector3 lookAt(0.0f, height, 0.0f);
			m_spotLights[i].dir = DXVector3(lookAt - m_spotLights[i].position);
			m_spotLights[i].dir.Normalize();*/
		}
	}

	// Toggle the rain state and set the time step iff necessary.
	m_pRainToggler->toggleState();
	if (m_pRainToggler->getState())	m_particleSystems["Rain"].pSystem->setTimeStep(dt);

	// Toggle the rain state.
	
}

// Renders the scene
void ModelsTestApp::drawScene()
{
	// @TODO: update as/when required
	DXApp::drawScene();
}

// Define resources such as textures, blend/rasterizer/depth-stencil states and so on
void ModelsTestApp::createResources()
{
	// Get the texture root path.
	const wpath textureRoot = m_pSceneBuilder->getTextureRootW();

	// Load the default specular map.
	/*const*/ wpath texturePath = textureRoot / wpath(L"defaultspec.dds");
	HR(loadTexture(md3dDevice, md3dDeviceContext, texturePath, NULL, &m_pSpecularMap.p));

	// Load the raindrop texture.
	texturePath = textureRoot / wpath(L"raindrop.dds");
	HR(loadTexture(md3dDevice, md3dDeviceContext, texturePath, &m_textures["Raindrop"].pResource.p, &m_textures["Raindrop"].pResourceView.p));

	// Load the grass skybox textgure.
	texturePath = textureRoot / wpath(L"grasscube1024.dds");
	HR(loadTexture(md3dDevice, md3dDeviceContext, texturePath, &m_textures["GrassCube"].pResource.p, &m_textures["GrassCube"].pResourceView.p));

	// Create the additive blend state
	RenderStateMaker renderStateMaker;
	renderStateMaker.createAdditiveBlendingBS(md3dDevice, m_pAdditiveBS);

	// @TODO: add implementation here
}

// Define scene objects which need to be rendered
void ModelsTestApp::createObjects()
{
	// Init the basic models dir and all its model instances.
	m_pSceneBuilder->buildBasicModels(m_basicModels);
	for (BasicModelDirectory::iterator i = m_basicModels.begin(); i != m_basicModels.end(); ++i)
	{
		i->second.pModel->init(md3dDevice, 1.0f);
	}

#if 0
	// Load the model (argv[1]) using Assimp::Importer with the appropriate options for use in DX.
	//const npath modelPath("N:\\assimp--3.0.1270-sdk\\test\\models\\OBJ\\spider.obj");
	const npath modelPath("N:\\DirectXLearning\\models\\Sponza\\sponza.obj");
	//const npath modelPath("C:\\ProgramData\\NVIDIA Corporation\\NVIDIA Direct3D SDK 11\\Media\\sponza\\SponzaNoFlag.x");
	m_basicModels["Sponza"].pModel.reset(new DXBasicModel(modelPath));
	m_basicModels["Sponza"].pModel->init(md3dDevice, 1.0f);
#endif // 0


	// Init the full-screen quad
	m_pFullscreenQuad.reset(new ComplexQuad());
	m_pFullscreenQuad->init(md3dDevice, 1.0f);

	// @TODO: add implementation here
}

// Meat of the drawScene method, this is where the scene rendering is implemented
void ModelsTestApp::drawObjects()
{
	// Render the scene to generate the shadow map used for shadow mapping
	// @TODO: fix to only be performed if shadow mapping is enabled

	// Set the input layout and the primitive topology.
	md3dDeviceContext->IASetInputLayout(m_pDeferredShadingEffect->getInputLayout().p);
	md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the default depth stencil state.
	md3dDeviceContext->OMSetDepthStencilState(0, 0);

	if (m_pDeferredShadingEffect->doingShadowMapping())
	{
		// Add frame marker push.
		nvtxRangePushA("Shadow mapping");

		// Clear the shadow map's depth-stencil view and the spot lights' shadow transforms
		m_pShadowMap->ClearDepthStencilBuffer(md3dDeviceContext);
		m_spotLightsShadowTransforms.clear();
		const DXMatrix lightTangent(DXMatrix::CreateScale(0.5f, -0.5f, 1.0f) * DXMatrix::CreateTranslation(0.5f, 0.5f, 0.0f));

		for (std::size_t i = 0; i < m_spotLights.size(); ++i)
		{
			// Bind the shadow map's depth buffer
			m_pShadowMap->BindSubDsvAndSetNullRenderTarget(md3dDeviceContext, i);

			// Set the effect params
			const DXVector3 eye(m_spotLights[i].position), lookAt(m_spotLights[i].position + 1000.0f*m_spotLights[i].dir), up(0.0f, 1.0f, 0.0f);
			const float fov(45.0f), aspectRatio(static_cast<float>(m_nShadowMapWidth) / static_cast<float>(m_nShadowMapHeight)), nearPlane(10.0f), farPlane(/*500.0f*/m_spotLights[i].range);
			const DXMatrix viewProj(DXMatrix::CreateLookAt(eye, lookAt, up) * DXMatrix::CreatePerspectiveFieldOfView(fov, aspectRatio, nearPlane, farPlane));
			DXMatrix world(m_basicModels["Sponza"].getTransform()), texMtx(DXMatrix::Identity());
			DeferredShadingEffect::EffectParams eParams;
			eParams.ePass = DeferredShadingEffect::EP_G_BUFFER_GENERATION;
			eParams.texMtx = &texMtx;

			// Setup the model's root node's 1st child for drawing and render.
			m_basicModels["Sponza"].pModel->setVertexAndIndexBuffers();
			std::vector<DXMatrix> transformationStack(1, world);
			renderModelGBuffer(m_basicModels["Sponza"].pModel->rootNode(), m_basicModels["Sponza"].pModel, viewProj, transformationStack, eParams);

			// Setup the spider model for drawing and render
			m_basicModels["Spider1"].pModel->setVertexAndIndexBuffers();
			world = m_basicModels["Spider1"].getTransform();
			transformationStack.resize(1);	transformationStack[0] = world;
			ZeroMemory(&eParams, sizeof(eParams));
			eParams.ePass = DeferredShadingEffect::EP_G_BUFFER_GENERATION;
			eParams.texMtx = &texMtx;
			renderModelGBuffer(m_basicModels["Spider1"].pModel->rootNode(), m_basicModels["Spider1"].pModel, viewProj, transformationStack, eParams);

			// Add the current spot light's shadow transform
			m_spotLightsShadowTransforms.push_back(DXMatrix(viewProj * lightTangent)/*.Transpose()*/);	// quick and dirty hack, fix when done testing
		}

		// reset the default viewport
		md3dDeviceContext->RSSetViewports(1, &mDefaultViewport);

		// Add frame marker pop.
		nvtxRangePop();
	}

	// Add renderGBuffer marker push.
	nvtxRangePushA("GBuffer generation");

	// Render the scene to the deferred shading effect's GBuffer.
	renderGBuffer();

	// Clear the depth-stencil view
	md3dDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Reset the default render targets and depth-stencil view
	md3dDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	// Add renderGBuffer marker pop.
	nvtxRangePop();

	// Render the deferred shading output if the GBuffer output toggle state is not set,
	// else render the GBuffer output
	if (!m_pGBufferOutput->getState())
	{
		// Add shading marker push.
		nvtxRangePushA("Shading");

		// Set the effect params
		DXMatrix world(DXMatrix::Identity()), texMtx(DXMatrix::Identity()), wvp(DXMatrix::Identity());
		DeferredShadingEffect::EffectParams eParams;
		eParams.eTech = DeferredShadingEffect::ET_TILED;	// @TODO: Update to switch based on a user defined param.
		eParams.ePass = DeferredShadingEffect::EP_DEFERRED_SHADING;
		eParams.texMtx = &texMtx;
		eParams.world = &world;
		eParams.worldInvTrans = &world.Invert().Transpose();
		eParams.wvp = &wvp;
		eParams.pPositionMap = m_pGBuffer->positionMap()->GetShaderResource();
		eParams.pNormalMap = m_pGBuffer->normalMap()->GetShaderResource();
		eParams.pDiffuseMap = m_pGBuffer->diffuseMap()->GetShaderResource();
		eParams.pSpecularMap = m_pGBuffer->specularMap()->GetShaderResource();
		eParams.eyePosW = m_pCamera->eyePosW();
		eParams.parallelLights = &m_parallelLights;
		eParams.pointLights = &m_pointLights;
		eParams.spotLights = &m_spotLights;
		eParams.shadowMapDims = DXVector2(m_pShadowMap->width(), m_pShadowMap->height());
		eParams.pShadowMap = m_pShadowMap->DepthMapSRV();
		eParams.spotLightsShadowTransforms = &m_spotLightsShadowTransforms;

		// motion blur params
		DXMatrix viewProj(m_pCamera->view() * m_pCamera->proj());
		eParams.currViewProj = &viewProj;
		eParams.prevViewProj = &m_prevViewProj;
		eParams.nMotionBlurSamples = 150;	// TODO: revise as necessary, ideally should be read from the scene file
		//eParams.nMotionBlurSamples = 150;	// TODO: revise as necessary, ideally should be read from the scene file

		//// Set the additive blend state
		//const FLOAT blendFactors[4] = { 0.65f, 0.65f, 0.65f, 1.0f };
		//const UINT sampleMask(0xFFFFFFFF);
		//md3dDeviceContext->OMSetBlendState(m_pAdditiveBS.p, blendFactors, sampleMask);
		///*md3dDeviceContext->OMSetBlendState(m_pAdditiveBS.p, 0, 0xFFFFFFFF);*/

		m_pDeferredShadingEffect->apply(eParams);		

		// Draw the full screen quad
		m_pFullscreenQuad->draw();		

		// Cleanup
		m_pDeferredShadingEffect->cleanup();

		// Display the effect states // @TODO: put back when done recording
		//m_pDeferredShadingEffect->dispEffectStates(mSpriteBatch, mFont, mClientWidth, mClientHeight);

		//// Restore the default blend state
		//md3dDeviceContext->OMSetBlendState(0, 0, sampleMask);

		// Store the current view projection matrix for the next frame
		m_prevViewProj = *eParams.currViewProj;

		//// Draw the skybox
		//SkyboxEffect::EffectParams skyboxParams;
		//skyboxParams.eyePosW = m_pCamera->eyePosW();
		//skyboxParams.scale = 5.0f;
		//skyboxParams.pSkyboxTexture = m_textures["GrassCube"].pResourceView.p;
		//skyboxParams.viewProj = m_pCamera->view() * m_pCamera->proj();
		//m_pSkybox->apply(skyboxParams);

		// @TODO: remove when done testing
#if 0
		// Render the rain effect iff enabled.
		if (m_pRainToggler->getState())
		{
			m_particleSystems["Rain"].params.eyePosW = m_pCamera->eyePosW();
			m_particleSystems["Rain"].params.viewProj = m_pCamera->view() * m_pCamera->proj();
			m_particleSystems["Rain"].params.pParticleTexture = m_textures["Raindrop"].pResourceView.p;
			m_particleSystems["Rain"].pSystem->apply(m_particleSystems["Rain"].params);

			//// Set the input layout and the primitive topology.
			//md3dDeviceContext->IASetInputLayout(m_pDeferredShadingEffect->getInputLayout().p);
			//md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
#endif // 0

		// Add shading marker pop.
		nvtxRangePop();		
	}
	else
	{
		// Debug the GBuffer's textures

		// Add GBuffer Debug marker push.
		nvtxRangePushA("GBuffer Debug");

		// Normal map
		const DXMatrix debugTransform(DXMatrix::CreateScale(0.5));
		DXMatrix currTilePos = DXMatrix::CreateTranslation(-0.5f, -0.5f, 0.0f);	// bottom left
		m_pDebugTextureEffect->setWVP(debugTransform * currTilePos);
		m_pDebugTextureEffect->setTexMtx(DXMatrix::Identity());
		m_pDebugTextureEffect->setTextureIndex(DebugTextureEffect::DC_ALL);
		m_pDebugTextureEffect->setDebugTexture(m_pGBuffer->normalMap()->GetShaderResource());
		m_pDebugTextureEffect->setMultiplierTerm(0.5f);	// To scale the normal map values from [-1,1] to [0,1]
		m_pDebugTextureEffect->setAdditiveTerm(0.5f);
		m_pDebugTextureEffect->apply();

		// Draw the full screen quad.
		m_pFullscreenQuad->draw();

		// Depth map
		currTilePos = DXMatrix::CreateTranslation(0.5f, -0.5f, 0.0f);	// bottom right
		m_pDebugTextureEffect->setWVP(debugTransform * currTilePos);
		m_pDebugTextureEffect->setTextureIndex(DebugTextureEffect::DC_RED);
		m_pDebugTextureEffect->setDebugTexture(/*m_pShadowMap->DepthMapSRV()*/m_pGBuffer->depthMap()->GetShaderResource());
		m_pDebugTextureEffect->setMultiplierTerm(1.0f);	// Reset the terms to default values.
		m_pDebugTextureEffect->setAdditiveTerm(0.0f);
		m_pDebugTextureEffect->apply();

		// Draw the full screen quad.
		m_pFullscreenQuad->draw();

		// Diffuse map
		currTilePos = DXMatrix::CreateTranslation(-0.5f, 0.5f, 0.0f);	// top left
		m_pDebugTextureEffect->setWVP(debugTransform * currTilePos);
		m_pDebugTextureEffect->setTextureIndex(DebugTextureEffect::DC_ALL);
		m_pDebugTextureEffect->setDebugTexture(m_pGBuffer->diffuseMap()->GetShaderResource());
		m_pDebugTextureEffect->apply();

		// Draw the full screen quad.
		m_pFullscreenQuad->draw();

		// Specular map
		currTilePos = DXMatrix::CreateTranslation(0.5f, 0.5f, 0.0f);	// top right
		m_pDebugTextureEffect->setWVP(debugTransform * currTilePos);
		m_pDebugTextureEffect->setTextureIndex(DebugTextureEffect::DC_ALL);
		m_pDebugTextureEffect->setDebugTexture(m_pGBuffer->specularMap()->GetShaderResource());
		m_pDebugTextureEffect->apply();

		// Draw the full screen quad.
		m_pFullscreenQuad->draw();

		// Cleanup
		m_pDebugTextureEffect->cleanup();

		// Draw the labels for the GBuffer textures
		const float windowWidth(mClientWidth), windowHeight(mClientHeight);
		mSpriteBatch->Begin();
		mFont->DrawString(mSpriteBatch.get(), L"Normal map", DXVector2(0.2f*windowWidth, windowHeight - mFont->GetLineSpacing()));	// Bottom left
		mFont->DrawString(mSpriteBatch.get(), L"Depth map", DXVector2(0.725f*windowWidth, windowHeight - mFont->GetLineSpacing()), BLACK);	// Bottom right

		mFont->DrawString(mSpriteBatch.get(), L"Diffuse map", DXVector2(0.2f*windowWidth, 0.5f*windowHeight - mFont->GetLineSpacing()));	// Top left
		mFont->DrawString(mSpriteBatch.get(), L"Specular map", DXVector2(0.725f*windowWidth, 0.5f*windowHeight - mFont->GetLineSpacing()));	// Top right
		mSpriteBatch->End();

		// Add GBuffer Debug pop.
		nvtxRangePop();
	}	

	// @TODO: keep just what we need
#if 0
	// Set the input layout and the primitive topology.
	md3dDeviceContext->IASetInputLayout(m_pVertexLayout.p);
	md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the default depth stencil state.
	md3dDeviceContext->OMSetDepthStencilState(0, 0);

	// Set the TexEffect params
	const DXMatrix viewProj(m_pCamera->view() * m_pCamera->proj());
	DXMatrix world(/*DXMatrix::CreateRotationY(PI/2)*/DXMatrix::Identity()), texMtx(DXMatrix::Identity());
	//DXMatrix wvp(world * viewProj);
	//m_pTexEffect->setWorld(world);
	//m_pTexEffect->setWVP(wvp);
	m_pTexEffect->setTexMtx(texMtx);

	//m_pTexEffect->setSpecMap(m_pSpecularMap);
	m_pTexEffect->setEyePosW(m_pCamera->eyePosW());
	m_pTexEffect->setCurrTime(mTimer.getGameTime());
	m_pTexEffect->setParallelLights(m_parallelLights);
	m_pTexEffect->setSpotLights(m_spotLights);
	m_pTexEffect->setPointLights(m_pointLights);
	//m_pTexEffect->setLight(m_parallelLights[0]);	

	// Set the NormalMappingEffect params if the corresponding flag is set
	if (m_pNormalMappingEffectToggle->getState())
	{
		/*m_pNormalMappingEffect->setWorld(world);
		m_pNormalMappingEffect->setWorldInvTrans(world.Invert().Transpose());*/
		m_pNormalMappingEffect->setTexMtx(texMtx);

		m_pNormalMappingEffect->setEyePosW(m_pCamera->eyePosW());
		m_pNormalMappingEffect->setParallelLights(m_parallelLights);
		m_pNormalMappingEffect->setSpotLights(m_spotLights);
		m_pNormalMappingEffect->setPointLights(m_pointLights);
	}

	// Setup the model's root node's 1st child for drawing and render.
	m_basicModels["Sponza"].pModel->setVertexAndIndexBuffers();
	std::vector<DXMatrix> transformationStack(1, world);
	drawModelTex(m_basicModels["Sponza"].pModel->rootNode(), viewProj, transformationStack);

	// @TODO: add implementation here
#endif	// 0

}

// Renders the scene to the deferred shading effect's GBuffer
void ModelsTestApp::renderGBuffer()
{
	//// Set the input layout and the primitive topology.
	//md3dDeviceContext->IASetInputLayout(m_pDeferredShadingEffect->getInputLayout().p);
	//md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//// Set the default depth stencil state.
	//md3dDeviceContext->OMSetDepthStencilState(0, 0);

	// NOTE: Attempting z pre-pass by rendering the scene without render targets assigned to see if that helps any
#if 0
	if (false)
	{
		// Bind and unbind the GBuffer render targets to leave only the depth buffer bound.
		m_pGBuffer->bindRenderTargets(md3dDeviceContext);
		m_pGBuffer->unbindRenderTargets(md3dDeviceContext);
		//m_pGBuffer->clearRenderTargets(md3dDeviceContext);
		m_pGBuffer->clearDSV(md3dDeviceContext);

		// Set the effect params
		const DXMatrix viewProj(m_pCamera->view() * m_pCamera->proj());
		DXMatrix world(/*DXMatrix::Identity()*/DXMatrix::CreateScale(m_fSceneScale)), texMtx(DXMatrix::Identity());
		DeferredShadingEffect::EffectParams eParams;
		eParams.ePass = DeferredShadingEffect::EP_G_BUFFER_GENERATION;
		eParams.texMtx = &texMtx;

		// Setup the model's root node's 1st child for drawing and render.
		m_basicModels["Sponza"].pModel->setVertexAndIndexBuffers();
		std::vector<DXMatrix> transformationStack(1, world);
		renderModelGBuffer(m_basicModels["Sponza"].pModel->rootNode(), m_basicModels["Sponza"].pModel, viewProj, transformationStack, eParams);

		// Setup the spider model for drawing and render
		m_basicModels["Spider1"].pModel->setVertexAndIndexBuffers();
		world = m_basicModels["Spider1"].getTransform();
		transformationStack.resize(1);	transformationStack[0] = world;
		ZeroMemory(&eParams, sizeof(eParams));
		eParams.ePass = DeferredShadingEffect::EP_G_BUFFER_GENERATION;
		eParams.texMtx = &texMtx;
		renderModelGBuffer(m_basicModels["Spider1"].pModel->rootNode(), m_basicModels["Spider1"].pModel, viewProj, transformationStack, eParams);

		// Render the rain effect iff enabled.
		if (m_pRainToggler->getState())
		{
			m_particleSystems["Rain"].params.eyePosW = m_pCamera->eyePosW();
			m_particleSystems["Rain"].params.viewProj = m_pCamera->view() * m_pCamera->proj();
			m_particleSystems["Rain"].params.pParticleTexture = m_textures["Raindrop"].pResourceView.p;
			m_particleSystems["Rain"].pSystem->apply(m_particleSystems["Rain"].params);

			// Set the input layout and the primitive topology.
			md3dDeviceContext->IASetInputLayout(m_pDeferredShadingEffect->getInputLayout().p);
			md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}

		// Unbind the GBuffer's render targets and cleanup
		m_pGBuffer->unbindRenderTargets(md3dDeviceContext);
		m_pDeferredShadingEffect->cleanup();
	}
#endif // 0


	// Bind the GBuffer render targets
	m_pGBuffer->bindRenderTargets(md3dDeviceContext);
	m_pGBuffer->clearRenderTargets(md3dDeviceContext);
	m_pGBuffer->clearDSV(md3dDeviceContext);

	// Set the effect params
	const DXMatrix viewProj(m_pCamera->view() * m_pCamera->proj());
	DXMatrix world(/*DXMatrix::Identity()*/DXMatrix::CreateScale(m_fSceneScale)), texMtx(DXMatrix::Identity());
	DeferredShadingEffect::EffectParams eParams;
	eParams.ePass = DeferredShadingEffect::EP_G_BUFFER_GENERATION;
	eParams.texMtx = &texMtx;

	// Setup the model's root node's 1st child for drawing and render.
	m_basicModels["Sponza"].pModel->setVertexAndIndexBuffers();
	std::vector<DXMatrix> transformationStack(1, world);
	renderModelGBuffer(m_basicModels["Sponza"].pModel->rootNode(), m_basicModels["Sponza"].pModel, viewProj, transformationStack, eParams);

	// Setup the spider model for drawing and render
	m_basicModels["Spider1"].pModel->setVertexAndIndexBuffers();
	world = m_basicModels["Spider1"].getTransform();
	transformationStack.resize(1);	transformationStack[0] = world;
	ZeroMemory(&eParams, sizeof(eParams));
	eParams.ePass = DeferredShadingEffect::EP_G_BUFFER_GENERATION;
	eParams.texMtx = &texMtx;
	renderModelGBuffer(m_basicModels["Spider1"].pModel->rootNode(), m_basicModels["Spider1"].pModel, viewProj, transformationStack, eParams);

	// Render the rain effect iff enabled.
	if (m_pRainToggler->getState())
	{
		m_particleSystems["Rain"].params.eyePosW = m_pCamera->eyePosW();
		m_particleSystems["Rain"].params.viewProj = m_pCamera->view() * m_pCamera->proj();
		m_particleSystems["Rain"].params.pParticleTexture = m_textures["Raindrop"].pResourceView.p;
		m_particleSystems["Rain"].pSystem->apply(m_particleSystems["Rain"].params);

		// Set the input layout and the primitive topology.
		md3dDeviceContext->IASetInputLayout(m_pDeferredShadingEffect->getInputLayout().p);
		md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	// Unbind the GBuffer's render targets and cleanup
	m_pGBuffer->unbindRenderTargets(md3dDeviceContext);	
	m_pDeferredShadingEffect->cleanup();
}

// Recursive function which traverses the model's node tree and renders the geometry of all the meshes at each node
// into the deferred shading effect's GBuffer.
void ModelsTestApp::renderModelGBuffer(const DXModelNode &currNode, const BasicModelPtr &pModel, const DXMatrix &viewProj,
	std::vector<DXMatrix> &transformationStack, DeferredShadingEffect::EffectParams &eParams)
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
		eParams.world = &world;
		eParams.worldInvTrans = &world.Invert().Transpose();

		// Compute the current WVP matrix and set it.
		DXMatrix wvp(world * viewProj);
		eParams.wvp = &wvp;
		//eParams.wvp = world * viewProj;

		// Set the current diffuse, specular and normal textures and apply the effect.
		const UINT uiMeshIndex(/*m_basicModels["Sponza"].*/pModel->getMaterialIndex(currMeshes[i]));
		const ShaderResourceViewPtr &pDiffuse(/*m_basicModels["Sponza"].*/pModel->getDiffuseTexture(uiMeshIndex));
		eParams.pDiffuseMap = pDiffuse;
		const ShaderResourceViewPtr &pSpecular(/*m_basicModels["Sponza"].*/pModel->getSpecularTexture(uiMeshIndex));
		eParams.pSpecularMap = pSpecular ? pSpecular : m_pSpecularMap;
		const ShaderResourceViewPtr &pNormal(/*m_basicModels["Sponza"].*/pModel->getNormalTexture(uiMeshIndex));
		eParams.pNormalMap = pNormal ? pNormal : ShaderResourceViewPtr();

		// Apply the deferred shading effect
		m_pDeferredShadingEffect->apply(eParams);

		// Draw the current mesh.
		/*m_basicModels["Sponza"].*/pModel->draw(currMeshes[i]);
	}

	// Draw the meshes of all the children of the current node.
	for (std::size_t i = 0; i < currNode.childNodes.size(); ++i)
		renderModelGBuffer(currNode.childNodes[i], pModel, viewProj, transformationStack, eParams);

	// Pop the current node's transformation from the stack.
	transformationStack.pop_back();
}

// Define to build the shaders which will be used by the application
void ModelsTestApp::buildShaders()
{
	// Init the TexEffect instance.
	m_pTexEffect.reset(new TexEffect(m_pSceneBuilder->getShaderRootW(), md3dDevice, md3dDeviceContext));
	m_pTexEffect->init();

	// Init the NormalMappingEffect instance
	m_pNormalMappingEffect.reset(new NormalMappingEffect(m_pSceneBuilder->getShaderRootW(), md3dDevice, md3dDeviceContext));
	m_pNormalMappingEffect->init();

	// Init the NormalMappingEffect's toggle state to toggle on pressing the 'N' key
	//m_pNormalMappingEffectToggle.reset(new ToggleState(L"Normal mapping", 'N'));	// @TODO: remove when done testing

	// Load the parallel lights in the scene.
	ParallelLightParams parallelLightParams;
	parallelLightParams.dir = DXVector3(0.5f, -0.5f, 0.5f);
	parallelLightParams.ambient = DXColor(0.2f, 0.2f, 0.2f, 1.0f);
	parallelLightParams.diffuse = WHITE;
	parallelLightParams.specular = WHITE;
	parallelLightParams.radius = 0.0f;
	parallelLightParams.angle = 0.0f;
	parallelLightParams.animationSpeed = 0.0f;

	// init the scene's parallel lights
	m_pSceneBuilder->buildParallelLights<SLight>(parallelLightParams, m_parallelLights);

	// init the default point light params
	PointLightParams pointLightParams;
	pointLightParams.position = DXVector3(0.0f, 225.0f, 0.0f);	// ceiling top
	pointLightParams.ambient = DXColor(0.2f, 0.2f, 0.2f, 1.0f);
	pointLightParams.diffuse = WHITE;
	pointLightParams.specular = WHITE;
	pointLightParams.att = DXVector3(2.0f, 0.0f, 0.0f);
	pointLightParams.range = 100.0f;
	pointLightParams.radius = 0.0f;
	pointLightParams.angle = 0.0f;
	pointLightParams.animationSpeed = 0.0f;

	// init the scene's point lights w/ a random pattern
	const UINT nPointLights(1024);
	createPointLightsPattern(nPointLights, m_fPointLightsPatternRadius, pointLightParams, m_movingPointLights);
	m_pSceneBuilder->buildPointLights<SLight>(pointLightParams, m_stationaryPointLights);
	m_pointLights = m_stationaryPointLights;
	m_pointLights.insert(m_pointLights.end(), m_movingPointLights.begin(), m_movingPointLights.end());

	// init the default spot light params
	SpotLightParams spotLightParams;
	spotLightParams.ambient = /*RED;*/ DXColor(0.2f, 0.2f, 0.2f, 1.0f);
	spotLightParams.diffuse = BLUE; /*GREEN;*/
	spotLightParams.specular = BLUE;
	spotLightParams.att = DXVector3(1.0f, 0.0f, 0.0f);
	spotLightParams.spotPow = 64.0f;
	spotLightParams.range = 10000.0f;
	spotLightParams.radius = 0.0f;
	spotLightParams.angle = 0.0f;
	spotLightParams.animationSpeed = 0.0f;

	// init the scene's spot lights
	m_pSceneBuilder->buildSpotLights<SLight>(spotLightParams, m_spotLights);

	// Initialize the shadow map's size to that of the spotlights
	m_pShadowMap.reset(new ShadowMap(md3dDevice, m_nShadowMapWidth, m_nShadowMapHeight, m_spotLights.size()));

	// Init the DeferredShadingEffect instance
	m_pDeferredShadingEffect.reset(new DeferredShadingEffect(m_pSceneBuilder->getShaderRootW(), md3dDevice, md3dDeviceContext, mClientWidth, mClientHeight));
	m_pDeferredShadingEffect->init(m_parallelLights.size(), m_pointLights.size(), m_spotLights.size());

	// Init the GBuffer
	m_pGBuffer.reset(new GBuffer(md3dDevice, mClientWidth, mClientHeight));

	// Init the DebugTextureEffect instance
	m_pDebugTextureEffect.reset(new DebugTextureEffect(m_pSceneBuilder->getShaderRootW(), md3dDevice, md3dDeviceContext));
	m_pDebugTextureEffect->init();

	// Init the particle system directory
	m_pSceneBuilder->buildParticleSystems(m_pSceneBuilder->getShaderRootW(), md3dDevice, md3dDeviceContext, m_particleSystems);

	// Init the skybox
	const float fSkyboxRadius(1.0f);
	m_pSkybox.reset(new SkyboxEffect(m_pSceneBuilder->getShaderRootW(), md3dDevice, md3dDeviceContext, fSkyboxRadius));

	// @TODO: add implementation here
}

// Define to build the vertex layout which will be bound to the input assembly stage
void ModelsTestApp::buildVertexLayouts()
{
	// Init the vertex layout from the TexEffect instance.
	HR(md3dDevice->CreateInputLayout(Vertex::GetElementDesc(), Vertex::GetNumElements(),
		m_pTexEffect->getVSBufferPointer(), m_pTexEffect->getVSBufferSize(), &m_pVertexLayout.p));
	
	// Validate the vertex layout against the NormalMappingEffect instance to ensure that there won't be any issues
	InputLayoutPtr pTmp;
	HR(md3dDevice->CreateInputLayout(Vertex::GetElementDesc(), Vertex::GetNumElements(),
		m_pNormalMappingEffect->getVSBufferPointer(), m_pNormalMappingEffect->getVSBufferSize(), &pTmp.p));

	// Validate the DeferredShadingEffect's input layout
	HR(md3dDevice->CreateInputLayout(Vertex::GetElementDesc(), Vertex::GetNumElements(),
		m_pDeferredShadingEffect->getVSBufferPointer(), m_pDeferredShadingEffect->getVSBufferSize(), &pTmp.p));

	// Validate the DebugTextureEffect's input layout
	HR(md3dDevice->CreateInputLayout(Vertex::GetElementDesc(), Vertex::GetNumElements(),
		m_pDebugTextureEffect->getVSBufferPointer(), m_pDebugTextureEffect->getVSBufferSize(), &pTmp.p));

	// @TODO: add implementation here
}

// Recursive function which traverses the model's node tree and draws the meshes at each node using the TexEffect.
void ModelsTestApp::drawModelTex(const DXModelNode &currNode, const DXMatrix &viewProj, std::vector<DXMatrix> &transformationStack)
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
		m_pTexEffect->setWorld(world);

		// Compute the current WVP matrix and set it.
		const DXMatrix wvp(world * viewProj);
		m_pTexEffect->setWVP(wvp);

		// Set the current diffuse and specular textures and apply the effect.
		const UINT uiMeshIndex(m_basicModels["Sponza"].pModel->getMaterialIndex(currMeshes[i]));
		const ShaderResourceViewPtr &pDiffuse(m_basicModels["Sponza"].pModel->getDiffuseTexture(uiMeshIndex));
		m_pTexEffect->setDiffuseMap(pDiffuse);
		const ShaderResourceViewPtr &pSpecular(m_basicModels["Sponza"].pModel->getSpecularTexture(uiMeshIndex));
		pSpecular.p ? m_pTexEffect->setSpecMap(pSpecular) : m_pSpecularMap;

		// Set the current diffuse and specular textures in the normal mapping effect.
		// Choose to apply either the texture mapping effect or the normal mapping effect
		// depending on whether the current material has a normal map or not.
		if (m_pNormalMappingEffectToggle->getState())
		{
			const ShaderResourceViewPtr &pNormal(m_basicModels["Sponza"].pModel->getNormalTexture(uiMeshIndex));
			if (pNormal.p)
			{
				md3dDeviceContext->IASetInputLayout(m_pNormalMappingEffect->getInputLayout());	// set the input layout of the normal mapping effect

				m_pNormalMappingEffect->setWorld(world);
				m_pNormalMappingEffect->setWorldInvTrans(world.Invert().Transpose());
				m_pNormalMappingEffect->setWVP(wvp);

				m_pNormalMappingEffect->setDiffuseMap(pDiffuse);
				pSpecular.p ? m_pNormalMappingEffect->setSpecularMap(pSpecular) : m_pSpecularMap;

				m_pNormalMappingEffect->setNormalMap(pNormal);
				m_pNormalMappingEffect->apply();
			}
			else
				m_pTexEffect->apply();
		}
		else
			m_pTexEffect->apply();

		// Draw the current mesh.
		m_basicModels["Sponza"].pModel->draw(currMeshes[i]);
	}

	// Draw the meshes of all the children of the current node.
	for (std::size_t i = 0; i < currNode.childNodes.size(); ++i)
		drawModelTex(currNode.childNodes[i], viewProj, transformationStack);

	// Pop the current node's transformation from the stack.
	transformationStack.pop_back();
}

// NOTE: These function were taken from Andrew Lauritzen's 2010 Deferred Shading demo: http://software.intel.com/en-us/articles/deferred-rendering-for-current-and-future-rendering-pipelines/
// Appending the license notice for reference.

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

// The original function is called InitializeLightParameters and was adapted for our purposes.
void ModelsTestApp::createPointLightsPattern(const UINT nMaxLights, const float fMaxRadius, const PointLightParams pointLightParams, std::vector<SLight> &pointLights)
{
	// Use a constant seed for consistency
	std::tr1::mt19937 rng(1337);

	std::tr1::uniform_real<float> radiusNormDist(0.0f, 1.0f);
	//const float fMaxRadius = 100.0f;
	std::tr1::uniform_real<float> angleDist(0.0f, 2.0f * PI);
	std::tr1::uniform_real<float> heightDist(50.0f, 450.0f);
	std::tr1::uniform_real<float> animationSpeedDist(60.0f, 80.0f);
	std::tr1::uniform_int<int> animationDirection(0, 1);
	std::tr1::uniform_real<float> hueDist(0.0f, 1.0f);
	std::tr1::uniform_real<float> intensityDist(0.1f, 0.5f);
	std::tr1::uniform_real<float> attenuationDist(2.0f, 15.0f);
	const float attenuationStartFactor = 0.8f;

	pointLights.resize(nMaxLights);
	for (unsigned int i = 0; i < nMaxLights; ++i) {
		
		pointLights[i].radius = std::sqrt(radiusNormDist(rng)) * fMaxRadius;
		pointLights[i].angle = angleDist(rng);
		const float height = heightDist(rng);

		// NOTE: Using 5*cos for x and 3*sin for z to get an elliptical pattern.
		pointLights[i].position = DXVector3(5.0f * std::cos(pointLights[i].angle) * fMaxRadius, height, 2.0f * std::sin(pointLights[i].angle) * fMaxRadius);
		// Normalize by arc length
		pointLights[i].animationSpeed = (animationDirection(rng) * 2 - 1) * animationSpeedDist(rng) / pointLights[i].radius;

		// HSL->RGB, vary light hue
		pointLights[i].ambient = pointLightParams.ambient;
		const DXVector3 diffuseColour(intensityDist(rng) * HueToRGB(hueDist(rng)));
		pointLights[i].diffuse = DXColor(diffuseColour.x, diffuseColour.y, diffuseColour.z, 1.0f);
		pointLights[i].specular = pointLights[i].diffuse; //pointLightParams.specular;
		pointLights[i].range = pointLightParams.range;
		pointLights[i].att = DXVector3(0.0f, 0.25 / attenuationDist(rng), 0.0f);	// @TODO: play with this to get an appropriate set of values
		/*params.attenuationEnd = attenuationDist(rng);
		params.attenuationBegin = attenuationStartFactor * params.attenuationEnd;*/
	}
}

// Simple function for getting bright colors...
// Hue in [0, 1)
DXVector3 ModelsTestApp::HueToRGB(const float hue)
{
	float intPart;
	float fracPart = modf(hue * 6.0f, &intPart);
	int region = static_cast<int>(intPart);

	switch (region) {
	case 0: return DXVector3(1.0f, fracPart, 0.0f);
	case 1: return DXVector3(1.0f - fracPart, 1.0f, 0.0f);
	case 2: return DXVector3(0.0f, 1.0f, fracPart);
	case 3: return DXVector3(0.0f, 1.0f - fracPart, 1.0f);
	case 4: return DXVector3(fracPart, 0.0f, 1.0f);
	case 5: return DXVector3(1.0f, 0.0f, 1.0f - fracPart);
	};

	return DXVector3(0.0f, 0.0f, 0.0f);
}

// The original function is called Move and was adapted for our purposes.
void ModelsTestApp::movePointLightsPattern(const float elapsedTime, const float fMaxRadius, const std::size_t nStart, const std::size_t nEnd, std::vector<SLight> &pointLights)
{
	// Update positions of active lights
	const float ellipseX(5.0f), ellipseZ(2.0f);	// parameteric form of an ellipse: (5t, 2t)
	for (unsigned int i = nStart; i <= nEnd; ++i) {
		//const PointLightInitTransform& initTransform = mLightInitialTransform[i];
		float angle = pointLights[i].angle + mTimer.getGameTime() * pointLights[i].animationSpeed;
		pointLights[i].position = DXVector3(
			pointLights[i].radius * ellipseX * std::cos(angle),
			pointLights[i].position.y,
			pointLights[i].radius * ellipseZ * std::sin(angle));
	}
}