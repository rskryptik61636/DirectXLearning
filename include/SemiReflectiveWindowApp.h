// Header file for the SemiReflectiveWindowApp class.

#ifndef SEMIREFLECTIVEWINDOWAPP_H
#define SEMIREFLECTIVEWINDOWAPP_H

// header of the base class DXApp
#include "DXApp.h"

// need the shapes classes
#include "DXShapes.h"

// TODO: Add additional headers here.

// App which implements a window that is mostly transparent but also slightly reflective in that it posesses slight mirror like properties.
class SemiReflectiveWindowApp : public DXApp
{
public:

	// Parameterized ctor, creates a DirectX app given the hInstance and the path to a scene description file
	// @param: hInstance = handle to the Windows app instance
	// @param: strSceneFilePath = absolute path to the scene description file (TODO: decide on the scene description file format)
	SemiReflectiveWindowApp(HINSTANCE hInstance, const std::string strSceneFilePath);

	// Virtual destructor
	virtual ~SemiReflectiveWindowApp();

	// Initializes the DirectX application's objects and resources
	/*virtual*/ void initApp();

	// Called when the client window is resized
	/*virtual*/ void onResize();

	// Called when Windows events have to be processed and there is something that the application might have to take care off
	// @param: dt = delta time interval of the mGameTimer member
	/*virtual*/ void updateScene(float dt);

	// Renders the scene
	/*virtual*/ void drawScene();

	// Not really useful at this point. Use if deemed necessary.
#if 0
	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y);	// {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y); // {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y); // {}
#endif	 // 0

	// @TODO: add public methods here

public:
	// @TODO: add public members here

protected:

	// Define resources such as textures, blend/rasterizer/depth-stencil states and so on
	// TODO: Implement this method.
	virtual void createResources() /*= 0*/;

	// Define scene objects which need to be rendered.
	// TODO: Implement this method.
	virtual void createObjects() /*= 0*/;

	// Meat of the drawScene method, this is where the scene rendering is implemented.
	// TODO: Implement this method.
	virtual void drawObjects() /*= 0*/;

	// Define to build the shaders which will be used by the application.
	// TODO: Implement this method.
	virtual void buildShaders() /*= 0*/;

	// Define to build the vertex layout which will be bound to the input assembly stage
	// TODO: Implement this method.
	virtual void buildVertexLayouts() /*= 0*/;

	// TODO: Re-implement iff necessary.
#if 0
	// Initializes the camera using the given params
	virtual void initCamera();

	// Handles the camera motion
	virtual void moveCamera(float dt);
#endif // 0

	// @TODO: add protected methods here

protected:

	// Room object.
	RoomV1Ptr m_pRoom;

	// Room textures.
	ShaderResourceViewPtr m_pCrateRV, m_pFloorRV, m_pMirrorRV, m_pSpecRV, m_pWallRV;

	// Light source.
	Light m_parallelLight;

	// @TODO: add protected members here

};	// end of class SemiReflectiveWindowApp

#endif	// SEMIREFLECTIVEWINDOWAPP_H