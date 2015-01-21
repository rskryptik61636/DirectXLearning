// source file for FogApp which extends GenericApp

#include "FogApp.h"

// headers of scene objects
#include "TextureBox.h"
#include "TexturedWavesV1.h"
#include "TexturedPeaksAndValleysV1.h"

// link to DXEngine library
#ifdef _DEBUG
#pragma comment(lib, "DXEngined.lib")
#else
#pragma comment(lib, "DXEngine.lib")
#endif	// _DEBUG

FogApp::FogApp(HINSTANCE hInstance, std::wstring shaderFilename)
	: GenericApp(hInstance, shaderFilename)
{
	DXVector3 boxWorldTranslation(8.0f, 0.0f, -15.0f);
	mBoxWorldMatrix = DXMatrix::CreateTranslation(boxWorldTranslation);
	mWavesWorldMatrix.Identity();
	mTerrainWorldMatrix.Identity();
	// @TODO: remove when done testing
	//D3DXMatrixTranslation(&mBoxWorldMatrix, boxWorldTranslation.x, boxWorldTranslation.y, boxWorldTranslation.z);
	//D3DXMatrixIdentity(&mWavesWorldMatrix);
	//D3DXMatrixIdentity(&mTerrainWorldMatrix);
}

FogApp::~FogApp()
{
	// clear D3D device state
	if (D3DApp::md3dDeviceContext)
	{
		D3DApp::md3dDeviceContext->ClearState();
	}		

	// use of CComPtr should ensure all COM objects get released
}

void FogApp::updateScene(float dt)
{
	GenericApp::updateScene(dt);	// invoke base class version

	// animate waves texture
	mWavesTextureOffset.x += 0.1f*dt;
	mWavesTextureOffset.y += 0.25f * sinf(4.0f * mWavesTextureOffset.y);

	__int64 startTime, endTime, frequency;
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	const float msecPerCount = 1000.0f / (float)frequency;

	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	// update the waves
	mWaves->update(dt);

	QueryPerformanceCounter((LARGE_INTEGER*)&endTime);
	mUpdateTime += static_cast<float>(endTime - startTime) * msecPerCount;
	++mUpdateCounter;

}

void FogApp::createResources()
{
	// define shader resource view filenames
	std::wstring terrainRVName = L"grass.dds";
	std::wstring wavesRVName = L"water2.dds";
	std::wstring boxRVName = L"WireFence.dds";
	std::wstring defaultSpecularRVName = L"defaultspec.dds";

	// create shader resource views
	// @TODO: must find out if the ID3D11Resource* output from CreateDDSTextureFromFile is required or not
	HR( DirectX::CreateDDSTextureFromFile(md3dDevice, terrainRVName.c_str(), 0, &mTerrainResourceView.p) );
	HR( DirectX::CreateDDSTextureFromFile(md3dDevice, wavesRVName.c_str(), 0, &mWavesResourceView.p) );
	HR( DirectX::CreateDDSTextureFromFile(md3dDevice, boxRVName.c_str(), 0, &mBoxResourceView.p) );
	HR( DirectX::CreateDDSTextureFromFile(md3dDevice, defaultSpecularRVName.c_str(), 0, &mDefaultSpecularResourceView.p) );

	// @TODO: remove when done testing
	//HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, terrainRVName.c_str(), 0, 0, &mTerrainResourceView.p, 0));
	//HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, wavesRVName.c_str(), 0, 0, &mWavesResourceView.p, 0));
	//HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, boxRVName.c_str(), 0, 0, &mBoxResourceView.p, 0));
	//HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, defaultSpecularRVName.c_str(), 0, 0, &mDefaultSpecularResourceView.p, 0));

	// initialize waves texture offset
	mWavesTextureOffset.x = mWavesTextureOffset.y = 0.0f;

	// initialize scene light
	DXVector3 lightDirection(0.57735f, -0.57735f, 0.57735f);
	DXColor ambientLightColour(0.2f, 0.2f, 0.2f, 1.0f);
	mParallelLight.dir = lightDirection;
	mParallelLight.ambient = ambientLightColour;
	mParallelLight.diffuse = WHITE;
	mParallelLight.specular = WHITE;

	// create rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	HR( md3dDevice->CreateRasterizerState(&rasterizerDesc, &mNoCullRasterizerState.p) );

	// create blend state
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR( md3dDevice->CreateBlendState(&blendDesc, &mTransparentBlendState.p) );
}

void FogApp::createObjects()
{
	// create the box
	mBox.reset(new TextureBox());
	mBox->init(md3dDevice, 5.0f);

	// create the terrain
	DWORD terrainRows = 129, terrainCols = 129;
	float terrainDX = 1.0f;
	mTerrain.reset( new TexturedPeaksAndValleysV1(terrainRows, terrainCols, terrainDX) );
	mTerrain->init(md3dDevice, 1.0f);

	// create the waves
	DWORD wavesRows = 257, wavesCols = 257;
	float wavesDX = 0.5f, wavesDT = 0.03f, wavesSpeed = 3.25f, wavesDamping = 0.0f;
	mWaves.reset( new TexturedWavesV1() );
	//mWaves.reset(new Waves());
	mWaves->init(md3dDevice, wavesRows, wavesCols, wavesDX, wavesDT, wavesSpeed, wavesDamping);

	// generate some waves at startup
	for(int k = 0; k < 30; ++k)
	{
		DWORD i = 5 + rand() % 250;
		DWORD j = 5 + rand() % 250;

		float magnitude = RandF(0.5f, 1.25f);

		mWaves->disturb(i, j, magnitude);
	}

	// initialize the vertex type
	GenericApp::mPrimaryVertexType = TEXTURE_VERTEX;
}

void FogApp::drawObjects()
{
	// set box's world matrix to identity
	DXMatrix boxTextureMatrix(DXMatrix::Identity());
	//D3DXMatrixIdentity(&boxTextureMatrix);	// @TODO: remove when done testing

	// scale terrain (and waves) texture co-ordinates to (0,5) from (0,1)
	const float textureScaleFactor = 5.0f;
	DXMatrix textureScalingMatrix(DXMatrix::CreateScale(textureScaleFactor, textureScaleFactor, 1.0f));
	DXMatrix terrainTextureMatrix(textureScalingMatrix);
	// @TODO: remove when done testing
	//DXMatrix textureScalingMatrix;
	//D3DXMatrixScaling(&textureScalingMatrix, textureScaleFactor, textureScaleFactor, 1.0f);
	//DXMatrix terrainTextureMatrix = textureScalingMatrix;
	
	// waves' texture matrix has to be translated by the calculated offset
	DXMatrix wavesMotionMatrix(DXMatrix::CreateTranslation(mWavesTextureOffset.x, mWavesTextureOffset.y, 0.0f));
	//D3DXMatrixTranslation(&wavesMotionMatrix, mWavesTextureOffset.x, mWavesTextureOffset.y, 0.0f);	// @TODO: remove when done testing
	DXMatrix wavesTextureMatrix = textureScalingMatrix * wavesMotionMatrix;

	// get the technique description
	D3DX11_TECHNIQUE_DESC techniqueDesc;
	mTech->GetDesc(&techniqueDesc);

	// iterate over all passes and draw all the scene objects
	float blendFactors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	for(UINT i = 0; i < techniqueDesc.Passes; ++i)
	{
		ID3DX11EffectPass *currentPass = mTech->GetPassByIndex(i);

		// set terrain-specific constants and draw the terrain
		mWVP = mTerrainWorldMatrix * mView * mProj;
		mWVPMatrixVariable->SetMatrix( (float*)&mWVP );
		mWorldMatrixVariable->SetMatrix( (float*)&mTerrainWorldMatrix );
		mTextureMatrixVariable->SetMatrix( (float*)&terrainTextureMatrix );
		mDiffuseMapVariable->SetResource( mTerrainResourceView.p );
		mSpecularMapVariable->SetResource( mDefaultSpecularResourceView.p );
		currentPass->Apply(0, md3dDeviceContext);
		mTerrain->draw();

		// set box specific constants and draw the box
		mWVP = mBoxWorldMatrix * mView * mProj;
		mWVPMatrixVariable->SetMatrix( (float*)&mWVP );
		mWorldMatrixVariable->SetMatrix( (float*)&mBoxWorldMatrix );
		mTextureMatrixVariable->SetMatrix( (float*)&boxTextureMatrix );
		mDiffuseMapVariable->SetResource( mBoxResourceView.p );
		mSpecularMapVariable->SetResource( mDefaultSpecularResourceView.p );

		// set the no cull rasterizer state and draw the box
		md3dDeviceContext->RSSetState( mNoCullRasterizerState.p );
		currentPass->Apply(0, md3dDeviceContext);
		mBox->draw();
		md3dDeviceContext->RSSetState( 0 );

		// set waves specific constants and draw the box
		mWVP = mWavesWorldMatrix * mView * mProj;
		mWVPMatrixVariable->SetMatrix( (float*)&mWVP );
		mWorldMatrixVariable->SetMatrix( (float*)&mWavesWorldMatrix );
		mTextureMatrixVariable->SetMatrix( (float*)&wavesTextureMatrix );
		mDiffuseMapVariable->SetResource( mWavesResourceView.p );
		mSpecularMapVariable->SetResource( mDefaultSpecularResourceView.p );
		
		// set the transparent blend state and draw the waves
		currentPass->Apply(0, md3dDeviceContext);
		md3dDeviceContext->OMSetBlendState( mTransparentBlendState.p, blendFactors, 0xffffffff );
		mWaves->draw();
	}
}

void FogApp::getShaderVariables()
{
	// store the shader variable names to avoid mishaps
	std::string techniqueName = "FogTechnique";
	std::string wvpName = "gWVPMatrix";
	std::string worldName = "gWorldMatrix";
	std::string textureName = "gTextureMatrix";
	std::string eyePosName = "gEyePosW";
	std::string lightName = "gLight";
	std::string diffuseMapName = "gDiffuseMap";
	std::string specularMapName = "gSpecularMap";

	// get the effect technique
	//mTech = mFX->GetTechniqueByName("ClipTech");
	mTech = mFX->GetTechniqueByName(techniqueName.c_str());

	// get the shader matrix variables
	/*mWVPMatrixVariable = mFX->GetVariableByName("gWVP")->AsMatrix();
	mWorldMatrixVariable = mFX->GetVariableByName("gWorld")->AsMatrix();
	mTextureMatrixVariable = mFX->GetVariableByName("gTexMtx")->AsMatrix();*/
	mWVPMatrixVariable = mFX->GetVariableByName(wvpName.c_str())->AsMatrix();
	mWorldMatrixVariable = mFX->GetVariableByName(worldName.c_str())->AsMatrix();
	mTextureMatrixVariable = mFX->GetVariableByName(textureName.c_str())->AsMatrix();

	// get the eye position and scene light shader variables
	mEyePositionVariable = mFX->GetVariableByName(eyePosName.c_str());
	mLightVariable = mFX->GetVariableByName(lightName.c_str());

	// get the shader resource variables
	mDiffuseMapVariable = mFX->GetVariableByName(diffuseMapName.c_str())->AsShaderResource();
	mSpecularMapVariable = mFX->GetVariableByName(specularMapName.c_str())->AsShaderResource();
	//mSpecularMapVariable = mFX->GetVariableByName("gSpecMap")->AsShaderResource();
}

void FogApp::setShaderVariables()
{
	mEyePositionVariable->SetRawValue(&mEyePos, 0, sizeof(DXVector3));
	mLightVariable->SetRawValue(&mParallelLight, 0, sizeof(Light));
}