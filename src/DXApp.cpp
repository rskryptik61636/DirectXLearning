// source file of the DXApp class

#include "DXApp.h"

// param ctor
DXApp::DXApp(HINSTANCE hInstance, const std::string strSceneFilePath) : D3DApp(hInstance), m_strSceneFilePath(strSceneFilePath), 
m_pVertexLayout(), m_lastMousePos(0, 0), m_runToggler(L"Run", VK_CAPITAL)
{
}

// dtor
DXApp::~DXApp()	{}

// initializes the DirectX application's objects and resources
void DXApp::initApp()
{
	D3DApp::initApp();

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
}

// called when the client window is resized
void DXApp::onResize()
{
	D3DApp::onResize();

	// update the aspect ratio of the camera iff it has been defined yet
	if (m_pCamera)
		m_pCamera->setAspectRatio((float)mClientWidth / (float)mClientHeight);
}

// called when Windows events have to be processed and there is something that the application might have to take care off
void DXApp::updateScene(float dt)
{
	D3DApp::updateScene(dt);

	// Query the 'run' toggle state
	m_runToggler.toggleState();

	// move/rotate the camera according to the keyboard input
	moveCamera(dt);
}

// renders the scene
void DXApp::drawScene()
{
	D3DApp::drawScene();

	// draw objects
	drawObjects();

	// draw frame stats
	mSpriteBatch->Begin();
	mFont->DrawString(mSpriteBatch.get(), mFrameStats.c_str(), DirectX::XMFLOAT2(0, 0));
	mSpriteBatch->End();

	// swap the front and back buffers
	mSwapChain->Present(0, 0);
}

// initializes the camera
void DXApp::initCamera()
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
void DXApp::moveCamera(float dt)
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

// Utility class to read the shader's compiled bytecode into the given buffer
// source: http://gamedev.stackexchange.com/questions/49197/loading-a-vertex-shader-compiled-by-visual-studio-2012
bool DXApp::getShaderByteCode(const std::wstring strShaderPath, BlobPtr &shaderBuf)
{
	std::ifstream shaderStream;
	std::size_t bufSize;
	//char* vs_data;

	shaderStream.open(strShaderPath, std::ifstream::in | std::ifstream::binary);
	if (shaderStream.good())
	{
		shaderStream.seekg(0, std::ios::end);
		bufSize = size_t(shaderStream.tellg());
		//shaderBuf.resize(bufSize);
		HRESULT hr = D3DCreateBlob(bufSize, &shaderBuf.p);
		if (hr != S_OK)
			return false;

		shaderStream.seekg(0, std::ios::beg);
		shaderStream.read((char*)shaderBuf->GetBufferPointer(), bufSize);
		shaderStream.close();
	}
	else
	{
		return false;
	}

	return true;
}

// Utility function to construct an input layout using reflection
// Source: http://takinginitiative.wordpress.com/2011/12/11/directx-1011-basic-shader-reflection-automatic-input-layout-creation/
void DXApp::createInputLayoutFromShaderInfo(ID3DBlob* pShaderBlob, InputLayoutPtr &pInputLayout)
{
	// Reflect shader info
	ShaderReflectionPtr pVertexShaderReflection;
	HR(D3DReflect(pShaderBlob->GetBufferPointer(),						// pointer to shader bytecode buffer
				pShaderBlob->GetBufferSize(),							// size in bytes of shader bytecode buffer
				IID_ID3D11ShaderReflection,								// GUID of the shader reflection interface
				reinterpret_cast<void**>(&pVertexShaderReflection.p)));	// pointer to shader reflection interface

	// Get shader info
	D3D11_SHADER_DESC shaderDesc;
	pVertexShaderReflection->GetDesc(&shaderDesc);

	// Read input layout description from shader info
	UINT byteOffset = 0;
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (UINT i = 0; i< shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc);

		// fill out input element desc
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = byteOffset;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		// determine DXGI format
		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			byteOffset += 4;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			byteOffset += 8;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			byteOffset += 12;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			byteOffset += 16;
		}

		//save element desc
		inputLayoutDesc.push_back(elementDesc);
	}

	// Try to create Input Layout
	HR(D3DApp::md3dDevice->CreateInputLayout(inputLayoutDesc.data(),			// base address of input layout desc array
											inputLayoutDesc.size(),				// no. of elements in input layout desc array
											pShaderBlob->GetBufferPointer(),	// pointer to shader bytecode buffer
											pShaderBlob->GetBufferSize(),		// size of shader bytecode buffer in bytes
											&pInputLayout.p));				// input vertex layout interface

	//Free allocation shader reflection memory
	pVertexShaderReflection.Release();
	//return hr;
}

// Utility function to create a shader constant buffer
void DXApp::createConstantBuffer(const UINT bufSize, BufferPtr &pBuf)
{
	D3D11_BUFFER_DESC vsBufDesc;
	vsBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// bind as a constant buffer
	vsBufDesc.ByteWidth = bufSize;						// buffer size
	vsBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// we're going to be mapping this buffer to app memory every time its contents need to be updated
	vsBufDesc.MiscFlags = 0;							// no misc flags
	vsBufDesc.Usage = D3D11_USAGE_DYNAMIC;				// it is going to be updated on the fly
	HR(D3DApp::md3dDevice->CreateBuffer(&vsBufDesc, 0, &pBuf.p));	// create the buffer
}

// @TODO: remove when done testing as this now lives in DXEffect
// Utility function to get the info about a given constant buffer in a shader
void DXApp::getShaderConstBufInfo(ID3DBlob *pShaderBlob, const std::string &bufName, D3D11_SHADER_BUFFER_DESC &constBufDesc, ShaderCBVariableInfoList &cbVarInfoList)
{
	// reflect the shader info and get the requested constant buffer
	ShaderReflectionPtr pShaderReflection;
	HR(D3DReflect(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&pShaderReflection)));
	ID3D11ShaderReflectionConstantBuffer *pConstBuf = pShaderReflection->GetConstantBufferByName(bufName.c_str());
	HR(pConstBuf->GetDesc(&constBufDesc));

	// get the info of all the variables listed in cbVarInfoList
	for (std::size_t i = 0; i < cbVarInfoList.size(); ++i)
	{
		ID3D11ShaderReflectionVariable *pVar = pConstBuf->GetVariableByName(cbVarInfoList[i].first.c_str());
		D3D11_SHADER_VARIABLE_DESC varDesc;
		HR(pVar->GetDesc(&varDesc));
		cbVarInfoList[i].second = varDesc;
	}
}

// @TODO: figure this out once we have a better idea of how to init shader constant buffers dynamically
#if 0
// Utility function to get a list of constant buffers that are defined in the given shader
// Adapted from: http://members.gamedev.net/JasonZ/Heiroglyph/D3D11ShaderReflection.pdf
void DXApp::getShaderConstantBuffers(ID3DBlob *pShaderBlob, std::vector<ShaderConstantBuffer1> &constantBuffers)
{
	// get the shader information using reflection
	ShaderReflectionPtr pShaderReflection;
	HR(D3DReflect(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&pShaderReflection.p)));
	D3D11_SHADER_DESC shaderDesc;
	pShaderReflection->GetDesc(&shaderDesc);

	// populate the constant buffers array with the information about each constant buffer
	const UINT nConstBufs = shaderDesc.ConstantBuffers;
	constantBuffers.resize(static_cast<std::size_t>(nConstBufs));
	for (UINT i = 0; i < nConstBufs; ++i)
	{
		// get the current shader buffer description
		ID3D11ShaderReflectionConstantBuffer *pConstBuf = pShaderReflection->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC bufDesc;
		pConstBuf->GetDesc(&bufDesc);
		constantBuffers[i].desc = bufDesc;
		//pConstBuf->GetDesc(&constantBuffers[i].desc);	// this doesn't work

		// populate the current shader buffer's list of variables
		const UINT nVars = constantBuffers[i].desc.Variables;
		constantBuffers[i].variables.resize(nVars);
		for (UINT j = 0; j < nVars; ++j)
		{
			// get the current variable as well as its type's description
			ID3D11ShaderReflectionVariable *pVar = pConstBuf->GetVariableByIndex(j);
			D3D11_SHADER_VARIABLE_DESC varDesc;
			pVar->GetDesc(&varDesc);
			constantBuffers[i].variables[j].first = varDesc;
			//pVar->GetDesc(&constantBuffers[i].variables[j].first);	// this doesn't work

			ID3D11ShaderReflectionType *pType = pVar->GetType();
			D3D11_SHADER_TYPE_DESC typeDesc;
			pType->GetDesc(&typeDesc);
			constantBuffers[i].variables[j].second = typeDesc;
			//pType->GetDesc(&constantBuffers[i].variables[j].second);	// this doesn't work
		}
	}

	// release the shader reflection interface
	pShaderReflection.Release();
}
#endif	// 0

// not really useful at this point
#if 0
// Convenience overrides for handling mouse input.
void DXApp::OnMouseDown(WPARAM btnState, int x, int y)	// {}
{
	// update the last mouse position
	m_lastMousePos.x = static_cast<float>(x);
	m_lastMousePos.y = static_cast<float>(y);

	// set the D3D window to capture mouse events
	SetCapture(mhMainWnd);
}

void DXApp::OnMouseUp(WPARAM btnState, int x, int y) // {}
{
	// release the D3D window from capturing mouse events
	ReleaseCapture();
}

void DXApp::OnMouseMove(WPARAM btnState, int x, int y)
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