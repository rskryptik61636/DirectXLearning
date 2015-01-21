// entry point for the MirrorApp demo

#include "MirrorApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	std::wstring strSceneFilePath = L"";	// @TODO: add a path to an actual scene description file once the functionality has been implemented
	MirrorApp mirrorApp(hInstance, strSceneFilePath);
	mirrorApp.initApp();
	return mirrorApp.run();
}