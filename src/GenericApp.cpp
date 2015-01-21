// source file for GenericApp class

#include "GenericApp.h"
#include <sstream>

extern "C"	{
#include "time.h"
}

GenericApp::GenericApp(HINSTANCE hInstance)
	: D3DApp(hInstance), /*mFX(0), mTech(0), 
	mVertexLayout(0),*/ mEyePos(0.0f, 0.0f, 0.0f), 
	mRadius(75.0f), mTheta(0.0f), mPhi(PI*0.4f), 
	mShaderFilename(NULL)
	// initialize shader and shader resource variables
{
	// set world matrix to identity

	// set all other pipeline matrices to identity
	mView.Identity();
	mProj.Identity();
	mWVP.Identity();

	// @TODO: remove when done testing
	//D3DXMatrixIdentity(&mView);
	//D3DXMatrixIdentity(&mProj);
	//D3DXMatrixIdentity(&mWVP); 
}

GenericApp::GenericApp(HINSTANCE hInstance, std::wstring shaderFilename)
	: D3DApp(hInstance), /*mFX(0), mTech(0), 
	mVertexLayout(0), mEyePos(0.0f, 0.0f, 0.0f), */
	mEyePos(0.0f, 0.0f, -50.0f),
	mRadius(75.0f), mTheta(0.0f), mPhi(PI*0.4f), 
	mShaderFilename(shaderFilename)
	// initialize shader and shader resource variables
{
	// init the camera
	DXVector3 lookAt(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f);
	float FOVAngle = 0.25f * PI;
	float nearPlane = 1.0f, farPlane = 1000.0f;
	float aspectRatio = (float)mClientWidth / (float)mClientHeight;
	mCamera.reset(new DXCamera(mEyePos, lookAt, up, FOVAngle, aspectRatio, nearPlane, farPlane));

	// set world matrix to identity

	// set all other pipeline matrices to identity
	mView.Identity();
	mProj.Identity();
	mWVP.Identity();

	// @TODO: remove when done testing
	/*D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); */
}

GenericApp::~GenericApp()
{
	// NOTE: better if this happens in the derived class
	//if(md3dDevice)	md3dDeviceContext->ClearState();

	/*ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);*/
	
	// release all shader resource variables
}

void GenericApp::initApp()
{
	D3DApp::initApp();

	mClearColor = DXColor(0.0f,0.0f,0.0f,1.0f);	// set clear color to black

	// create and intialize scene objects
	createObjects();

	// build shader effects, vertex layouts and initialize all resources
	buildFX();
	buildVertexLayouts();
	createResources();
}

void GenericApp::onResize()
{
	D3DApp::onResize();

	// update the aspect ratio of the camera
	mCamera->setAspectRatio((float)mClientWidth / (float)mClientHeight);

	// update mProj
	mProj = mCamera->proj();

	/*float FOVAngle = 0.25f * PI;
	float nearPlane = 1.0f, farPlane = 1000.0f;
	float aspectRatio = (float)mClientWidth / (float)mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj,FOVAngle,aspectRatio,nearPlane,farPlane);*/
}

void GenericApp::updateScene(float dt)
{
	D3DApp::updateScene(dt);

	// define keyboard input to rotate camera and to zoom in/out
	float motionScaleFactor = 50.0f;
	float zoomFactor = 1.0f;

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
	if(GetAsyncKeyState('U') & 0x8000)	mCamera->roll(zoomFactor * dt);			// roll left
	if(GetAsyncKeyState('O') & 0x8000)	mCamera->roll(-zoomFactor * dt);			// roll right

	// zoom in/out according to the keyboard input
	if (GetAsyncKeyState('Z') & 0x8000)	mCamera->zoomOut();	// zoom out
	if (GetAsyncKeyState('X') & 0x8000)	mCamera->zoomIn();	// zoom in

	// update mView and mProj and mEyePos
	mView = mCamera->view();
	mProj = mCamera->proj();
	mEyePos = mCamera->eyePosW();

#if 0
	if(GetAsyncKeyState('A') & 0x8000)	mTheta -= motionScaleFactor * dt;	// rotate left	
	if(GetAsyncKeyState('D') & 0x8000)	mTheta += motionScaleFactor * dt;	// rotate right
	if(GetAsyncKeyState('W') & 0x8000)	mPhi -= motionScaleFactor * dt;		// rotate down
	if(GetAsyncKeyState('S') & 0x8000)	mPhi += motionScaleFactor * dt;		// rotate up
	if(GetAsyncKeyState('Z') & 0x8000)	mRadius -= zoomFactor * dt;			// zoom in
	if(GetAsyncKeyState('X') & 0x8000)	mRadius += zoomFactor * dt;			// zoom out

	// restrict mPhi to [phiClamp,PI-phiClamp]
	float phiClamp = 0.1f;
	mPhi = mPhi < phiClamp ? phiClamp : mPhi;
	mPhi = mPhi > PI-phiClamp ? PI-phiClamp : mPhi;

	// Convert Spherical to Cartesian coordinates: mPhi measured from +y
	// and mTheta measured counterclockwise from -z.
	mEyePos.x = mRadius*sinf(mPhi)*sinf(mTheta);	// y = r*sin(Phi)*sin(Theta)	[NOTE: rotating about y axis]
	mEyePos.z = -mRadius*sinf(mPhi)*cosf(mTheta);	// x = r*sin(Phi)*cos(Theta)	[NOTE: rotating about x axis in opp direction]
	mEyePos.y = mRadius*cosf(mPhi);					// z = r*cos(Phi)				[NOTE: rotating about z axis]

	// Build the view matrix
	DXVector3 target(0.0f,0.0f,0.0f);
	DXVector3 up(0.0f,1.0f,0.0f);
	D3DXMatrixLookAtLH(&mView,&mEyePos,&target,&up);
#endif	// 0
}

void GenericApp::drawScene()
{
	// render profiling attempt
	__int64 startTime, endTime, frequency;
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	const float msecPerCount = 1000.0f / (float)frequency;

	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	D3DApp::drawScene();

	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDeviceContext->OMSetDepthStencilState(0,0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDeviceContext->OMSetBlendState(0,blendFactors,0xffffffff);
	md3dDeviceContext->IASetInputLayout(mVertexLayout.p);
	md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set shader variables here
	setShaderVariables();

	// draw objects
	drawObjects();

	QueryPerformanceCounter((LARGE_INTEGER*)&endTime);
	mDrawTime += static_cast<float>(endTime-startTime) * msecPerCount;	// time elapsed in ms
	++mDrawCounter;
	
	/*std::wostringstream ostr;
	ostr.precision(15);
	ostr << L"Draw time: " << frameElapsed << L" ms\n\n";
	mFrameStats += ostr.str();*/

	// draw frame stats
	mSpriteBatch->Begin();
	mFont->DrawString(mSpriteBatch.get(), mFrameStats.c_str(), DirectX::XMFLOAT2(0, 0));
	mSpriteBatch->End();
	//RECT frameStatsRect = {5, 5, 0, 0};
	//mFont->DrawText(0,mFrameStats.c_str(),-1,&frameStatsRect,DT_NOCLIP,WHITE);	// @TODO: remove when done testing

	mSwapChain->Present(0, 0);
}

void GenericApp::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif	// _DEBUG

	ID3DBlob *compilationErrors = 0;
	HRESULT hr = 0;
	std::string shaderVersion("fx_4_0");
	//ID3DInclude *pInclude;
	hr = D3DX11CompileEffectFromFile(mShaderFilename.c_str(),	// effects script filename
									0,							// D3D shader macro
									D3D_COMPILE_STANDARD_FILE_INCLUDE,							// additional include files
									//shaderVersion.c_str(),		// shader version
									shaderFlags,				// shader flags
									0,							// FX flags
									md3dDevice,					// D3D device
									//0,							// effect pool
									//0,							// thread pump
									&mFX,						// effect
									&compilationErrors);			// errors (if any)
									//0);							// result ptr

	// @TODO: remove when done testing
	//hr = D3DX10CreateEffectFromFile(mShaderFilename.c_str(),	// effects script filename
	//	0,							// D3D10 shader macro
	//	0,							// additional include files
	//	shaderVersion.c_str(),		// shader version
	//	shaderFlags,				// shader flags
	//	0,							// FX flags
	//	md3dDevice,					// D3D device
	//	0,							// effect pool
	//	0,							// thread pump
	//	&mFX,						// effect
	//	&compilationErrors,			// errors (if any)
	//	0);							// result ptr


	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILEW__, (DWORD)__LINE__, hr, L"D3DX11CompileEffectFromFile", true);
	}

	// get shader variables here
	getShaderVariables();
}

void GenericApp::buildVertexLayouts()
{
	// get the pass description
	D3DX11_PASS_DESC PassDesc;
	mTech->GetPassByIndex(0)->GetDesc(&PassDesc);

	// get the vertex elements description and no. of elements of the vertex buffer corresponding to mPrimaryVertexType
	D3D11_INPUT_ELEMENT_DESC *vertexElementsDesc = NULL;
	UINT nElements;
	GetVertexDesc(mPrimaryVertexType, &vertexElementsDesc, nElements);

	// create the input assembler vertex layout
	HR(D3DApp::md3dDevice->CreateInputLayout(vertexElementsDesc, nElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &mVertexLayout));

	// this information is already available in Vertex.h, shouldn't be duplicated here
#if 0
	// define vertex descriptions for all vertex types
	UINT nSimpleVertex = 2;
	D3D11_INPUT_ELEMENT_DESC simpleVertexDesc[] = 
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

		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}	
	};

	UINT nComplexVertex = 6;
	D3D11_INPUT_ELEMENT_DESC complexVertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	UINT nLightVertex = 4;
	D3D11_INPUT_ELEMENT_DESC lightVertexDesc[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"DIFFUSE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	UINT nTextureVertex = 3;
	D3D11_INPUT_ELEMENT_DESC textureVertexDesc[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// create the input layout based on mPrimaryVertexType
	switch(mPrimaryVertexType)
	{
	case SIMPLE_VERTEX:
		HR(md3dDevice->CreateInputLayout(simpleVertexDesc,nSimpleVertex,PassDesc.pIAInputSignature,
			PassDesc.IAInputSignatureSize,&mVertexLayout));
		break;

	case COMPLEX_VERTEX:
		HR(md3dDevice->CreateInputLayout(complexVertexDesc,nComplexVertex,PassDesc.pIAInputSignature,
			PassDesc.IAInputSignatureSize,&mVertexLayout));
		break;

	case LIGHT_VERTEX:
		HR(md3dDevice->CreateInputLayout(lightVertexDesc,nLightVertex,PassDesc.pIAInputSignature,
			PassDesc.IAInputSignatureSize,&mVertexLayout));
		break;

	case TEXTURE_VERTEX:
		HR(md3dDevice->CreateInputLayout(textureVertexDesc,nTextureVertex,PassDesc.pIAInputSignature,
			PassDesc.IAInputSignatureSize,&mVertexLayout));
		break;
	}
#endif	// 0
}

//#ifdef _DEBUG
//#pragma comment(lib, "DXEngined.lib")
//#else
//#pragma comment(lib, "DXEngine.lib")
//#endif	// _DEBUG

