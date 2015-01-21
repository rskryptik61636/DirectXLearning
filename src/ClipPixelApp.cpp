// source file for ClipPixelApp which extends GenericApp

#include "ClipPixelApp.h"

#include "TextureBox.h"
#include "TexturedPeaksAndValleysV1.h"
#include "TexturedWavesV1.h"

#ifdef _DEBUG
#pragma comment(lib, "DXEngined.lib")
#else
#pragma comment(lib, "DXEngine.lib")
#endif	// _DEBUG

ClipPixelApp::ClipPixelApp(HINSTANCE hInstance) 
	: GenericApp(hInstance), mTerrainRV(0), mWavesRV(0), mBoxRV(0), mDefaultSpecRV(0),
	mfxWVPVar(0), mfxWorldVar(0), mfxEyePosVar(0), mfxLightVar(0), mfxDiffuseMapVar(0),
	mfxSpecMapVar(0), mfxTexMtxVar(0)//, mRadius(75.0f)
{
	GenericApp::mRadius = 75.0f;

	// initialize object specific world matrices
	mCrateWorld = DirectX::XMMatrixTranslation(8.0f, 0.0f, -15.0f);
	mTerrainWorld.Identity();
	mWavesWorld.Identity();
	// @TODO: remove when done testing
	/*D3DXMatrixTranslation(&mCrateWorld, 8.0f, 0.0f, -15.0f);
	D3DXMatrixIdentity(&mTerrainWorld);
	D3DXMatrixIdentity(&mWavesWorld);*/
}

ClipPixelApp::ClipPixelApp(HINSTANCE hInstance, std::wstring shaderFilename) 
	: GenericApp(hInstance,shaderFilename), mTerrainRV(0), mWavesRV(0), mBoxRV(0), mDefaultSpecRV(0),
	mfxWVPVar(0), mfxWorldVar(0), mfxEyePosVar(0), mfxLightVar(0), mfxDiffuseMapVar(0),
	mfxSpecMapVar(0), mfxTexMtxVar(0)//, mRadius(75.0f)
{
	GenericApp::mRadius = 75.0f;

	// initialize object specific world matrices
	mCrateWorld = DirectX::XMMatrixTranslation(8.0f, 0.0f, -15.0f);
	mTerrainWorld.Identity();
	mWavesWorld.Identity();
	// @TODO: remove when done testing
	/*D3DXMatrixTranslation(&mCrateWorld, 8.0f, 0.0f, -15.0f);
	D3DXMatrixIdentity(&mTerrainWorld);
	D3DXMatrixIdentity(&mWavesWorld);*/
}

void ClipPixelApp::updateScene(float dt)
{
	__int64 startTime, endTime, frequency;
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	const float msecPerCount = 1000.0f / (float)frequency;

	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	GenericApp::updateScene(dt);	// invoke base class version

	// disturb water as a function of time
	mWavesTexOffset.y += 0.1f * dt;
	mWavesTexOffset.x = 0.25f * sinf(4.0f * mWavesTexOffset.y);
	mWaves->update(dt);

	QueryPerformanceCounter((LARGE_INTEGER*)&endTime);
	mUpdateTime += static_cast<float>(endTime - startTime) * msecPerCount;
	++mUpdateCounter;
}

ClipPixelApp::~ClipPixelApp()
{
	// release D3D device
	if(GenericApp::md3dDevice)	
		GenericApp::md3dDeviceContext->ClearState();

	// delete scene objects
	if(mBox)
		delete mBox;

	if(mTerrain)
		delete mTerrain;

	if(mWaves)
		delete mWaves;

	// release resource views
	/*ReleaseCOM(mTerrainRV);
	ReleaseCOM(mWavesRV);
	ReleaseCOM(mBoxRV);
	ReleaseCOM(mDefaultSpecRV);*/

	// release rasterizer and blend states
	/*ReleaseCOM(mNoCullRS);
	ReleaseCOM(mTransparentBS);*/
}

void ClipPixelApp::createResources()
{
	// define texture names
	std::wstring terrainTex		= L"grass.dds";
	std::wstring wireFenceTex = /*L"N:\\DirectXLearning\\shaders\\WoodCrate01.dds";*/  L"WireFence.dds"; /*L"water2a.dds";*/
	std::wstring wavesTex		= L"water2a.dds";
	std::wstring defaultSpecTex = L"defaultspec.dds";

	// initialize shader resource views
	HR( DirectX::CreateDDSTextureFromFile( md3dDevice,
												terrainTex.c_str(),
												//0,
												0,
												&mTerrainRV,
												0) );

	HR( DirectX::CreateDDSTextureFromFile( md3dDevice,
												wireFenceTex.c_str(),
												//0,
												0,
												&mBoxRV,
												0) );

	HR( DirectX::CreateDDSTextureFromFile( md3dDevice,
												wavesTex.c_str(),
												//0,
												0,
												&mWavesRV,
												0) );

	HR( DirectX::CreateDDSTextureFromFile( md3dDevice,
												defaultSpecTex.c_str(),
												//0,
												0,
												&mDefaultSpecRV,
												0) );

	// initialize water texture offset
	mWavesTexOffset = DXVector2(0.0f, 0.0f);

	// initialize scene light
	DXVector3 lightDir(0.5f, -0.5f, 0.5f);
	DXColor ambientColor(0.2f, 0.2f, 0.2f, 1.0f);
	mParallelLight.dir = lightDir;
	mParallelLight.ambient = ambientColor;
	mParallelLight.diffuse = WHITE;
	mParallelLight.specular = WHITE;

	// initialize rasterizer state (NOTE: enums commented for description's sake)
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = /*D3D10_FILL_MODE::*/D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = /*D3D10_CULL_MODE::*/D3D11_CULL_NONE;
	HR( md3dDevice->CreateRasterizerState( &rasterizerDesc, &mNoCullRS ) );

	// initialize blend state (NOTE: enums commented for description's sake)
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.AlphaToCoverageEnable		= false;
	blendDesc.RenderTarget[0].BlendEnable			= true;
	blendDesc.RenderTarget[0].SrcBlend					= /*D3D10_BLEND::*/D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend					= /*D3D10_BLEND::*/D3D11_BLEND_INV_SRC_ALPHA;
	//blendDesc.RenderTarget[0].SrcBlend					= D3D10_BLEND::D3D10_BLEND_INV_DEST_ALPHA;	// this combination gives the
	//blendDesc.RenderTarget[0].DestBlend					= D3D10_BLEND::D3D10_BLEND_INV_DEST_ALPHA;	// -darkest result
	blendDesc.RenderTarget[0].BlendOp					= /*D3D10_BLEND_OP::*/D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha				= /*D3D10_BLEND::*/D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha			= /*D3D10_BLEND::*/D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha				= /*D3D10_BLEND_OP::*/D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask	= /*D3D10_COLOR_WRITE_ENABLE::*/D3D11_COLOR_WRITE_ENABLE_ALL;
	HR( md3dDevice->CreateBlendState( &blendDesc, &mTransparentBS ) );
}

void ClipPixelApp::createObjects()
{
	// create box
	mBox = new TextureBox();
	mBox->init(md3dDevice, 3.0f);

	// create terrain
	DWORD terrainBlockSize = 129;
	float spatialOffset = 1.0f;
	mTerrain = new TexturedPeaksAndValleysV1(terrainBlockSize, terrainBlockSize, spatialOffset);
	mTerrain->init(md3dDevice, 1.0f);

	// create waves and generate at start up
	DWORD wavesBlockSize = 257;
	float spatialStep = 0.5f, timeStep = 0.03f, speed = 3.25f, damping = 0.0f;
	mWaves = new TexturedWavesV1();
	mWaves->init(md3dDevice, wavesBlockSize, wavesBlockSize, spatialStep, timeStep, speed, damping);

	// disturb the waves
	for(int k = 0; k < 30; ++k)
	{
		DWORD i = 5 + rand() % 250;
		DWORD j = 5 + rand() % 250;

		float magnitude = RandF(0.5f, 1.25f);

		mWaves->disturb(i, j, magnitude);
	}

	// set primary vertex type
	GenericApp::mPrimaryVertexType = TEXTURE_VERTEX;
}

void ClipPixelApp::drawObjects()
{
	// scale texture coordinates from [0,1] -> [0,5]
	const float scalingFactor = 5.0f;
	DXMatrix scalingMatrix = DirectX::XMMatrixScaling(scalingFactor, scalingFactor, 1.0f);
	//D3DXMatrixScaling(&scalingMatrix, scalingFactor, scalingFactor, 1.0f);

	// set terrain matrix 
	DXMatrix terrainTexMtx = scalingMatrix;

	// let box tex coords remain as-is
	DXMatrix boxTexMtx(DirectX::XMMatrixIdentity());
	//D3DXMatrixIdentity(&boxTexMtx);

	// offset waves texture by mWavesTexOffset
	DXMatrix translationMatrix = DirectX::XMMatrixTranslation(mWavesTexOffset.x, mWavesTexOffset.y, 0.0f);
	//D3DXMatrixTranslation(&translationMatrix, mWavesTexOffset.x, mWavesTexOffset.y, 0.0f);
	DXMatrix wavesTexMtx = scalingMatrix * translationMatrix;

	// set the technique desc
	D3DX11_TECHNIQUE_DESC techniqueDesc;
	mTech->GetDesc(&techniqueDesc);

	// blend factor for the transparent blend state
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// iterate over each pass of the technique and draw all the objects in the scene
	for(size_t i = 0; i < techniqueDesc.Passes; ++i)
	{
		// get the current pass
		ID3DX11EffectPass *currPass = mTech->GetPassByIndex(i);

		// setup and draw the terrain
		mWVP = mTerrainWorld * mView * mProj;							// calculate the terrain world-view-projection matrix
		mfxWVPVar->SetMatrix( (float*)&mWVP );				// set the terrain world-view-projection matrix
		mfxWorldVar->SetMatrix( (float*)&mTerrainWorld );	// set the terrain world matrix
		mfxTexMtxVar->SetMatrix( (float*)&terrainTexMtx );	// set the terrain texture matrix
		mfxDiffuseMapVar->SetResource(mTerrainRV);						// set the terrain diffuse colour map
		mfxSpecMapVar->SetResource(mDefaultSpecRV);						// set the terrain specular colour map
		currPass->Apply(0, md3dDeviceContext);												// apply the current pass
		mTerrain->draw();												// draw the terrain

		// setup and draw the box
		mWVP = mCrateWorld * mView * mProj;								// calculate the box world-view-projection matrix
		mfxWVPVar->SetMatrix( (float*)&mWVP );				// set the box world-view-projection matrix
		mfxWorldVar->SetMatrix( (float*)&mCrateWorld );		// set the box world matrix
		mfxTexMtxVar->SetMatrix( (float*)&boxTexMtx );		// set the box texture matrix
		mfxDiffuseMapVar->SetResource(mBoxRV);							// set the box diffuse colour map
		mfxSpecMapVar->SetResource(mDefaultSpecRV);						// set the box specular colour map

		// disable backface culling because the gate texture has transparent regions
		md3dDeviceContext->RSSetState(mNoCullRS);
		currPass->Apply(0, md3dDeviceContext);
		mBox->draw();
		md3dDeviceContext->RSSetState(0);	// restore to default state

		// setup and draw the waves
		mWVP = mWavesWorld * mView * mProj;
		mfxWVPVar->SetMatrix( (float*)&mWVP );
		mfxWorldVar->SetMatrix( (float*)&mWavesWorld );
		mfxTexMtxVar->SetMatrix( (float*)&wavesTexMtx );
		mfxDiffuseMapVar->SetResource(mWavesRV);
		mfxSpecMapVar->SetResource(mDefaultSpecRV);
		currPass->Apply(0, md3dDeviceContext);
		md3dDeviceContext->OMSetBlendState(mTransparentBS, blendFactor, 0xffffffff);
		mWaves->draw();
	}
}

void ClipPixelApp::setShaderVariables()
{
	mfxEyePosVar->SetRawValue(&mEyePos, 0, sizeof(DXVector3));
	mfxLightVar->SetRawValue(&mParallelLight, 0, sizeof(Light));
}

void ClipPixelApp::getShaderVariables()
{
	// get the shader technique
	mTech = mFX->GetTechniqueByName("ClipTech");

	// get the shader variables
	mfxWVPVar = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVar = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxTexMtxVar = mFX->GetVariableByName("gTexMtx")->AsMatrix();
	mfxEyePosVar = mFX->GetVariableByName("gEyePosW");
	mfxLightVar = mFX->GetVariableByName("gLight");
	mfxDiffuseMapVar = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mfxSpecMapVar = mFX->GetVariableByName("gSpecMap")->AsShaderResource();
}