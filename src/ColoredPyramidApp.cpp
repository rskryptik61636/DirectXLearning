#include "ColoredPyramidApp.h"

ColoredPyramidApp::ColoredPyramidApp(HINSTANCE hInstance) : D3DApp(hInstance), mTheta(0), mPhi(PI*0.25f)
{
	D3DXMatrixIdentity(&mWorld);
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP);
}

ColoredPyramidApp::~ColoredPyramidApp()
{
	if(md3dDevice)
		md3dDevice->ClearState();

	ReleaseCOM(mFx);
	ReleaseCOM(mVertexLayout);
}

void ColoredPyramidApp::initApp()
{
	D3DApp::initApp();

	//mPyramid.init(md3dDevice,1.0);
	mCubamid.init(md3dDevice,1.0);

	buildFX();
	buildVertexLayouts();
}

void ColoredPyramidApp::onResize()
{
	D3DApp::onResize();

	float aspect = float(mClientWidth) / float(mClientHeight);
	D3DXMatrixPerspectiveFovLH(&mProj,0.25*PI,aspect,1.0f,1000.0f);
}

void ColoredPyramidApp::drawScene()
{
	D3DApp::drawScene();

	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0,0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
	md3dDevice->IASetInputLayout(mVertexLayout);
	md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ);
	
	// set rasterizer state to wireframe
	md3dDevice->RSSetState(mWireframeRS);
	
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3D10EffectPass *pass = mTech->GetPassByIndex(p);//->Apply(0);
		//mPyramid.draw();

		// set constants and draw cube
		D3DXMatrixIdentity(&mWorld);
		mWVP = mWorld * mView * mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		pass->Apply(0);
		mCubamid.drawCube();

		// mod world matrix and draw pyramid
		mWorld = D3DXMATRIX(	1.0F, 0.0F, 0.0F, 0.0F,
							0.0F, 1.0F, 0.0F, 0.0F,
							0.0F, 0.0F, 1.0F, 0.0F,
							3.0F, 1.0F, 0.0F, 1.0F);
		mWVP = mWorld * mView * mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		pass->Apply(0);
		mCubamid.drawPyramid();
	}

	// set rasterizer state back to solid
	md3dDevice->RSSetState(mSolidRS);

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);

	mSwapChain->Present(0, 0);
}

void ColoredPyramidApp::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif	//endif

	ID3D10Blob *compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"color.fx",		// effects script filename
									0,					// D3D10 shader macro
									0,					// additional include files
									"fx_4_0",			// shader version
									shaderFlags,		// shader flags
									0,					// FX flags
									md3dDevice,			// D3D device
									0,					// effect pool
									0,					// thread pump
									&mFx,				// effect
									&compilationErrors,	// errors (if any)
									0);					// result ptr

	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	}

	mTech = mFx->GetTechniqueByName("ColorTech");
	mfxWVPVar = mFx->GetVariableByName("gWVP")->AsMatrix();

	// define a wireframe rasterizer state
	D3D10_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc,sizeof(D3D10_RASTERIZER_DESC));
	rsDesc.FillMode = D3D10_FILL_WIREFRAME;
	rsDesc.CullMode = D3D10_CULL_BACK;
	rsDesc.FrontCounterClockwise = false;
	HR(md3dDevice->CreateRasterizerState(&rsDesc,&mWireframeRS));

	// define a solid rasterizer state
	D3D10_RASTERIZER_DESC rsDesc2;
	ZeroMemory(&rsDesc2,sizeof(D3D10_RASTERIZER_DESC));
	rsDesc2.FillMode = D3D10_FILL_SOLID;
	rsDesc2.CullMode = D3D10_CULL_NONE;
	rsDesc.FrontCounterClockwise = false;
	HR(md3dDevice->CreateRasterizerState(&rsDesc2,&mSolidRS));
}

void ColoredPyramidApp::updateScene(float dt)
{
	D3DApp::updateScene(dt);

	// define keyboard interaction
	if(GetAsyncKeyState('A') & 0x8000)	mTheta -= 2.0f*dt;
	if(GetAsyncKeyState('D') & 0x8000)	mTheta += 2.0f*dt;
	if(GetAsyncKeyState('W') & 0x8000)	mPhi -= 2.0f*dt;
	if(GetAsyncKeyState('S') & 0x8000)	mPhi += 2.0f*dt;

	// Restrict the angle mPhi.
	if( mPhi < 0.1f )	mPhi = 0.1f;
	if( mPhi > PI-0.1f)	mPhi = PI-0.1f;

	// Convert Spherical to Cartesian coordinates: mPhi measured from +y
	// and mTheta measured counterclockwise from -z.
	float x =  15.0f*sinf(mPhi)*sinf(mTheta);
	float z = -15.0f*sinf(mPhi)*cosf(mTheta);
	float y =  15.0f*cosf(mPhi);

	// build the view matrix
	D3DXVECTOR3 position(x,y,z);
	D3DXVECTOR3 lookAt(0.0f,0.0f,0.0f);
	D3DXVECTOR3 upVector(0.0f,1.0f,0.0f);
	D3DXMatrixLookAtLH(&mView,&position,&lookAt,&upVector);
}

void ColoredPyramidApp::buildVertexLayouts()
{
	// create the input element desc
	D3D10_INPUT_ELEMENT_DESC vertexDesc1[] = 
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D10_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D10_INPUT_PER_VERTEX_DATA,0}
	};

	// create the input layout
	D3D10_PASS_DESC PassDesc;
	mTech->GetPassByIndex(0)->GetDesc(&PassDesc);

	HR(md3dDevice->CreateInputLayout(vertexDesc1,2,PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize,&mVertexLayout));
}