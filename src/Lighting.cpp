//=============================================================================
// Lighting.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Demonstrates a directional, point, and spot light.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//              'Z'/'X' - Zoom
//          '1'/'2'/'2' - Switch light type
//
//=============================================================================

#include "d3dApp.h"
#include "PeaksAndValleys.h"
#include "Waves.h"
#include "Light.h"


class LightingApp : public D3DApp
{
public:
	LightingApp(HINSTANCE hInstance);
	~LightingApp();

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

	Light mLights[3];
	int mLightType; // 0 (parallel), 1 (point), 2 (spot)
	int mShadingType;	// 0 (Phong), 1 (Toon)
	int mShaderImplType;	// 0 (vertex shader), 1 (pixel shader)

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWVPVar;
	ID3DX11EffectMatrixVariable* mfxWorldVar;
	ID3DX11EffectVariable* mfxEyePosVar;
	ID3DX11EffectVariable* mfxLightVar;
	ID3DX11EffectScalarVariable* mfxLightType;
	ID3DX11EffectScalarVariable* mfxShadingType;
	ID3DX11EffectScalarVariable* mfxShaderImplType;

	ID3D11InputLayout* mVertexLayout;
 
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


	LightingApp theApp(hInstance);
	
	theApp.initApp();

	return theApp.run();
}

LightingApp::LightingApp(HINSTANCE hInstance)
: D3DApp(hInstance), mLightType(0), mShadingType(0), mShaderImplType(0), 
  mFX(0), mTech(0), mfxWVPVar(0), mfxWorldVar(0), 
  mfxEyePosVar(0), mfxLightVar(0), mfxLightType(0), mVertexLayout(0), 
  mEyePos(0.0f, 0.0f, 0.0f), mRadius(75.0f), mTheta(0.0f), mPhi(PI*0.4f)
{
	mLandWorld.Identity();
	mWavesWorld.Identity();
	mView.Identity();
	mProj.Identity();
	mWVP.Identity();

	// @TODO: remove when done testing
	//D3DXMatrixIdentity(&mLandWorld);
	//D3DXMatrixIdentity(&mWavesWorld);
	//D3DXMatrixIdentity(&mView);
	//D3DXMatrixIdentity(&mProj);
	//D3DXMatrixIdentity(&mWVP); 
}

LightingApp::~LightingApp()
{
	if( md3dDeviceContext )
		md3dDeviceContext->ClearState();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
}

void LightingApp::initApp()
{
	D3DApp::initApp();

	mClearColor = DXColor(0.9f, 0.9f, 0.9f, 1.0f);

	mLand.init(md3dDevice, 129, 129, 1.0f);
	mWaves.init(md3dDevice, 257, 257, 0.5f, 0.03f, 3.25f, 0.4f);

	buildFX();
	buildVertexLayouts();

	mLightType = 0;
 
	// Parallel light.
	mLights[0].dir      = DXVector3(0.57735f, -0.57735f, 0.57735f);
	mLights[0].ambient  = DXColor(0.0f, 0.0f, 0.0f, 1.0f);
	mLights[0].diffuse  = /*RED;*/ DXColor(1.0f, 1.0f, 1.0f, 1.0f);
	mLights[0].specular = DXColor(1.0f, 1.0f, 1.0f, 1.0f);
 
	// Pointlight--position is changed every frame to animate.
	mLights[1].ambient  = DXColor(0.4f, 0.4f, 0.4f, 1.0f);
	mLights[1].diffuse  = /*GREEN;*/ DXColor(1.0f, 1.0f, 1.0f, 1.0f);
	mLights[1].specular = DXColor(1.0f, 1.0f, 1.0f, 1.0f);
	mLights[1].att.x    = 0.0f;
	mLights[1].att.y    = 0.1f;
	mLights[1].att.z    = 0.0f;
	mLights[1].range    = 50.0f;

	// Spotlight--position and direction changed every frame to animate.
	mLights[2].ambient  = DXColor(0.4f, 0.4f, 0.4f, 1.0f);
	mLights[2].diffuse  = /*BLUE;*/ DXColor(1.0f, 1.0f, 1.0f, 1.0f);
	mLights[2].specular = DXColor(1.0f, 1.0f, 1.0f, 1.0f);
	mLights[2].att.x    = 1.0f;
	mLights[2].att.y    = 0.0f;
	mLights[2].att.z    = 0.0f;
	mLights[2].spotPow  = 64.0f;
	mLights[2].range    = 10000.0f;
}

void LightingApp::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	mProj = DirectX::XMMatrixPerspectiveFovLH(0.25f*PI, aspect, 1.0f, 1000.0f);
	//D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);	// @TODO: remove when done testing
}

void LightingApp::updateScene(float dt)
{
	D3DApp::updateScene(dt);

	// Every quarter second, generate a random wave.
	static float t_base = 0.0f;
	if( (mTimer.getGameTime() - t_base) >= 0.25f )
	{
		t_base += 0.25f;
 
		DWORD i = 5 + rand() % 250;
		DWORD j = 5 + rand() % 250;

		float r = RandF(1.0f, 2.0f);

		mWaves.disturb(i, j, r);
	}

	// Update angles based on input to orbit camera around scene.
	if(GetAsyncKeyState('A') & 0x8000)	mTheta -= 2.0f*dt;
	if(GetAsyncKeyState('D') & 0x8000)	mTheta += 2.0f*dt;
	if(GetAsyncKeyState('W') & 0x8000)	mPhi -= 2.0f*dt;
	if(GetAsyncKeyState('S') & 0x8000)	mPhi += 2.0f*dt;
	if(GetAsyncKeyState('Z') & 0x8000)	mRadius -= 25.0f*dt;
	if(GetAsyncKeyState('X') & 0x8000)	mRadius += 25.0f*dt;

	// Restrict the angle mPhi and radius mRadius.
	if( mPhi < 0.1f )	mPhi = 0.1f;
	if( mPhi > PI-0.1f)	mPhi = PI-0.1f;

	if( mRadius < 25.0f) mRadius = 25.0f;

	// set light type (1 = parallel, 2 = directional, 3 = spotlight)
	if(GetAsyncKeyState('1') & 0x8000)	mLightType = 0;
	if(GetAsyncKeyState('2') & 0x8000)	mLightType = 1;
	if(GetAsyncKeyState('3') & 0x8000)	mLightType = 2;

	// set shading type (5 = Phong, 6 = Toon, 7 = Strauss)
	if(GetAsyncKeyState('5') & 0x8000)	mShadingType = 0;
	if(GetAsyncKeyState('6') & 0x8000)	mShadingType = 1;
	if(GetAsyncKeyState('7') & 0x8000)	mShadingType = 2;

	// set shader implementation type (9 = vertex shader, 0 = pixel shader)
	if(GetAsyncKeyState('9') & 0x8000)	mShaderImplType = 0;
	if(GetAsyncKeyState('0') & 0x8000)	mShaderImplType = 1;

	// change the spot light radius (Q = increase, E = decrease)
	if(mLightType == 2)
	{
		if(GetAsyncKeyState('Q') & 0x8000 && mLights[2].spotPow <= 512.0f)
			mLights[2].spotPow++;
		else if(GetAsyncKeyState('E') & 0x8000 && mLights[2].spotPow >= 8.0f)
			mLights[2].spotPow--;
	}

	// Convert Spherical to Cartesian coordinates: mPhi measured from +y
	// and mTheta measured counterclockwise from -z.
	mEyePos.x =  mRadius*sinf(mPhi)*sinf(mTheta);
	mEyePos.z = -mRadius*sinf(mPhi)*cosf(mTheta);
	mEyePos.y =  mRadius*cosf(mPhi);

	// Build the view matrix.
	DXVector3 target(0.0f, 0.0f, 0.0f);
	DXVector3 up(0.0f, 1.0f, 0.0f);
	mView = DirectX::XMMatrixLookAtLH(mEyePos, target, up);
	//D3DXMatrixLookAtLH(&mView, &mEyePos, &target, &up);	// @TODO: remove when done testing


	// The point light circles the scene as a function of time, 
	// staying 7 units above the land's or water's surface.
	mLights[1].position.x = 50.0f*cosf( mTimer.getGameTime() );
	mLights[1].position.z = 50.0f*sinf( mTimer.getGameTime() );
	mLights[1].position.y = Max(mLand.getHeight(
		mLights[1].position.x, mLights[1].position.z), 0.0f) + 7.0f;


	// The spotlight takes on the camera position and is aimed in the
	// same direction the camera is looking.  In this way, it looks
	// like we are holding a flashlight.
	mLights[2].position = mEyePos;
	/*DXVector3 lightDir = target - mEyePos;
	lightDir.Normalize();*/
	mLights[2].dir = DirectX::XMVector3Normalize(DXVector3(target - mEyePos));
	//D3DXVec3Normalize(&mLights[2].dir, &(target-mEyePos));

	mWaves.update(dt);
}

void LightingApp::drawScene()
{
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
	mfxLightVar->SetRawValue(&mLights[mLightType], 0, sizeof(Light));
	mfxLightType->SetInt(mLightType);
	mfxShadingType->SetInt(mShadingType);
	mfxShaderImplType->SetInt(mShaderImplType);

    D3DX11_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );

    for(UINT i = 0; i < techDesc.Passes; ++i)
    {
        ID3DX11EffectPass* pass = mTech->GetPassByIndex(i);

		mWVP = mLandWorld*mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		mfxWorldVar->SetMatrix((float*)&mLandWorld);
		pass->Apply(0, md3dDeviceContext);
		mLand.draw();
	
		mWVP = mWavesWorld*mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		mfxWorldVar->SetMatrix((float*)&mWavesWorld);
		pass->Apply(0, md3dDeviceContext);
		mWaves.draw();
    }

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	md3dDeviceContext->RSSetState(0);
	mSpriteBatch->Begin();
	mFont->DrawString(mSpriteBatch.get(), mFrameStats.c_str(), DirectX::XMFLOAT2(0, 0));
	mSpriteBatch->End();
	//mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);	// @TODO: remove when done testing

	mSwapChain->Present(0, 0);
}

void LightingApp::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
 
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX11CompileEffectFromFile(L"lighting.fx", 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, shaderFlags, 0, md3dDevice, &mFX, &compilationErrors);

	// @TODO: remove when done testing
	//hr = D3DX10CreateEffectFromFile(L"lighting.fx", 0, 0, 
	//	"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILEW__, (DWORD)__LINE__, hr, L"D3DX11CompileEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("LightTech");
	
	mfxWVPVar    = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVar  = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxEyePosVar = mFX->GetVariableByName("gEyePosW");
	mfxLightVar  = mFX->GetVariableByName("gLight");
	mfxLightType = mFX->GetVariableByName("gLightType")->AsScalar();
	mfxShadingType = mFX->GetVariableByName("gShadingType")->AsScalar();
	mfxShaderImplType = mFX->GetVariableByName("gShaderImplType")->AsScalar();
}

void LightingApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"DIFFUSE",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3DX11_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 4, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}
