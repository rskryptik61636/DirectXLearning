// ColoredCubeApp class - source file

#include "ColoredCubeApp.h"

//// link to DXEngine library
//#ifdef _DEBUG
//#pragma comment(lib, "DXEngined.lib")
//#else
//#pragma comment(lib, "DXEngine.lib")
//#endif	// _DEBUG

ColoredCubeApp::ColoredCubeApp(HINSTANCE hInstance) 
	:	D3DApp(hInstance), mFx(0), mTech(0), mVertexLayout(0),
		mfxWVPVar(0), mfxGtimeVar(0), mTheta(0.0f), mPhi(PI*0.25f)
{
	mWVP.Identity();
	mView.Identity();
	mProj.Identity();

	// @TODO: remove when done testing
	/*D3DXMatrixIdentity(&mWVP);
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);*/
}

ColoredCubeApp::~ColoredCubeApp()
{
	if(md3dDevice)
		md3dDeviceContext->ClearState();

	ReleaseCOM(mFx);
	ReleaseCOM(mVertexLayout);
}

void ColoredCubeApp::initApp()
{
	D3DApp::initApp();

	mBox.init(md3dDevice,1.0f);

	buildFX();
	buildVertexLayouts();
}

void ColoredCubeApp::onResize()
{
	D3DApp::onResize();

	float aspect = float(mClientWidth) / float(mClientHeight);
	mProj = DirectX::XMMatrixPerspectiveFovLH(0.25*PI, aspect, 1.0f, 1000.0f);
	//D3DXMatrixPerspectiveFovLH(&mProj,0.25*PI,aspect,1.0f,1000.0f);	// @TODO: remove when done testing
}

void ColoredCubeApp::updateScene(float dt)
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
	float x =  5.0f*sinf(mPhi)*sinf(mTheta);
	float z = -5.0f*sinf(mPhi)*cosf(mTheta);
	float y =  5.0f*cosf(mPhi);

	// build the view matrix
	DXVector3 position(x,y,z);
	DXVector3 lookAt(0.0f,0.0f,0.0f);
	DXVector3 upVector(0.0f,1.0f,0.0f);
	mView = DirectX::XMMatrixLookAtLH(position, lookAt, upVector);
	//D3DXMatrixLookAtLH(&mView,&position,&lookAt,&upVector);	// @TODO: remove when done testing
}

void ColoredCubeApp::drawScene()
{
	D3DApp::drawScene();

	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDeviceContext->OMSetDepthStencilState(0,0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDeviceContext->OMSetBlendState(0, blendFactors, 0xffffffff);
	md3dDeviceContext->IASetInputLayout(mVertexLayout);
	md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//md3dDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ);
	
	// set constants
	mWVP = mView * mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	mfxGtimeVar->SetFloat(mTimer.getGameTime());

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, md3dDeviceContext);
		mBox.draw();
	}

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mSpriteBatch->Begin();
	mFont->DrawString(mSpriteBatch.get(), mFrameStats.c_str(), DirectX::XMFLOAT2(0, 0));
	mSpriteBatch->End();
	//mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);	// @TODO: remove when done testing

	mSwapChain->Present(0, 0);
}

void ColoredCubeApp::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif	//endif

	ID3DBlob *compilationErrors = 0;
	HRESULT hr = 0;

	hr = D3DX11CompileEffectFromFile(L"color.fx",	// effects script filename
									0,							// D3D shader macro
									D3D_COMPILE_STANDARD_FILE_INCLUDE,							// additional include files
									//shaderVersion.c_str(),		// shader version
									shaderFlags,				// shader flags
									0,							// FX flags
									md3dDevice,					// D3D device
									//0,							// effect pool
									//0,							// thread pump
									&mFx,						// effect
									&compilationErrors);			// errors (if any)
	//0);							// result ptr

	// @TODO: remove when done testing
	//hr = D3DX10CreateEffectFromFile(L"color.fx",		// effects script filename
	//								0,					// D3D10 shader macro
	//								0,					// additional include files
	//								"fx_4_0",			// shader version
	//								shaderFlags,		// shader flags
	//								0,					// FX flags
	//								md3dDevice,			// D3D device
	//								0,					// effect pool
	//								0,					// thread pump
	//								&mFx,				// effect
	//								&compilationErrors,	// errors (if any)
	//								0);					// result ptr

	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILEW__, (DWORD)__LINE__, hr, L"D3DX11CompileEffectFromFile", true);
	}

	mTech = mFx->GetTechniqueByName("ColorTech");
	mfxWVPVar = mFx->GetVariableByName("gWVP")->AsMatrix();
	mfxGtimeVar = mFx->GetVariableByName("gTime")->AsScalar();
}

void ColoredCubeApp::buildVertexLayouts()
{
	// create the input vertex layout
	D3D11_INPUT_ELEMENT_DESC vertexDesc1[] = 
	{
		{	
			"POSITION",						// semantic name
			0,								// semantic index
			DXGI_FORMAT_R32G32B32_FLOAT,	// format
			0,								// input slot
			0,								// aligned byte offset
			D3D11_INPUT_PER_VERTEX_DATA,	// input slot class
			0								// instance data step rate
		},

		//{"TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},	// 0 + 12

		//{"BINORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0},	// 12 + 12

		//{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,36,D3D11_INPUT_PER_VERTEX_DATA,0},	// 24 + 12

		//{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,48,D3D11_INPUT_PER_VERTEX_DATA,0},		// 36 + 12

		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}	
	};

	// create the input vertex layout
	D3D11_INPUT_ELEMENT_DESC vertexDesc2[] = 
	{
		{	
			"POSITION",						// semantic name
			0,								// semantic index
			DXGI_FORMAT_R32G32B32_FLOAT,	// format
			0,								// input slot
			0,								// aligned byte offset
			D3D11_INPUT_PER_VERTEX_DATA,	// input slot class
			0								// instance data step rate
		},

		{"TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},	// 0 + 12

		{"BINORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0},	// 12 + 12

		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,36,D3D11_INPUT_PER_VERTEX_DATA,0},	// 24 + 12

		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,48,D3D11_INPUT_PER_VERTEX_DATA,0},		// 36 + 12

		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,56,D3D11_INPUT_PER_VERTEX_DATA,0}	// 48 + 8
	};

	// create the input layout
	D3DX11_PASS_DESC PassDesc;
	mTech->GetPassByIndex(0)->GetDesc(&PassDesc);

	// VERY IMPORTANT: make sure that the number of elements ALWAYS matches with the input element desc
	/*HR(md3dDevice->CreateInputLayout(vertexDesc1,2,PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize,&mVertexLayout));*/
	HR(md3dDevice->CreateInputLayout(vertexDesc2,6,PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize,&mVertexLayout));
}