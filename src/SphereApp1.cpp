// source file for SphereApp1

#include "SphereApp1.h"
#include "BasicSphere.h"
#include "LitSphere.h"

// @TODO: remove when done testing
//// link to DXEngine library
//#ifdef _DEBUG
//#pragma comment(lib, "DXEngined.lib")
//#else
//#pragma comment(lib, "DXEngine.lib")
//#endif	// _DEBUG

// param ctor 1
SphereApp1::SphereApp1(HINSTANCE hInstance, std::wstring shaderFilename) : GenericApp(hInstance, shaderFilename)
{
	//mSphereWorldMatrix.Identity();	// @TODO: put back when done testing
	mSphereWorldMatrix = DXMatrix::CreateScale(10.0f);
}

// dtor
SphereApp1::~SphereApp1()
{
	// clear D3D device state
	if (GenericApp::md3dDeviceContext)
		GenericApp::md3dDeviceContext->ClearState();

	//mSphere.reset();	// reset sphere ptr
	mSphere2.reset();	// reset sphere ptr
}

// create resources
void SphereApp1::createResources()
{
	// initialize scene light
	DXVector3 lightDirection(0.5f, -0.5f, 0.5f);
	DXColor ambientLightColour(0.2f, 0.2f, 0.2f, 1.0f);
	mParallelLight.ambient = ambientLightColour;
	mParallelLight.dir = lightDirection;
	mParallelLight.diffuse = mParallelLight.specular = WHITE;
}

// create objects
void SphereApp1::createObjects()
{
	// initialize sphere
	/*mSphere.reset(new BasicSphere());
	mSphere->init(GenericApp::md3dDevice, 10.0f);*/

	mSphere2.reset(new LitSphere());
	mSphere2->init(GenericApp::md3dDevice, 1.0f);

	// initialize vertex type
	GenericApp::mPrimaryVertexType = LIGHT_VERTEX; // SIMPLE_VERTEX;
}

// draw objects
void SphereApp1::drawObjects()
{
	// TODO: remove when done testing
	//md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	// get the shader variables for the sphere technique
	getShaderVariables();

	// get the technique desc
	D3DX11_TECHNIQUE_DESC techDesc;
	GenericApp::mTech->GetDesc(&techDesc);

	// draw the sphere for each pass of the technique
	for(UINT i = 0; i < techDesc.Passes; ++i)
	{
		ID3DX11EffectPass *currPass = GenericApp::mTech->GetPassByIndex(i);

		GenericApp::mWVP = mSphereWorldMatrix * GenericApp::mView * GenericApp::mProj;
		mfxWVPVar->SetMatrix((float*)&(GenericApp::mWVP));
		mfxWorldVar->SetMatrix((float*)&(mSphereWorldMatrix));
		currPass->Apply(0, md3dDeviceContext);
		mSphere2->draw();
	}

	// get the shader variables for the normals shader technique
	getShaderVariables2();

	// set the shader variables for the normals effect
	setShaderVariables2();

	// set the primitive topology to points
	//md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// get the normals technique desc
	mTech->GetDesc(&techDesc);

	// draw the sphere for each pass of the normals technique
	for (UINT i = 0; i < techDesc.Passes; ++i)
	{
		ID3DX11EffectPass *currPass = GenericApp::mTech->GetPassByIndex(i);

		GenericApp::mWVP = mSphereWorldMatrix * GenericApp::mView * GenericApp::mProj;
		mfxWVPVar->SetMatrix((float*)&(GenericApp::mWVP));
		mfxWorldVar->SetMatrix((float*)&(mSphereWorldMatrix));
		currPass->Apply(0, md3dDeviceContext);
		mSphere2->draw();
	}
}

// get shader variables
void SphereApp1::getShaderVariables()
{
	// store the shader variable names to avoid mishaps
	std::string techniqueName = "SphereTech"; /*"LightTech";*/ /*"ColorTech";*/
	std::string wvpName = "gWVP";
	std::string worldName = "gWorld";
	//std::string textureName = "gTextureMatrix";
	std::string eyePosName = "gEyePosW";
	std::string lightName = "gLight";
	/*std::string diffuseMapName = "gDiffuseMap";
	std::string specularMapName = "gSpecularMap";*/
	std::string lightTypeName = "gLightType";
	std::string shaderImplTypeName = "gShaderImplType";
	std::string currTimeName = "gCurrTime";

	// get the technique 
	GenericApp::mTech = GenericApp::mFX->GetTechniqueByName(techniqueName.c_str());

	// get the WVP and World Matrix shader variables
	mfxWVPVar = GenericApp::mFX->GetVariableByName(wvpName.c_str())->AsMatrix();
	mfxWorldVar = GenericApp::mFX->GetVariableByName(worldName.c_str())->AsMatrix();
	mfxEyePosVar = GenericApp::mFX->GetVariableByName(eyePosName.c_str());
	mfxLightVar = GenericApp::mFX->GetVariableByName(lightName.c_str());
	mfxLightTypeVar = GenericApp::mFX->GetVariableByName(lightTypeName.c_str())->AsScalar();
	mfxShaderImplTypeVar = GenericApp::mFX->GetVariableByName(shaderImplTypeName.c_str())->AsScalar();
	mfxCurrTimeVar = GenericApp::mFX->GetVariableByName(currTimeName.c_str())->AsScalar();

	// @TODO: add implementation here
}

// get shader variables
void SphereApp1::getShaderVariables2()
{
	// store the shader variable names to avoid mishaps
	std::string techniqueName = "NormalsTech"; /*"LightTech";*/ /*"ColorTech";*/
	std::string wvpName = "gWVP";
	std::string worldName = "gWorld";
	//std::string textureName = "gTextureMatrix";
	std::string eyePosName = "gEyePosW";
	std::string lightName = "gLight";
	/*std::string diffuseMapName = "gDiffuseMap";
	std::string specularMapName = "gSpecularMap";*/
	std::string lightTypeName = "gLightType";
	std::string shaderImplTypeName = "gShaderImplType";
	std::string currTimeName = "gNormalLength";

	// get the technique 
	GenericApp::mTech = mFX2->GetTechniqueByName(techniqueName.c_str());

	// get the WVP and World Matrix shader variables
	mfxWVPVar = mFX2->GetVariableByName(wvpName.c_str())->AsMatrix();
	mfxWorldVar = mFX2->GetVariableByName(worldName.c_str())->AsMatrix();
	mfxEyePosVar = mFX2->GetVariableByName(eyePosName.c_str());
	mfxLightVar = mFX2->GetVariableByName(lightName.c_str());
	mfxLightTypeVar = mFX2->GetVariableByName(lightTypeName.c_str())->AsScalar();
	mfxShaderImplTypeVar = mFX2->GetVariableByName(shaderImplTypeName.c_str())->AsScalar();
	mfxCurrTimeVar = mFX2->GetVariableByName(currTimeName.c_str())->AsScalar();

	// @TODO: add implementation here
}

// set shader variables
void SphereApp1::setShaderVariables()
{
	int nPhong(0), nVertShader(1);

	mfxEyePosVar->SetRawValue(&(GenericApp::mEyePos), 0, sizeof(DXVector3));
	mfxLightVar->SetRawValue(&mParallelLight, 0, sizeof(Light));
	mfxLightTypeVar->SetInt(nPhong);
	mfxShaderImplTypeVar->SetInt(nVertShader);
	mfxCurrTimeVar->SetFloat(mTimer.getGameTime()/100.0f);

	// @TODO: add implementation here
}

// set shader variables for the normals effect
void SphereApp1::setShaderVariables2()
{
	int nPhong(0), nVertShader(1);

	mfxEyePosVar->SetRawValue(&(GenericApp::mEyePos), 0, sizeof(DXVector3));
	mfxLightVar->SetRawValue(&mParallelLight, 0, sizeof(Light));
	mfxLightTypeVar->SetInt(nPhong);
	mfxShaderImplTypeVar->SetInt(nVertShader);
	mfxCurrTimeVar->SetFloat(1.0f);	// normal length actually

	// @TODO: add implementation here
}

// override the buildFX virtual func to compile the normals effect in addition to the default effect
void SphereApp1::buildFX()
{
	// invoke the base class version to compile the default shader
	GenericApp::buildFX();

	// compile the normals effect
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif	// _DEBUG

	ID3DBlob *compilationErrors = 0;
	HRESULT hr = 0;
	std::string shaderVersion("fx_5_0");
	//ID3DInclude *pInclude;
	const std::wstring normalsFXPath(L"N:\\DirectXLearning\\shaders\\normalShader.fx");
	hr = D3DX11CompileEffectFromFile(normalsFXPath.c_str(),	// effects script filename
		0,							// D3D shader macro
		D3D_COMPILE_STANDARD_FILE_INCLUDE,							// additional include files
		//shaderVersion.c_str(),		// shader version
		shaderFlags,				// shader flags
		0,							// FX flags
		md3dDevice,					// D3D device
		//0,							// effect pool
		//0,							// thread pump
		&mFX2,						// effect
		&compilationErrors);			// errors (if any)
	//0);							// result ptr

	// @TODO: remove when done testing
	//hr = D3DX10CreateEffectFromFile(mShaderFilename.c_str(),	// effects script filename
	//	0,							// D3D10 shader macro
	//	0,							// additional include files
	//	shaderVersion.c_str(),		// shader version
	//	shaderFlags,				// shader flags
	//	0,							// FX flags
	//	md3dDevice,					// D3D device
	//	0,							// effect pool
	//	0,							// thread pump
	//	&mFX,						// effect
	//	&compilationErrors,			// errors (if any)
	//	0);							// result ptr


	if (FAILED(hr))
	{
		if (compilationErrors)
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILEW__, (DWORD)__LINE__, hr, L"D3DX11CompileEffectFromFile", true);
	}
}