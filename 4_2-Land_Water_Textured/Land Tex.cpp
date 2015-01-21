//=============================================================================
// Land Tex.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Demonstrates how to texture a surface, and texture animation.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//              'Z'/'X' - Zoom
//
//=============================================================================

#include "d3dApp.h"
#include "PeaksAndValleys.h"
#include "Waves.h"
#include "Light.h"
#include "DXCamera.h"

extern "C"	{
#include "time.h"
}

// @TODO: remove when done testing
//#ifdef _DEBUG
//#pragma comment(lib, "DXEngineD.lib")
//#else
//#pragma comment(lib, "DXEngine.lib")
//#endif	// _DEBUG

class LandTexApp : public D3DApp
{
public:
	LandTexApp(HINSTANCE hInstance);
	~LandTexApp();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 

private:
	void buildFX();
	void buildVertexLayouts();

private:

	PeaksAndValleys mLand;
	Waves mWaves;

	Light mParallelLight;

	// translate water tex-coords.
	DXVector2 mWaterTexOffset;
 
	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3D11ShaderResourceView* mGrassMapRV;
	ID3D11ShaderResourceView* mWaterMapRV;
	ID3D11ShaderResourceView* mDefaultSpecMapRV;

	ID3DX11EffectMatrixVariable* mfxWVPVar;
	ID3DX11EffectMatrixVariable* mfxWorldVar;
	ID3DX11EffectVariable* mfxEyePosVar;
	ID3DX11EffectVariable* mfxLightVar;
	ID3DX11EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3DX11EffectShaderResourceVariable* mfxSpecMapVar;
	ID3DX11EffectMatrixVariable* mfxTexMtxVar;

	ID3D11InputLayout* mVertexLayout;

	DXCameraPtr mCamera;
 
	DXMatrix mLandWorld;
	DXMatrix mWavesWorld;

	DXMatrix mView;
	DXMatrix mProj;
	DXMatrix mWVP;

	DXVector3 mEyePos;
	float mRadius;
	float mTheta;
	float mPhi;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	LandTexApp theApp(hInstance);
	
	theApp.initApp();

	return theApp.run();
}

LandTexApp::LandTexApp(HINSTANCE hInstance)
: D3DApp(hInstance), mFX(0), mTech(0), mfxWVPVar(0), mfxWorldVar(0), mfxEyePosVar(0),
  mfxLightVar(0), mfxDiffuseMapVar(0), mfxSpecMapVar(0), mfxTexMtxVar(0), 
  mVertexLayout(0), mGrassMapRV(0), mWaterMapRV(0), mDefaultSpecMapRV(0), 
  mEyePos(0.0f, 0.0f, -50.0f), mRadius(75.0f), mTheta(0.0f), mPhi(PI*0.4f)
{
	// init the camera
	DXVector3 lookAt(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f);
	float FOVAngle = 0.25f * PI;
	float nearPlane = 1.0f, farPlane = 1000.0f;
	float aspectRatio = (float)mClientWidth / (float)mClientHeight;
	mCamera.reset(new DXCamera(mEyePos, lookAt, up, FOVAngle, aspectRatio, nearPlane, farPlane));

	mLandWorld.Identity();
	mWavesWorld.Identity();
	mView.Identity();
	mProj.Identity();
	mWVP.Identity();

	// @TODO: remove when done testing
	/*D3DXMatrixIdentity(&mLandWorld);
	D3DXMatrixIdentity(&mWavesWorld);
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); */
}

LandTexApp::~LandTexApp()
{
	if (md3dDeviceContext)
		md3dDeviceContext->ClearState();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
	ReleaseCOM(mGrassMapRV);
	ReleaseCOM(mWaterMapRV);
	ReleaseCOM(mDefaultSpecMapRV);
}

void LandTexApp::initApp()
{
	D3DApp::initApp();

	mClearColor = DXColor(0.9f, 0.9f, 0.9f, 1.0f);

	mLand.init(md3dDevice, 129, 129, 1.0f);

	// No wave damping.
	mWaves.init(md3dDevice, 257, 257, 0.5f, 0.03f, 3.25f, 0.0f);

	// Generate some waves at start up.
	for(int k = 0; k < 30; ++k)
	{ 
		DWORD i = 5 + rand() % 250;
		DWORD j = 5 + rand() % 250;

		float r = RandF(0.5f, 1.25f);

		mWaves.disturb(i, j, r);
	}

	
	buildFX();
	buildVertexLayouts();

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice, 
		L"grass.dds", 0, &mGrassMapRV ));

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice, 
		L"water2.dds", 0, &mWaterMapRV ));

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice, 
		L"defaultspec.dds", 0, &mDefaultSpecMapRV ));

	mWaterTexOffset = DXVector2(0.0f, 0.0f);

	mParallelLight.dir      = DXVector3(0.57735f, -0.57735f, 0.57735f);
	mParallelLight.ambient  = DXColor(0.2f, 0.2f, 0.2f, 1.0f);
	mParallelLight.diffuse  = DXColor(1.0f, 1.0f, 1.0f, 1.0f);
	mParallelLight.specular = DXColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void LandTexApp::onResize()
{
	D3DApp::onResize();

	// update the aspect ratio of the camera
	mCamera->setAspectRatio((float)mClientWidth / (float)mClientHeight);
	mProj = mCamera->proj();

	/*float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);*/
}

void LandTexApp::updateScene(float dt)
{
	D3DApp::updateScene(dt);

	// define keyboard input to rotate camera and to zoom in/out
	const float motionScaleFactor = 50.0f;
	const float zoomFactor = 1.0f;

	// move/rotate the camera according to the keyboard input
	// @TODO: update to work with the mouse
	if(GetAsyncKeyState('A') & 0x8000)	mCamera->slide(-motionScaleFactor * dt, 0, 0);	// move left	
	if(GetAsyncKeyState('D') & 0x8000)	mCamera->slide(motionScaleFactor * dt, 0, 0);	// move right
	if(GetAsyncKeyState('W') & 0x8000)	mCamera->slide(0, 0, motionScaleFactor * dt);		// move forward
	if(GetAsyncKeyState('S') & 0x8000)	mCamera->slide(0, 0, -motionScaleFactor * dt);		// move backward
	if(GetAsyncKeyState('Q') & 0x8000)	mCamera->slide(0, motionScaleFactor * dt, 0);			// move up
	if(GetAsyncKeyState('E') & 0x8000)	mCamera->slide(0, -motionScaleFactor * dt, 0);			// move down

	if(GetAsyncKeyState('J') & 0x8000)	mCamera->yaw(-zoomFactor * dt);	// yaw left
	if(GetAsyncKeyState('L') & 0x8000)	mCamera->yaw(zoomFactor * dt);	// yaw right
	if(GetAsyncKeyState('I') & 0x8000)	mCamera->pitch(-zoomFactor * dt);		// pitch up
	if(GetAsyncKeyState('K') & 0x8000)	mCamera->pitch(zoomFactor * dt);		// pitch down
	if(GetAsyncKeyState('U') & 0x8000)	mCamera->roll(-zoomFactor * dt);			// roll left
	if(GetAsyncKeyState('O') & 0x8000)	mCamera->roll(zoomFactor * dt);			// roll right

	// update mView
	mView = mCamera->view();

	//// Update angles based on input to orbit camera around scene.
	//if(GetAsyncKeyState('A') & 0x8000)	mTheta -= 2.0f*dt;
	//if(GetAsyncKeyState('D') & 0x8000)	mTheta += 2.0f*dt;
	//if(GetAsyncKeyState('W') & 0x8000)	mPhi -= 2.0f*dt;
	//if(GetAsyncKeyState('S') & 0x8000)	mPhi += 2.0f*dt;
	//if(GetAsyncKeyState('Z') & 0x8000)	mRadius -= 15.0f*dt;
	//if(GetAsyncKeyState('X') & 0x8000)	mRadius += 15.0f*dt;

	//// Restrict the angle mPhi.
	//if( mPhi < 0.1f )	mPhi = 0.1f;
	//if( mPhi > PI-0.1f)	mPhi = PI-0.1f;

	//// Convert Spherical to Cartesian coordinates: mPhi measured from +y
	//// and mTheta measured counterclockwise from -z.
	//mEyePos.x =  mRadius*sinf(mPhi)*sinf(mTheta);
	//mEyePos.z = -mRadius*sinf(mPhi)*cosf(mTheta);
	//mEyePos.y =  mRadius*cosf(mPhi);

	//// Build the view matrix.
	//DXVector3 target(0.0f, 0.0f, 0.0f);
	//DXVector3 up(0.0f, 1.0f, 0.0f);
	//D3DXMatrixLookAtLH(&mView, &mEyePos, &target, &up);

	// Animate water texture as a function of time.
	mWaterTexOffset.y += 0.1f*dt;
	mWaterTexOffset.x = 0.25f*sinf(4.0f*mWaterTexOffset.y); 

	__int64 startTime, endTime, frequency;
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	const float msecPerCount = 1000.0f / (float)frequency;

	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	mWaves.update(dt);

	QueryPerformanceCounter((LARGE_INTEGER*)&endTime);
	mUpdateTime += static_cast<float>(endTime - startTime) * msecPerCount;
	++mUpdateCounter;
}

void LandTexApp::drawScene()
{
	clock_t end, start = clock();

	D3DApp::drawScene();
	
	
	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDeviceContext->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDeviceContext->OMSetBlendState(0, blendFactors, 0xffffffff);
    md3dDeviceContext->IASetInputLayout(mVertexLayout);
    md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set per frame constants.
	mfxEyePosVar->SetRawValue(&mEyePos, 0, sizeof(DXVector3));
	mfxLightVar->SetRawValue(&mParallelLight, 0, sizeof(Light));

	// Scale texture coordinates by 5 units to map [0,1]-->[0,5]
	// so that the texture repeats five times in each direction.
	DXMatrix S(DirectX::XMMatrixScaling(5.0f, 5.0f, 1.0f));
	//D3DXMatrixScaling(&S, 5.0f, 5.0f, 1.0f);	// @TODO: remove when done testing
	DXMatrix landTexMtx = S;
	
	// Scale and translate the texture.
	DXMatrix T(DirectX::XMMatrixScaling(mWaterTexOffset.x, mWaterTexOffset.y, 0.0f));
	//D3DXMatrixTranslation(&T, mWaterTexOffset.x, mWaterTexOffset.y, 0.0f);	// @TODO: remove when done testing
	DXMatrix waterTexMtx = S*T;


    D3DX11_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );

    for(UINT i = 0; i < techDesc.Passes; ++i)
    {
        ID3DX11EffectPass* pass = mTech->GetPassByIndex(i);

		mWVP = mLandWorld*mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		mfxWorldVar->SetMatrix((float*)&mLandWorld);
		mfxTexMtxVar->SetMatrix((float*)&landTexMtx);
		mfxDiffuseMapVar->SetResource(mGrassMapRV);
		mfxSpecMapVar->SetResource(mDefaultSpecMapRV);
		pass->Apply(0, md3dDeviceContext);
		mLand.draw();
	
		mWVP = mWavesWorld*mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		mfxWorldVar->SetMatrix((float*)&mWavesWorld);
		mfxTexMtxVar->SetMatrix((float*)&waterTexMtx);
		mfxDiffuseMapVar->SetResource(mWaterMapRV);
		mfxSpecMapVar->SetResource(mDefaultSpecMapRV);
		pass->Apply(0, md3dDeviceContext);
		mWaves.draw(); 
    }

	end = clock();
	mDrawTime += (end-start);
	++mDrawCounter;

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	md3dDeviceContext->RSSetState(0);
	mSpriteBatch->Begin();
	mFont->DrawString(mSpriteBatch.get(), mFrameStats.c_str(), DirectX::XMFLOAT2(0, 0));
	mSpriteBatch->End();
	//mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);

	mSwapChain->Present(0, 0);
}

void LandTexApp::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
 
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX11CompileEffectFromFile(L"tex.fx", 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, shaderFlags, 0, md3dDevice, &mFX, &compilationErrors);
	// @TODO: remove when done testing
	/*hr = D3DX10CreateEffectFromFile(L"tex.fx", 0, 0, 
		"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);*/
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILEW__, (DWORD)__LINE__, hr, L"D3DX11CompileEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("TexTech");
	
	mfxWVPVar        = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVar      = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxEyePosVar     = mFX->GetVariableByName("gEyePosW");
	mfxLightVar      = mFX->GetVariableByName("gLight");
	mfxDiffuseMapVar = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mfxSpecMapVar    = mFX->GetVariableByName("gSpecMap")->AsShaderResource();
	mfxTexMtxVar     = mFX->GetVariableByName("gTexMtx")->AsMatrix();
}

void LandTexApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// Create the input layout
    D3DX11_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 3, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}
