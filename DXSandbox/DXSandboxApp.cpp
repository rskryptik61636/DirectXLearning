// Source file for sandbox app for experimenting with different effects

#include "DXSandboxApp.h"

#include <ScreenGrab.h>

#include <DXShaderUtils.h>

// Parameterized ctor, initializes the DXSandboxApp
DXSandboxApp::DXSandboxApp(HINSTANCE hInstance, const std::string strSceneFilePath)
: DXApp(hInstance, strSceneFilePath), m_nMaxParticles(512 * 5), m_pRainToggler(new ToggleState(L"Rain", 'R'))
{

}

// Dtor
DXSandboxApp::~DXSandboxApp()
{

}

// Called when Windows events have to be processed and there is something that the application might have to take care off
// @param: dt = delta time interval of the mGameTimer member
void DXSandboxApp::updateScene(float dt)
{
	DXApp::updateScene(dt);	// do everything that's being done in the base class version

	// Set the shadow normal mapping effect's current technique
	if (GetAsyncKeyState('1') & 0x8000)	m_pShadowNormalMappingEffect->setCurrentTechnique(ShadowNormalMappingEffect::ET_SIMPLE);
	if (GetAsyncKeyState('2') & 0x8000)	m_pShadowNormalMappingEffect->setCurrentTechnique(ShadowNormalMappingEffect::ET_PCF_1_TAP);
	if (GetAsyncKeyState('3') & 0x8000) m_pShadowNormalMappingEffect->setCurrentTechnique(ShadowNormalMappingEffect::ET_PCF_16_TAP);

	// update the position of the spot light to circle the scene as a function of the current time
	if (m_spotLights.size() > 0)
	{
		const float speedFactor(5.0f), spinRadius(150.0f), height(-100.0f);

		for (std::size_t i = 0; i < m_spotLights.size(); ++i)
		{
			//m_spotLights[i].position = DXVector3(0.0f, -150.0f, -150.0f);
			//m_spotLights[i].position = DXVector3(spinRadius * cosf(mTimer.getGameTime()*speedFactor), height, spinRadius * sinf(mTimer.getGameTime()*speedFactor));

			// update the direction of the spot light to point straight down
			//m_spotLights[0].dir = DXVector3(0, -1, 0);
			const DXVector3 lookAt(0.0f, -225.0f, 0.0f);
			m_spotLights[i].dir = DXVector3(lookAt - m_spotLights[i].position);
			m_spotLights[i].dir.Normalize();
		}
	}

	// Set the particle system effect's time step iff required
	if (m_strSceneName == "ParticleSystemTest")
	{
		m_particleSystems["Rain"].pSystem->setTimeStep(dt);

		// Toggle the rain state.
		m_pRainToggler->toggleState();
	}
}

// Define resources such as textures, blend/rasterizer/depth-stencil states and so on
void DXSandboxApp::createResources()
{
	// load all texture and normal maps and the default specular light map
	const wpath textureRoot(m_pSceneBuilder->getTextureRootW());
	const wpath brickTexturePath = textureRoot / wpath(L"bricks.dds");
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice, brickTexturePath.file_string().c_str(), NULL, &m_pBrickTexture.p));
	
	const wpath brickNormalMapPath = textureRoot / wpath(L"bricks_nmap.dds");
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice, brickNormalMapPath.file_string().c_str(), NULL, &m_pBrickNormalMap.p));

	const wpath floorTexturePath = textureRoot / wpath(L"floor.dds");
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice, floorTexturePath.file_string().c_str(), NULL, &m_pFloorTexture.p));

	const wpath floorNormalMapPath = textureRoot / wpath(L"floor_nmap.dds");
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice, floorNormalMapPath.file_string().c_str(), NULL, &m_pFloorNormalMap.p));

	const wpath specularMapPath = textureRoot / wpath(L"defaultspec.dds");
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice, specularMapPath.file_string().c_str(), NULL, &m_pSpecularMap.p));

	const wpath raindropTexturePath = textureRoot / wpath(L"raindrop.dds");
	HR(loadTexture(md3dDevice, md3dDeviceContext, raindropTexturePath, &m_raindropTexture.pResource.p, &m_raindropTexture.pResourceView.p));

	// init the scene lights
	initLights();

	// init the render states
	const INT iDepthBias(0);
	const FLOAT fDepthBiasClamp(0.0f), fSlopedScaledDepthBias(0.0f);
	RenderStateMaker renderStateMaker;
	renderStateMaker.createCounterClockwiseFrontFacingRS(md3dDevice, m_pCounterClockwiseFrontFacing/*, iDepthBias, fDepthBiasClamp, fSlopedScaledDepthBias*/);
	renderStateMaker.createShadowMapRS(md3dDevice, m_pShadowMapRS/*, iDepthBias, fDepthBiasClamp, fSlopedScaledDepthBias*/);
	renderStateMaker.createDepthTestingDSS(md3dDevice, m_pDepthTestingDSS);
}

// Define scene objects which need to be rendered
void DXSandboxApp::createObjects()
{
	// init the room
	m_pRoom.reset(new TangentBox());
	m_pRoom->initBox(md3dDevice, 1.0f, true);	// specify that the normals need to be flipped

	// init the ordinary box
	m_pBox1.reset(new TangentBox());
	m_pBox1->initBox(md3dDevice, 1.0f);	// no need to flip the normals

	// init the shadow map quad
	m_pShadowMapQuad.reset(new TangentQuad());
	m_pShadowMapQuad->init(md3dDevice, 1.0f);

	// init the shadow map helper
	//const UINT shadowMapSize(2048/*1024*/);
	m_pShadowMap.reset(new ShadowMap(md3dDevice, mClientWidth, mClientHeight, m_spotLights.size()));
}

// Renders the scene
void DXSandboxApp::drawScene()
{
	// Decide what to do based on the current scene
	if (m_strSceneName == "NormalMapTest")
	{
		DXApp::drawScene();
	}
	else if (m_strSceneName == "ShadowMapTest")
	{
#if 0
		// render the scene from the perspective of the camera and use the shadow map dummy pixel shader to render depth
		{
			// clear the depth and render targets
			D3DApp::drawScene();

			// init the input layout and the primitive topology
			md3dDeviceContext->IASetInputLayout(m_pVertexLayout.p);
			md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// determine the view projection matrix from the perspective of the light we're using to generate the shadow map
			const Light &shadowLight = m_spotLights[0];
			const DXVector3 eyePos(shadowLight.position), lookAt(0.0f, -225.0f, 0.0f), up(0.0f, 1.0f, 0.0f);
			const float fov(45.0f), aspectRatio(1.0f/*static_cast<float>(mClientWidth) / static_cast<float>(mClientHeight)*/),
				nearPlane(1.0f), farPlane(shadowLight.range);
			const DXMatrix viewProj(m_pCamera->view() * m_pCamera->proj());	// just to test
			/*const DXMatrix viewProj(DXMatrix::CreateLookAt(eyePos, lookAt, up) * 
			DXMatrix::CreatePerspectiveFieldOfView(fov, aspectRatio, nearPlane, farPlane));*/

			// init the world, texture and world view projection matrices
			const float roomScaleFactor(500.0f), roomTexScaleFactor(4.0f);
			//const float roomScaleFactor(1.0f), roomTexScaleFactor(4.0f);	// just to test
			const DXMatrix roomWorld(DXMatrix::CreateScale(roomScaleFactor));
			DXMatrix world(roomWorld), wvp(roomWorld * viewProj)/*, texMtx(DXMatrix::CreateScale(roomTexScaleFactor))*/;

			// set the ShadowNormalMappingEffect and apply it
			m_pShadowNormalMappingEffect->setCurrentPass(ShadowNormalMappingEffect::EP_SHADOW_MAP_GENERATION);
			m_pShadowNormalMappingEffect->setWVP(wvp);
			m_pShadowNormalMappingEffect->apply();
			
			// set the CCW front facing rasterizer state
			md3dDeviceContext->RSSetState(m_pCounterClockwiseFrontFacing.p);

			// draw the room
			m_pRoom->draw();

			// reset the rasterizer state
			md3dDeviceContext->RSSetState(0);

			// draw the ordinary box at the origin
			world = DXMatrix::CreateScale(roomScaleFactor / 10) * DXMatrix::CreateTranslation(0.0f, -225.0f, 0.0f);
			//world = DXMatrix::Identity();
			wvp = world * viewProj;
			m_pShadowNormalMappingEffect->setWVP(wvp);
			m_pShadowNormalMappingEffect->apply();

			// set the shadow map rasterizer state
			md3dDeviceContext->RSSetState(m_pShadowMapRS);

			// draw the box
			m_pBox1->draw();

			// reset the rasterizer state
			md3dDeviceContext->RSSetState(0);

			// swap the front and back buffers
			mSwapChain->Present(0, 0);
		}
#endif	// 0

#if 1
		//m_pShadowMap->BindDsvAndSetNullRenderTarget(md3dDeviceContext);	// bind the shadow map's depth stencil view and set the render target to NULL
		//																// this is done to construct the depth map from the light's perspective

		// Clear the depth stencil view in the shadow map
		m_pShadowMap->ClearDepthStencilBuffer(md3dDeviceContext);

		// render the scene objects and generate the shadow map
		for (std::size_t i = 0; i < m_spotLights.size(); ++i)
		{
			// Bind the depth stencil view of the i'th shadow map
			m_pShadowMap->BindSubDsvAndSetNullRenderTarget(md3dDeviceContext, i);

			// reset the depth and blend states because the debug font drawing changes it
			md3dDeviceContext->OMSetDepthStencilState(m_pDepthTestingDSS.p, 0);
			//md3dDeviceContext->OMSetDepthStencilState(0, 0);
			const float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			const UINT sampleMask = 0xffffffff;
			md3dDeviceContext->OMSetBlendState(0, blendFactors, sampleMask);

			// init the input layout and the primitive topology
			md3dDeviceContext->IASetInputLayout(m_pVertexLayout.p);
			md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// determine the view projection matrix from the perspective of the light we're using to generate the shadow map
			const Light &shadowLight = m_spotLights[i];
			const DXVector3 eyePos(shadowLight.position), lookAt(0.0f, -225.0f, 0.0f), up(0.0f, 1.0f, 0.0f);
			const float fov(45.0f), aspectRatio(/*1.0f*/ static_cast<float>(m_pShadowMap->width()) / static_cast<float>(m_pShadowMap->height())),
						nearPlane(100.0f), farPlane(shadowLight.range);
			//const DXMatrix viewProj(m_pCamera->view() * m_pCamera->proj());	// just to test
			const DXMatrix viewProj(DXMatrix::CreateLookAt(eyePos, lookAt, up) * 
									DXMatrix::CreatePerspectiveFieldOfView(fov, aspectRatio, nearPlane, farPlane));

			// init the world, texture and world view projection matrices
			const float roomScaleFactor(500.0f), roomTexScaleFactor(4.0f);
			//const float roomScaleFactor(1.0f), roomTexScaleFactor(4.0f);	// just to test
			const DXMatrix roomWorld(DXMatrix::CreateScale(roomScaleFactor));
			DXMatrix world(roomWorld), wvp(roomWorld * viewProj)/*, texMtx(DXMatrix::CreateScale(roomTexScaleFactor))*/;

			// set the ShadowNormalMappingEffect and apply it
			m_pShadowNormalMappingEffect->setCurrentPass(ShadowNormalMappingEffect::EP_SHADOW_MAP_GENERATION);
			m_pShadowNormalMappingEffect->setFirstPassLightWVP(wvp);
			m_pShadowNormalMappingEffect->apply();
			
			// set the CCW front facing rasterizer state
			md3dDeviceContext->RSSetState(m_pCounterClockwiseFrontFacing.p);

			// draw the room
			m_pRoom->draw();

			// reset the rasterizer state
			md3dDeviceContext->RSSetState(0);

			// draw the ordinary box at the origin
			world = DXMatrix::CreateScale(roomScaleFactor / 10) * DXMatrix::CreateTranslation(0.0f, -225.0f, 0.0f);
			//world = DXMatrix::Identity();
			wvp = world * viewProj;
			m_pShadowNormalMappingEffect->setFirstPassLightWVP(wvp);
			m_pShadowNormalMappingEffect->apply();

			// set the shadow map rasterizer state
			md3dDeviceContext->RSSetState(m_pShadowMapRS);

			// draw the box
			m_pBox1->draw();

			// reset the rasterizer state
			md3dDeviceContext->RSSetState(0);

			// reset the depth stencil state
			md3dDeviceContext->OMSetDepthStencilState(0, 0);
		}

		// restore the default viewport, depth stencil view and render targets
		md3dDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
		md3dDeviceContext->RSSetViewports(1, &mDefaultViewport);

		D3DApp::drawScene();	// invoke the base D3D class' drawScene method to clear the render target and depth stencil views

		// render the scene again with the second pass of the shadow mapping effect wherein the shadow will actually be rendered
		{
			// reset the depth and blend states because the debug font drawing changes it
			md3dDeviceContext->OMSetDepthStencilState(m_pDepthTestingDSS.p, 0);
			//md3dDeviceContext->OMSetDepthStencilState(0, 0);
			const float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			const UINT sampleMask = 0xffffffff;
			md3dDeviceContext->OMSetBlendState(0, blendFactors, sampleMask);

			// init the input layout and the primitive topology
			md3dDeviceContext->IASetInputLayout(m_pVertexLayout.p);
			md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// Transforms from [-1,+1] to [0,1]; -0.5 scale for the y value as in texture space, 
			// the direction of the y axis is inverted since it is in image coordinates
			const DXMatrix lightTangent(DXMatrix::CreateScale(0.5f, -0.5f, 1.0f) * DXMatrix::CreateTranslation(0.5f, 0.5f, 0.0f));
			/*const DXMatrix lightViewProjTangent(DXMatrix::CreateLookAt(eyePos, lookAt, up) *
			DXMatrix::CreatePerspectiveFieldOfView(fov, aspectRatio, nearPlane, farPlane) *
			lightTangent);*/

			// determine the world view projection tangent matrix from the perspective of each light
			std::vector<DXMatrix> lightViewProjTangent(m_spotLights.size()), worldLightViewProjTangent(m_spotLights.size());
			for (std::size_t i = 0; i < m_spotLights.size(); ++i)
			{
				const Light &shadowLight = m_spotLights[i];
				const DXVector3 eyePos(shadowLight.position), lookAt(0.0f, -225.0f, 0.0f), up(0.0f, 1.0f, 0.0f);
				const float fov(45.0f), aspectRatio(/*1.0f*/ static_cast<float>(m_pShadowMap->width()) / static_cast<float>(m_pShadowMap->height())),
					nearPlane(100.0f), farPlane(shadowLight.range);
				lightViewProjTangent[i] = DXMatrix::CreateLookAt(eyePos, lookAt, up) *
					DXMatrix::CreatePerspectiveFieldOfView(fov, aspectRatio, nearPlane, farPlane) *
					lightTangent;
			}

			// init the world, texture and world view projection matrices
			const float roomScaleFactor(500.0f), roomTexScaleFactor(4.0f);
			//const float roomScaleFactor(1.0f), roomTexScaleFactor(4.0f);	// just to test
			const DXMatrix roomWorld(DXMatrix::CreateScale(roomScaleFactor));
			const DXMatrix viewProj(m_pCamera->view() * m_pCamera->proj());	// camera view projection matrix
			DXMatrix texMtx(DXMatrix::CreateScale(roomTexScaleFactor));
			DXMatrix world(roomWorld), wvp(roomWorld * viewProj)/*, texMtx(DXMatrix::CreateScale(roomTexScaleFactor))*/;

			// populate the worldLightViewProjTangent matrix list for the current world transformation
			for (std::size_t i = 0; i < worldLightViewProjTangent.size(); ++i)
				worldLightViewProjTangent[i] = world * lightViewProjTangent[i];

			// set the second pass of the ShadowNormalMappingEffect and apply it
			m_pShadowNormalMappingEffect->setCurrentPass(ShadowNormalMappingEffect::EP_SHADOW_MAP_RENDER);
			m_pShadowNormalMappingEffect->setSecondPassLightWorldViewProjTangentMatrix(worldLightViewProjTangent);
			/*m_pShadowNormalMappingEffect->setSecondPassLightWorldViewProjTangentMatrix(world * lightViewProjTangent);*/
			m_pShadowNormalMappingEffect->setSecondPassWorld(world);
			m_pShadowNormalMappingEffect->setSecondPassWorldInvTrans(world.Invert().Transpose());
			m_pShadowNormalMappingEffect->setSecondPassWVP(wvp);
			m_pShadowNormalMappingEffect->setSecondPassTexMtx(texMtx);

			m_pShadowNormalMappingEffect->setSpotLights(m_spotLights);
			m_pShadowNormalMappingEffect->setEyePosW(m_pCamera->eyePosW());
			m_pShadowNormalMappingEffect->setDiffuseMap(m_pFloorTexture);
			m_pShadowNormalMappingEffect->setSpecularMap(m_pSpecularMap);
			m_pShadowNormalMappingEffect->setNormalMap(m_pFloorNormalMap);
			m_pShadowNormalMappingEffect->setShadowMap(m_pShadowMap->DepthMapSRV());
			m_pShadowNormalMappingEffect->setShadowMapSize(m_pShadowMap->width(), m_pShadowMap->height());
			m_pShadowNormalMappingEffect->apply();

			// set the CCW front facing rasterizer state
			md3dDeviceContext->RSSetState(m_pCounterClockwiseFrontFacing.p);

			// draw the room
			m_pRoom->draw();

			// reset the rasterizer state
			md3dDeviceContext->RSSetState(0);

			// draw the ordinary box at the origin
			world = DXMatrix::CreateScale(roomScaleFactor / 10) * DXMatrix::CreateTranslation(0.0f, -225.0f, 0.0f);
			//world = DXMatrix::Identity();
			wvp = world * viewProj;
			texMtx = DXMatrix::Identity();

			// populate the worldLightViewProjTangent matrix list for the current world transformation
			for (std::size_t i = 0; i < worldLightViewProjTangent.size(); ++i)
				worldLightViewProjTangent[i] = world * lightViewProjTangent[i];

			// set the second pass of the ShadowNormalMappingEffect and apply it
			/*m_pShadowNormalMappingEffect->setSecondPassLightWorldViewProjTangentMatrix(world * lightViewProjTangent);*/
			m_pShadowNormalMappingEffect->setSecondPassLightWorldViewProjTangentMatrix(worldLightViewProjTangent);
			m_pShadowNormalMappingEffect->setSecondPassWorld(world);
			m_pShadowNormalMappingEffect->setSecondPassWorldInvTrans(world.Invert().Transpose());
			m_pShadowNormalMappingEffect->setSecondPassWVP(wvp);
			m_pShadowNormalMappingEffect->setSecondPassTexMtx(texMtx);

			/*m_pShadowNormalMappingEffect->setSpotLights(m_spotLights);
			m_pShadowNormalMappingEffect->setEyePosW(m_pCamera->eyePosW());			
			m_pShadowNormalMappingEffect->setSpecularMap(m_pSpecularMap);			
			m_pShadowNormalMappingEffect->setShadowMap(m_pShadowMap->DepthMapSRV());*/
			m_pShadowNormalMappingEffect->setDiffuseMap(m_pBrickTexture);
			m_pShadowNormalMappingEffect->setNormalMap(m_pBrickNormalMap);
			m_pShadowNormalMappingEffect->apply();

			// set the shadow map rasterizer state
			md3dDeviceContext->RSSetState(m_pShadowMapRS);

			// draw the box
			m_pBox1->draw();

			// reset the rasterizer state
			md3dDeviceContext->RSSetState(0);

			// reset the depth stencil state
			md3dDeviceContext->OMSetDepthStencilState(0, 0);

			// unbind the shadow map SRV from the pixel shader
			m_pShadowNormalMappingEffect->releaseShadowMap();

			// draw the scene lights
			drawLightBoxes(viewProj);

			// draw the shadow map
			drawShadowMap();
		}

		//{
		//	// init the input layout and the primitive topology
		//	md3dDeviceContext->IASetInputLayout(m_pVertexLayout.p);
		//	md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//	// draw the shadow map
		//	drawShadowMap();
		//}

		//// render the scene as usual
		//drawObjects();

		// draw frame stats
		const std::wstring strInfo(mFrameStats + m_pShadowNormalMappingEffect->getEffectInfo());
		mSpriteBatch->Begin();
		mFont->DrawString(mSpriteBatch.get(), strInfo.c_str(), DirectX::XMFLOAT2(0, 0));
		mSpriteBatch->End();

		mSwapChain->Present(0, 0);	// swap the front and back buffers
#endif	// 0

	}
	else if (m_strSceneName == "ParticleSystemTest")
	{
		D3DApp::drawScene();

		// draw objects
		drawObjects();

		// set the dynamic params and run the rain particle system iff enabled
		if (m_pRainToggler->getState())
		{			
			m_particleSystems["Rain"].params.eyePosW = m_pCamera->eyePosW();
			m_particleSystems["Rain"].params.viewProj = m_pCamera->view() * m_pCamera->proj();
			m_particleSystems["Rain"].params.pParticleTexture = m_raindropTexture.pResourceView.p;
			m_particleSystems["Rain"].pSystem->apply(m_particleSystems["Rain"].params);
		}

		// draw frame stats
		mSpriteBatch->Begin();
		mFont->DrawString(mSpriteBatch.get(), mFrameStats.c_str(), DirectX::XMFLOAT2(0, 0));
		mSpriteBatch->End();

		// swap the front and back buffers
		mSwapChain->Present(0, 0);
	}
}

// Meat of the drawScene method, this is where the scene rendering is implemented
void DXSandboxApp::drawObjects()
{
	// reset the depth and blend states because the debug font drawing changes it
	md3dDeviceContext->OMSetDepthStencilState(0, 0);
	const float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	const UINT sampleMask = 0xffffffff;
	md3dDeviceContext->OMSetBlendState(0, blendFactors, sampleMask);

	// init the input layout and the primitive topology
	md3dDeviceContext->IASetInputLayout(m_pVertexLayout.p);
	md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// init the world, texture and world view projection matrices
	const float roomScaleFactor(500.0f), roomTexScaleFactor(4.0f);
	const DXMatrix viewProj(m_pCamera->view() * m_pCamera->proj());
	const DXMatrix roomWorld(DXMatrix::CreateScale(roomScaleFactor));
	DXMatrix world(roomWorld), wvp(roomWorld * viewProj), texMtx(DXMatrix::CreateScale(roomTexScaleFactor));

	// set the NormalMappingEffect and apply it
	m_pNormalMappingEffect->setWorld(world);
	m_pNormalMappingEffect->setWorldInvTrans(world.Invert().Transpose());
	m_pNormalMappingEffect->setWVP(wvp);
	m_pNormalMappingEffect->setTexMtx(texMtx);

	m_pNormalMappingEffect->setParallelLights(m_parallelLights);
	m_pNormalMappingEffect->setPointLights(m_pointLights);
	m_pNormalMappingEffect->setSpotLights(m_spotLights);
	m_pNormalMappingEffect->setEyePosW(m_pCamera->eyePosW());

	m_pNormalMappingEffect->setDiffuseMap(m_pFloorTexture);
	m_pNormalMappingEffect->setNormalMap(m_pFloorNormalMap);
	m_pNormalMappingEffect->setSpecularMap(m_pSpecularMap);

	m_pNormalMappingEffect->apply();

	// set the CCW front facing rasterizer state
	md3dDeviceContext->RSSetState(m_pCounterClockwiseFrontFacing.p);

	// draw the room
	m_pRoom->draw();

	// reset the rasterizer state
	md3dDeviceContext->RSSetState(0);

	// draw the ordinary box at the origin
	world = DXMatrix::CreateScale(roomScaleFactor/10) * DXMatrix::CreateTranslation(0.0f, -225.0f, 0.0f);
	wvp = world * viewProj;
	texMtx = DXMatrix::Identity();
	m_pNormalMappingEffect->setWorld(world);
	m_pNormalMappingEffect->setWorldInvTrans(world.Invert().Transpose());
	m_pNormalMappingEffect->setWVP(wvp);
	m_pNormalMappingEffect->setTexMtx(texMtx);
	m_pNormalMappingEffect->setDiffuseMap(m_pBrickTexture);
	m_pNormalMappingEffect->setNormalMap(m_pBrickNormalMap);
	m_pNormalMappingEffect->apply();
	m_pBox1->draw();

	// draw the scene lights
	drawLightBoxes(viewProj);

	// @TODO: we probably don't need this because drawObjects() is not invoked when the shadow map test is running
#if 0
	// draw the shadow map
	drawShadowMap();
#endif // 0

}

// Define to build the shaders which will be used by the application
void DXSandboxApp::buildShaders()
{
	// init the NormalMappingEffect
	const wpath shaderRoot(m_pSceneBuilder->getShaderRootW());
	m_pNormalMappingEffect.reset(new NormalMappingEffect(shaderRoot, md3dDevice, md3dDeviceContext));
	m_pNormalMappingEffect->init();

	// init the ColorEffect
	m_pColorEffect.reset(new ColorEffect(shaderRoot, md3dDevice, md3dDeviceContext));
	m_pColorEffect->init();

	// init the DebugTextureEffect
	m_pDebugTextureEffect.reset(new DebugTextureEffect(shaderRoot, md3dDevice, md3dDeviceContext));
	m_pDebugTextureEffect->init();

	// init the ShadowNormalMappingEffect
	m_pShadowNormalMappingEffect.reset(new ShadowNormalMappingEffect(shaderRoot, md3dDevice, md3dDeviceContext));
	m_pShadowNormalMappingEffect->init();

	// init the particle system directory
	m_pSceneBuilder->buildParticleSystems(shaderRoot, md3dDevice, md3dDeviceContext, m_particleSystems);
		
	// @TODO: add implementation here
}

// Define to build the vertex layout which will be bound to the input assembly stage
void DXSandboxApp::buildVertexLayouts()
{
	// get the input layout from the NormalMappingEffect as that corresponds to TangentVertex
	// which is the primary VertexType that we'll be using
	m_pVertexLayout = m_pNormalMappingEffect->getInputLayout();

	// @TODO: add implementation here
}

#if 0
// Initializes the camera using the given params (TODO: deprecate when we implement the scene file functionality)
void DXSandboxApp::initCamera()
{
	const DXVector3 eyePos = DXVector3(0.0f, -200.0f, -100.0f);
	const DXVector3 lookAt = DXVector3(0.0f, -200.0f, 0.0f);
	const DXVector3 up = DXVector3(0.0f, 1.0f, 0.0f);
	const float FOVAngle = 0.25f * PI;
	const float nearPlane = 1.0f, farPlane = 1000.0f;
	const float aspectRatio = (float)mClientWidth / (float)mClientHeight;
	m_pCamera.reset(new DXCamera(eyePos, lookAt, up, FOVAngle, aspectRatio, nearPlane, farPlane));
}
#endif	// 0

// Initializes the scene lights
void DXSandboxApp::initLights()
{
	// init the point and spot lights from the scene builder
	// @TODO: remove when done testing
#if 0
	// init the parallel lights in the scene
	Light parallelLight;
	parallelLight.dir = DXVector3(0.5f, -0.5f, 0.5f);
	parallelLight.ambient = DXColor(0.4f, 0.4f, 0.4f, 1.0f);
	parallelLight.diffuse = WHITE;
	parallelLight.specular = WHITE;
	//m_parallelLights.push_back(parallelLight);

	// init the point lights in the scene
	Light pointLight;
	pointLight.position = DXVector3(0.0f, 225.0f, 0.0f);	// ceiling top
	pointLight.ambient = DXColor(0.4f, 0.4f, 0.4f, 1.0f);
	pointLight.diffuse = WHITE;
	pointLight.specular = WHITE;
	pointLight.att = DXVector3(2.0f, 0.0f, 0.0f);
	pointLight.range = 5000.0f;
	//m_pointLights.push_back(pointLight);	//

	pointLight.position = DXVector3(-225.0f, 0.0f, 0.0f);	// left wall
	m_pointLights.push_back(pointLight);

	pointLight.position = DXVector3(225.0f, 0.0f, 0.0f);	// right wall
	m_pointLights.push_back(pointLight);

	pointLight.position = DXVector3(0.0f, 0.0f, 225.0f);	// back wall
	m_pointLights.push_back(pointLight);

	pointLight.position = DXVector3(0.0f, 0.0f, -225.0f);	// front wall
	m_pointLights.push_back(pointLight);

	// init the spot lights in the scene
	Light spotLight;
	spotLight.ambient = /*RED;*/ DXColor(0.4f, 0.4f, 0.4f, 1.0f);
	spotLight.diffuse = BLUE; /*GREEN;*/
	spotLight.specular = BLUE;
	spotLight.att = DXVector3(1.0f, 0.0f, 0.0f);
	spotLight.spotPow = 64.0f;
	spotLight.range = 10000.0f;
	m_spotLights.push_back(spotLight);
#endif	// 0

	// init the default parallel light params
	ParallelLightParams parallelLightParams;
	parallelLightParams.dir = DXVector3(0.5f, -0.5f, 0.5f);
	parallelLightParams.ambient = DXColor(0.4f, 0.4f, 0.4f, 1.0f);
	parallelLightParams.diffuse = WHITE;
	parallelLightParams.specular = WHITE;

	// init the scene's parallel lights
	m_pSceneBuilder->buildParallelLights(parallelLightParams, m_parallelLights);

	// init the default point light params
	PointLightParams pointLightParams;
	pointLightParams.position = DXVector3(0.0f, 225.0f, 0.0f);	// ceiling top
	pointLightParams.ambient = DXColor(0.4f, 0.4f, 0.4f, 1.0f);
	pointLightParams.diffuse = WHITE;
	pointLightParams.specular = WHITE;
	pointLightParams.att = DXVector3(2.0f, 0.0f, 0.0f);
	pointLightParams.range = 5000.0f;

	// init the scene's point lights
	m_pSceneBuilder->buildPointLights(pointLightParams, m_pointLights);

	// init the default spot light params
	SpotLightParams spotLightParams;
	spotLightParams.ambient = /*RED;*/ DXColor(0.4f, 0.4f, 0.4f, 1.0f);
	spotLightParams.diffuse = BLUE; /*GREEN;*/
	spotLightParams.specular = BLUE;
	spotLightParams.att = DXVector3(1.0f, 0.0f, 0.0f);
	spotLightParams.spotPow = 64.0f;
	spotLightParams.range = 10000.0f;

	// init the scene's spot lights
	m_pSceneBuilder->buildSpotLights(spotLightParams, m_spotLights);

	// init the positional boxes for the point lights in the scene
	m_pointLightBoxes.resize(m_pointLights.size());
	for (std::size_t i = 0; i < m_pointLightBoxes.size(); ++i)
	{
		m_pointLightBoxes[i].reset(new SimpleBox());
		m_pointLightBoxes[i]->initBox(md3dDevice, 1.0f, m_pointLights[i].diffuse);
	}

	// init the positional boxes for the spot lights in the scene
	m_spotLightBoxes.resize(m_spotLights.size());
	for (std::size_t i = 0; i < m_spotLightBoxes.size(); ++i)
	{
		m_spotLightBoxes[i].reset(new SimpleBox());
		m_spotLightBoxes[i]->initBox(md3dDevice, 1.0f, m_spotLights[i].diffuse);
	}
}

// Draws the boxes that indicate the positions of each of the scene lights
void DXSandboxApp::drawLightBoxes(const DXMatrix &viewProj)
{
	// set the input layout as that of the Color effect's
	md3dDeviceContext->IASetInputLayout(m_pColorEffect->getInputLayout().p);

	// scaling matrix for all the light boxes
	const DXMatrix scale(DXMatrix::CreateScale(5.0f));

	// draw all of the point light boxes
	for (std::size_t i = 0; i < m_pointLightBoxes.size(); ++i)
	{
		// determine the world and wvp matrices using the light's position, set the wvp matrix in the Color effect and apply it
		const DXMatrix world(scale * DXMatrix::CreateTranslation(m_pointLights[i].position)), wvp(world * viewProj);		
		m_pColorEffect->setWVP(wvp);
		m_pColorEffect->apply();

		m_pointLightBoxes[i]->draw();
	}

	// draw all of the spot light boxes
	for (std::size_t i = 0; i < m_spotLightBoxes.size(); ++i)
	{
		// determine the world and wvp matrices using the light's position, set the wvp matrix in the Color effect and apply it
		const DXMatrix world(scale * DXMatrix::CreateTranslation(m_spotLights[i].position)), wvp(world * viewProj);
		m_pColorEffect->setWVP(wvp);
		m_pColorEffect->apply();

		m_spotLightBoxes[i]->draw();
	}		
}

// Draws the shadow map to a debug window
void DXSandboxApp::drawShadowMap()
{
	// Set the debug window to be a quad on the bottom left corner of the screen
	const float quadScale(0.25f);
	const DXMatrix wvp(DXMatrix::CreateScale(quadScale) * DXMatrix::CreateTranslation(1.0f - quadScale, 1.0f - quadScale, 0.0f));
	//const DXMatrix wvp(DXMatrix::Identity());
	const DXMatrix texMtx(DXMatrix::Identity());

	// Setup the DebugTextureEffect and display the brick texture just to test
	m_pDebugTextureEffect->setWVP(wvp);
	m_pDebugTextureEffect->setTexMtx(texMtx);

	//m_pDebugTextureEffect->setDebugTexture(m_pBrickTexture);
	m_pDebugTextureEffect->setDebugTexture(m_pShadowMap->DepthMapSRV());

	m_pDebugTextureEffect->setTextureIndex(DebugTextureEffect::DC_RED);
	m_pDebugTextureEffect->apply();

	// Setup the input layout for the shadow map quad
	md3dDeviceContext->IASetInputLayout(m_pVertexLayout);

	// Draw the shadow map quad
	m_pShadowMapQuad->draw();

	// clear out the pixel shader resources just in case to ensure that the depth stencil buffer doesn't stay bound
	m_pDebugTextureEffect->releaseDebugTexture();

}