// entry point for FogApp

#include "FogApp.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, 
	PSTR cmdLine, int showCmd)
{
		// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	std::wstring shaderFilename = /*L"E:\\DirectXLearning\\shaders\\ClipPixel.fx";*/ L"N:\\DirectXLearning\\shaders\\FogShader.fx";
	FogApp fogApp(hInstance, shaderFilename);
	fogApp.initApp();
	return fogApp.run();
}