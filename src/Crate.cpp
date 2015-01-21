//=============================================================================
// Create.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Shows how to texture a box to make it look like a crate.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//              'Z'/'X' - Zoom
//
//=============================================================================

#include <sstream>

#include "d3dApp.h"
#include "Light.h"
#include "TextureBox.h"
//#include "Box.h"

// @TODO: remove when done testing
//#ifdef _DEBUG
//	#pragma comment(lib, "DXEngined.lib")
//#else
//	#pragma comment(lib, "DXEngine.lib")
//#endif	// _DEBUG

class CrateApp : public D3DApp
{
public:
	CrateApp(HINSTANCE hInstance);
	~CrateApp();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 

private:
	void buildFX();
	void buildVertexLayouts();
 
private:

	TextureBox mCrateMesh;
	//Box mCrateMesh;

	Light mParallelLight;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3D11InputLayout* mVertexLayout;
	ID3D11ShaderResourceView* mDiffuseMapRV;
	ID3D11ShaderResourceView* mDiffuseMapRV2;
	ID3D11ShaderResourceView* mSpecMapRV;
	ID3D11ShaderResourceView* mFireMapRV[120];

	ID3DX11EffectMatrixVariable* mfxWVPVar;
	ID3DX11EffectMatrixVariable* mfxWorldVar;
	ID3DX11EffectVariable* mfxEyePosVar;
	ID3DX11EffectVariable* mfxLightVar;
	ID3DX11EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3DX11EffectShaderResourceVariable* mfxDiffuseMapVar2;
	ID3DX11EffectShaderResourceVariable* mfxSpecMapVar;
	ID3DX11EffectMatrixVariable* mfxTexMtxVar;
	ID3DX11EffectVariable *mfxTimeVar;

	DXMatrix mCrateWorld;

	DXMatrix mView;
	DXMatrix mProj;
	DXMatrix mWVP;

	DXVector3 mEyePos;
	float mRadius;
	float mTheta;
	float mPhi;

	// time variable for rotating texture
	static float mBaseTime;
	float mCurrTime;
	int mCurrFireIndex;
};

float CrateApp::mBaseTime = 0.0f;	// initialize static variable outside the class

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	CrateApp theApp(hInstance);
	
	theApp.initApp();

	return theApp.run();
}

CrateApp::CrateApp(HINSTANCE hInstance)
: D3DApp(hInstance), mFX(0), mTech(0), mfxWVPVar(0), mfxWorldVar(0), mfxEyePosVar(0),
  mfxLightVar(0), mfxDiffuseMapVar(0), mfxSpecMapVar(0), mfxTexMtxVar(0), 
  mVertexLayout(0), mDiffuseMapRV(0), mSpecMapRV(0), mEyePos(0.0f, 0.0f, 0.0f), 
  mRadius(5.0f), mTheta(0.0f), mPhi(PI*0.4f), mCurrTime(0.0f), mCurrFireIndex(0)
{
	mCrateWorld.Identity();
	mView.Identity();
	mProj.Identity();
	mWVP.Identity();

	// @TODO: remove when done testing
	//D3DXMatrixIdentity(&mCrateWorld);
	//D3DXMatrixIdentity(&mView);
	//D3DXMatrixIdentity(&mProj);
	//D3DXMatrixIdentity(&mWVP); 
}

CrateApp::~CrateApp()
{
	if (md3dDeviceContext)
		md3dDeviceContext->ClearState();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
	ReleaseCOM(mDiffuseMapRV);
	ReleaseCOM(mSpecMapRV);

	for(int i = 0; i < 120; ++i)
		ReleaseCOM(mFireMapRV[i]);
}

void CrateApp::initApp()
{
	D3DApp::initApp();

	mClearColor = DXColor(0.9f, 0.9f, 0.9f, 1.0f);
	
	buildFX();
	buildVertexLayouts();
	
	mCrateMesh.init(md3dDevice, 1.0f);

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice, 
		L"flare.dds", 0, &mDiffuseMapRV ));
	/*HR(DirectX::CreateDDSTextureFromFile(md3dDevice, 
		L"WoodCrate02.dds", 0, 0, &mDiffuseMapRV, 0 ));*/

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"flarealpha.dds", 0, &mDiffuseMapRV2));

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice, 
		L"defaultspec.dds", 0, &mSpecMapRV ));

	// initialize fire textures
	for(int i = 1; i <= 120; ++i)
	{
		std::wstringstream texNo;
		texNo << i;
		std::wstring *currFireTex;
		if(i < 10)
		{
			currFireTex = new std::wstring(L"FireAnim/Fire00" + texNo.str() + L".bmp");
		}
		else if(i < 100)
		{
			currFireTex = new std::wstring(L"FireAnim/Fire0" + texNo.str() + L".bmp");
		}
		else
		{
			currFireTex = new std::wstring(L"FireAnim/Fire" + texNo.str() + L".bmp");
		}
		HR(DirectX::CreateWICTextureFromFile(md3dDevice, md3dDeviceContext,
				currFireTex->c_str(), 0, &mFireMapRV[i-1]));
		delete currFireTex;
	}

	mParallelLight.dir      = DXVector3(0.57735f, -0.57735f, 0.57735f);
	mParallelLight.ambient  = DXColor(0.4f, 0.4f, 0.4f, 1.0f);
	mParallelLight.diffuse  = DXColor(1.0f, 1.0f, 1.0f, 1.0f);
	mParallelLight.specular = DXColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void CrateApp::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	mProj = DirectX::XMMatrixPerspectiveFovLH(0.25f*PI, aspect, 1.0f, 1000.0f);
	//D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);	// @TODO: remove when done testing
}

void CrateApp::updateScene(float dt)
{
	D3DApp::updateScene(dt);

	// Update angles based on input to orbit camera around scene.
	if(GetAsyncKeyState('A') & 0x8000)	mTheta -= 2.0f*dt;
	if(GetAsyncKeyState('D') & 0x8000)	mTheta += 2.0f*dt;
	if(GetAsyncKeyState('W') & 0x8000)	mPhi -= 2.0f*dt;
	if(GetAsyncKeyState('S') & 0x8000)	mPhi += 2.0f*dt;
	if(GetAsyncKeyState('Z') & 0x8000)	mRadius -= 15.0f*dt;
	if(GetAsyncKeyState('X') & 0x8000)	mRadius += 15.0f*dt;

	// Restrict the angle mPhi.
	if( mPhi < 0.1f )	mPhi = 0.1f;
	if( mPhi > PI-0.1f)	mPhi = PI-0.1f;

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

	// bound mCurrTime to [0,1] based on game_time - mBaseTime
	float slowDownFactor = 1.0f, maxRotAngle = 360.0f;
	int maxFireTex = 120;
	float fireTexInterval = 1.0f / (float)maxFireTex;
	if((mTimer.getGameTime() - mBaseTime) > 1.0f * slowDownFactor)
		mBaseTime = mTimer.getGameTime();
	mCurrTime = (mTimer.getGameTime() - mBaseTime)/slowDownFactor; //* maxRotAngle;
	mCurrFireIndex = int(mCurrTime*1000.0f) / int(fireTexInterval*1000.0f);

	// clamp mCurrFireIndex to [0,maxFireTex)
	mCurrFireIndex = Clamp(mCurrFireIndex,0,maxFireTex-1);
	/*mCurrFireIndex = mCurrFireIndex < 0 ? 0 : mCurrFireIndex;
	mCurrFireIndex = mCurrFireIndex >= maxFireTex ? maxFireTex-1 : mCurrFireIndex; */

	/*mCurrTime = mCurrTime < 0.0f ? 0.0f : mCurrTime;
	mCurrTime = mCurrTime > maxRotAngle ? maxRotAngle : mCurrTime;*/
}

void CrateApp::drawScene()
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

 
	// set constants
	mfxEyePosVar->SetRawValue(&mEyePos, 0, sizeof(DXVector3));
	mfxLightVar->SetRawValue(&mParallelLight, 0, sizeof(Light));
	mWVP = mCrateWorld*mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	mfxWorldVar->SetMatrix((float*)&mCrateWorld);

	//mfxDiffuseMapVar->SetResource(mDiffuseMapRV);
	mfxDiffuseMapVar->SetResource(mFireMapRV[mCurrFireIndex]);
	mfxDiffuseMapVar2->SetResource(mDiffuseMapRV2);
	mfxSpecMapVar->SetResource(mSpecMapRV);
	mfxTimeVar->SetRawValue(&mCurrTime, 0, sizeof(float));
 
	// Don't transform texture coordinates, so just use identity transformation (NOT!!!)
	// Rotate texture based on time
	float currRotAngle = mCurrTime * PI/180.0f;
	DXMatrix texMtx(DirectX::XMMatrixIdentity());
	texMtx = DirectX::XMMatrixAffineTransformation2D(DXVector2(1, 1), DXVector2(0.5f, 0.5f), currRotAngle, DXVector2(0, 0));

	// @TODO: remove when done testing
	/*D3DXMatrixIdentity(&texMtx);
	D3DXMatrixTransformation2D(&texMtx,NULL,NULL,NULL,&DXVector2(0.5f,0.5f),currRotAngle,NULL);*/
	mfxTexMtxVar->SetMatrix((float*)&texMtx);

    D3DX11_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0, md3dDeviceContext);
        
		mCrateMesh.draw();
    }

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mSpriteBatch->Begin();
	mFont->DrawString(mSpriteBatch.get(), mFrameStats.c_str(), DXVector2(0, 0));
	mSpriteBatch->End();
	//mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);	// @TODO: remove when done testing

	RECT RTime = {5, 55, 0, 0};
	std::wstringstream streamer;
	streamer << mCurrFireIndex; // mCurrTime;
	std::wstring currTimeString(L"Fire texture index: " + streamer.str());
	mSpriteBatch->Begin();
	mFont->DrawString(mSpriteBatch.get(), currTimeString.c_str(), DXVector2(0, 50));
	mSpriteBatch->End();
	//mFont->DrawText(0, currTimeString.c_str(), -1, &RTime, DT_NOCLIP, BLACK);	// @TODO: remove when done testing

	mSwapChain->Present(0, 0);
}

void CrateApp::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
  
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX11CompileEffectFromFile(L"tex.fx", 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, shaderFlags, 0, md3dDevice, &mFX, &compilationErrors);
	// @TODO: remov ewhen done testing
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
	mfxDiffuseMapVar2= mFX->GetVariableByName("gDiffuseMap2")->AsShaderResource();
	mfxSpecMapVar    = mFX->GetVariableByName("gSpecMap")->AsShaderResource();
	mfxTexMtxVar     = mFX->GetVariableByName("gTexMtx")->AsMatrix();
	mfxTimeVar		 = mFX->GetVariableByName("gCurrTime");
}

void CrateApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	const UINT nElements(3);
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// Create the input layout
    D3DX11_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, nElements, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}

 