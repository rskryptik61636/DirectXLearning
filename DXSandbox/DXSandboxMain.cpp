// entry point for the sandbox app

#include "DXSandboxApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	const std::string strSceneFilePath(cmdLine);	// @TODO: fix this to account for "" at the beginning and end of the string
	DXSandboxApp sandbox(hInstance, strSceneFilePath);
	sandbox.initApp();
	return sandbox.run();
}