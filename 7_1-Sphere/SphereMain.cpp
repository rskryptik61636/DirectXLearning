// entry point for SphereApp1

#include "SphereApp1.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int showCmd)
{
			// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// init the shader filename
	std::string shaderRoot = std::string(getenv("DX_LEARNING_ROOT")) + "shaders\\";
	std::wstring shaderFilename(shaderRoot.begin(), shaderRoot.end());// = std::wstring() + L"\\color.fx";
	//std::copy(shaderRoot.begin(), shaderRoot.end(), shaderFilename.begin());
	shaderFilename += L"sphereShader.fx"; /*L"lighting.fx";*/

	// instantiate the SphereApp1 object and run
	SphereApp1 sphereApp1(hInstance, shaderFilename);
	sphereApp1.initApp();
	return sphereApp1.run();
}