// test d3d app - source file

#include "InitD3DApp.h"

InitD3DApp::InitD3DApp(HINSTANCE hInstance) : D3DApp(hInstance)
{	}

InitD3DApp::~InitD3DApp()
{
	if( md3dDevice )
		md3dDeviceContext->ClearState();
}

void InitD3DApp::initApp()
{
	D3DApp::initApp();
}

void InitD3DApp::onResize()
{
	D3DApp::onResize();
}

void InitD3DApp::updateScene(float dt)
{
	D3DApp::updateScene(dt);
}

void InitD3DApp::drawScene()
{
	D3DApp::drawScene();

	RECT R = {10, 5, 0, 0};
	mFont->DrawText(0,mFrameStats.c_str(),-1,&R,DT_NOCLIP,WHITE);

	mSwapChain->Present(0,0);
}
