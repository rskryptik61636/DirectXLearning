// header file for the DXApp class

#ifndef DXAPP_H
#define DXAPP_H

// header of the base class D3DApp
#include "d3dApp.h"

// headers of the Light and Vertex utility classes
#include "Light.h"
#include "Vertex.h"

// header of the camera class
#include "DXCamera.h"

// header of the SceneBuilder class
#include "SceneBuilder.h"

// vector class def
#include <vector>

// Base class for a DirectX application, extend to implement quick and dirty demos
// The following protected methods need to be implemented:
//	createResources()
//	createObjects()
//	drawObjects()
//	buildShaders()
//	buildVertexLayout()
class DXApp : public D3DApp
{
public:

	// Parameterized ctor, creates a DirectX app given the hInstance and the path to a scene description file
	// @param: hInstance = handle to the Windows app instance
	// @param: strSceneFilePath = absolute path to the scene description file (TODO: decide on the scene description file format)
	DXApp(HINSTANCE hInstance, const std::string strSceneFilePath);

	// Virtual destructor
	virtual ~DXApp();

	// Initializes the DirectX application's objects and resources
	virtual void initApp();

	// Called when the client window is resized
	virtual void onResize();

	// Called when Windows events have to be processed and there is something that the application might have to take care off
	// @param: dt = delta time interval of the mGameTimer member
	virtual void updateScene(float dt);

	// Renders the scene
	virtual void drawScene();

	// not really useful at this point
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
	virtual void createResources() = 0;

	// Define scene objects which need to be rendered
	virtual void createObjects() = 0;

	// Meat of the drawScene method, this is where the scene rendering is implemented
	virtual void drawObjects() = 0;

	// Define to build the shaders which will be used by the application
	virtual void buildShaders() = 0;

	// Define to build the vertex layout which will be bound to the input assembly stage
	virtual void buildVertexLayouts() = 0;

	// Initializes the camera using the given params
	virtual void initCamera();

	// Handles the camera motion
	virtual void moveCamera(float dt);

	// Utility function to read the shader's compiled bytecode into the given buffer
	// source: http://gamedev.stackexchange.com/questions/49197/loading-a-vertex-shader-compiled-by-visual-studio-2012
	bool getShaderByteCode(const std::wstring strShaderPath, BlobPtr &shaderBuf);

	// TODO: remove when done testing as this has been moved to DXEffect
	// Utility function to construct an input layout using reflection
	// Source: http://takinginitiative.wordpress.com/2011/12/11/directx-1011-basic-shader-reflection-automatic-input-layout-creation/
	void createInputLayoutFromShaderInfo(ID3DBlob* pShaderBlob, InputLayoutPtr &pInputLayout);

	// Utility function to create a shader constant buffer
	void createConstantBuffer(const UINT bufSize, BufferPtr &pBuf);

	// Utility function to get the info about a given constant buffer in a shader
	void getShaderConstBufInfo(ID3DBlob *pShaderBlob, const std::string &bufName, D3D11_SHADER_BUFFER_DESC &constBufDesc, ShaderCBVariableInfoList &cbVarInfoList);

	// @TODO: figure this out once we have a better idea of how to init shader constant buffers dynamically
#if 0
	// Utility function to get a list of constant buffers that are defined in the given shader
	// Adapted from: http://members.gamedev.net/JasonZ/Heiroglyph/D3D11ShaderReflection.pdf
	void getShaderConstantBuffers(ID3DBlob *pShaderBlob, std::vector<ShaderConstantBuffer1> &constantBuffers);
#endif	// 0

	// Specifies whether the scene file has been provided
	inline bool sceneFileAvailable()	{ return !m_strSceneFilePath.empty(); }

	// @TODO: add protected methods here

protected:

	InputLayoutPtr m_pVertexLayout;	// Smart pointer to the vertex buffer layout which will be initialized and bound to the input assembly stage

	std::string m_strSceneFilePath;	// Absolute path to the scene description file

	SceneBuilderPtr m_pSceneBuilder;	// SceneBuilder class object

	DXCameraPtr m_pCamera;				// Camera

	DXVector2 m_lastMousePos;

	std::string m_strSceneName;	// Name of the current scene

	ToggleState m_runToggler;

	// @TODO: add protected members here

};	// end of class DXApp

#endif	// DXAPP_H