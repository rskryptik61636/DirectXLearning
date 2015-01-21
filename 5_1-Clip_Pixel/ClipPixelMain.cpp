// entry point for the ClipPixelApp

#include "ClipPixelApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// enable run time memory check for debug builds
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif	// _DEBUG

	const std::wstring shaderFilename = L"ClipPixel.fx";
	ClipPixelApp clipper(hInstance, shaderFilename);
	clipper.initApp();
	return clipper.run();
}