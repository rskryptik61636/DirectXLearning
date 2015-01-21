// Source file of the SemiReflectiveWindowApp class.

#include "SemiReflectiveWindowApp.h"

// param ctor
// TODO: Initialize additional members as necessary
SemiReflectiveWindowApp::SemiReflectiveWindowApp(HINSTANCE hInstance, const std::string strSceneFilePath) : DXApp(hInstance, strSceneFilePath)
{
}

// dtor
SemiReflectiveWindowApp::~SemiReflectiveWindowApp()	{}

// initializes the DirectX application's objects and resources
void SemiReflectiveWindowApp::initApp()
{
	DXApp::initApp();

	// NOTE: This is what goes on in DXApp::initApp(). Left here as a quick reference.
#if 0
	mClearColor = BLACK; // DXColor(0.0f, 0.0f, 0.0f, 1.0f);	// set clear color to black

	// init the SceneBuilder object
	if (sceneFileAvailable())
	{
		m_pSceneBuilder.reset(new SceneBuilder(m_strSceneFilePath));
	}
	m_strSceneName = m_pSceneBuilder->getSceneName();

	// init the camera
	initCamera();

	// build shader effects, vertex layouts and initialize all resources
	buildShaders();
	buildVertexLayouts();
	createResources();

	// create and initialize scene objects
	createObjects();
#endif // 0

	// TODO: Add implementation here.

}

// called when the client window is resized
void SemiReflectiveWindowApp::onResize()
{
	DXApp::onResize();

	// TODO: Add implementation here.	
}

// called when Windows events have to be processed and there is something that the application might have to take care off
void SemiReflectiveWindowApp::updateScene(float dt)
{
	DXApp::updateScene(dt);

	// TODO: Add implementation here.
}

// renders the scene
void SemiReflectiveWindowApp::drawScene()
{
	DXApp::drawScene();

	// TODO: Add implementation here.
}

// Define resources such as textures, blend/rasterizer/depth-stencil states and so on
void SemiReflectiveWindowApp::createResources()
{
	// TODO: Add implementation here.
}

// Define scene objects which need to be rendered.
void SemiReflectiveWindowApp::createObjects()
{
	// TODO: Add implementation here.
}

// Meat of the drawScene method, this is where the scene rendering is implemented.
void SemiReflectiveWindowApp::drawObjects()
{
	// TODO: Add implementation here.
}

// Define to build the shaders which will be used by the application.
void SemiReflectiveWindowApp::buildShaders()
{
	// TODO: Add implementation here.
}

// Define to build the vertex layout which will be bound to the input assembly stage
void SemiReflectiveWindowApp::buildVertexLayouts() 
{
	// TODO: Add implementation here.
}

// TODO: Re-implement iff necessary.
#if 0
// initializes the camera
void SemiReflectiveWindowApp::initCamera()
{
	// declare the camera params
	DXVector3 eyePos(0.0f, 20.0f, -50.0f), lookAt(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f);
	float FOVAngle(0.25f * PI);
	float nearPlane(1.0f), farPlane(1000.0f);
	float aspectRatio((float)mClientWidth / (float)mClientHeight);
	float motionFactor(50.0f), rotationFactor(1.0f);

	// read the camera params from the scene file if it has been provided, else use the default params
	if (sceneFileAvailable())
	{
		m_pSceneBuilder->buildCamera(m_pCamera, aspectRatio, eyePos, lookAt, up, FOVAngle, nearPlane, farPlane, motionFactor, rotationFactor);
	}
	else
	{
		m_pCamera.reset(new DXCamera(eyePos, lookAt, up, FOVAngle, aspectRatio, nearPlane, farPlane, motionFactor, rotationFactor));
	}
}

// handles the camera motion
void SemiReflectiveWindowApp::moveCamera(float dt)
{
	// define keyboard input to rotate camera and to zoom in/out
	/*const*/ float motionScaleFactor = m_pCamera->motionFactor();
	/*const*/ float rotFactor = m_pCamera->rotationFactor();

	if (m_runToggler.getState())
	{
		motionScaleFactor *= 4.0f;	// run
		//rotFactor *= 2.0f;
	}

	if (GetAsyncKeyState('A') & 0x8000)	m_pCamera->slide(-motionScaleFactor * dt, 0, 0);	// move left	
	if (GetAsyncKeyState('D') & 0x8000)	m_pCamera->slide(motionScaleFactor * dt, 0, 0);	// move right
	if (GetAsyncKeyState('W') & 0x8000)	m_pCamera->slide(0, 0, motionScaleFactor * dt);		// move forward
	if (GetAsyncKeyState('S') & 0x8000)	m_pCamera->slide(0, 0, -motionScaleFactor * dt);		// move backward
	if (GetAsyncKeyState('Q') & 0x8000)	m_pCamera->slide(0, motionScaleFactor * dt, 0);			// move up
	if (GetAsyncKeyState('E') & 0x8000)	m_pCamera->slide(0, -motionScaleFactor * dt, 0);			// move down

	if (GetAsyncKeyState(VK_NUMPAD4) & 0x8000)	m_pCamera->rotateY(-rotFactor * dt);	// yaw left
	if (GetAsyncKeyState(VK_NUMPAD6) & 0x8000)	m_pCamera->rotateY(rotFactor * dt);	// yaw right
	if (GetAsyncKeyState(VK_NUMPAD8) & 0x8000)	m_pCamera->pitch(-rotFactor * dt);		// pitch up
	if (GetAsyncKeyState(VK_NUMPAD5) & 0x8000)	m_pCamera->pitch(rotFactor * dt);		// pitch down

	// NOTE: Disabling rolls as we don't really need them.
#if 0
	if (GetAsyncKeyState(VK_NUMPAD7) & 0x8000)	m_pCamera->roll(rotFactor * dt);			// roll left
	if (GetAsyncKeyState(VK_NUMPAD9) & 0x8000)	m_pCamera->roll(-rotFactor * dt);			// roll right
#endif	// 0

	// zoom in/out according to the keyboard input
	if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000)	m_pCamera->zoomOut();	// zoom out
	if (GetAsyncKeyState(VK_NUMPAD3) & 0x8000)	m_pCamera->zoomIn();	// zoom in
}
#endif // 0

// Not really useful at this point. Use if deemed necessary.
#if 0
// Convenience overrides for handling mouse input.
void SemiReflectiveWindowApp::OnMouseDown(WPARAM btnState, int x, int y)	// {}
{
	// update the last mouse position
	m_lastMousePos.x = static_cast<float>(x);
	m_lastMousePos.y = static_cast<float>(y);

	// set the D3D window to capture mouse events
	SetCapture(mhMainWnd);
}

void SemiReflectiveWindowApp::OnMouseUp(WPARAM btnState, int x, int y) // {}
{
	// release the D3D window from capturing mouse events
	ReleaseCapture();
}

void SemiReflectiveWindowApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	// only move the camera if the LMB is pressed
	if (btnState & MK_LBUTTON != 0)
	{
		// compute the current x and y offset relative to the last known mouse position
		const float dx = DirectX::XMConvertToRadians(m_pCamera->rotationFactor() * (m_lastMousePos.x - static_cast<float>(x)));
		const float dy = DirectX::XMConvertToRadians(m_pCamera->rotationFactor() * (m_lastMousePos.y - static_cast<float>(y)));

		// pitch the camera by dy and yaw the camera by dx
		m_pCamera->pitch(-dy);
		m_pCamera->rotateY(-dx);
	}
	
	// update the last known mouse position
	m_lastMousePos.x = x;
	m_lastMousePos.y = y;
}
#endif	// 0