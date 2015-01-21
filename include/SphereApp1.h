// header for SphereApp1 which extends GenericApp

#ifndef SPHERE_APP_1_H
#define SPHERE_APP_1_H

#include "GenericApp.h"

// forward declare BasicSphere
class BasicSphere;
class LitSphere;

class SphereApp1 : public GenericApp
{
public:
	SphereApp1(HINSTANCE hInstance, std::wstring shaderFilename);	// param ctor 1
	~SphereApp1();	// dtor

protected:
	// define all pure virtual funcs
	void createResources();
	void createObjects();
	void drawObjects();

	void getShaderVariables();
	void getShaderVariables2();	// for the normals effect
	void setShaderVariables();
	void setShaderVariables2();	// for the normals effect

	// override the buildFX virtual func to compile the normals effect in addition to the default effect
	void buildFX();

protected:
	// object ptr
	std::shared_ptr<BasicSphere> mSphere;
	std::shared_ptr<LitSphere> mSphere2;

	// scene light
	Light mParallelLight;

	// sphere world matrix
	DXMatrix mSphereWorldMatrix;

	// shader variables
	EffectMatrixVariablePtr mfxWorldVar;
	EffectMatrixVariablePtr mfxWVPVar;
	EffectVariablePtr mfxEyePosVar;
	EffectVariablePtr mfxLightVar;
	EffectScalarVariablePtr mfxLightTypeVar;
	EffectScalarVariablePtr mfxShaderImplTypeVar;
	EffectScalarVariablePtr mfxCurrTimeVar;

	// normals effect
	EffectPtr mFX2;

	// @TODO: add additional variables here
};	// end of class SphereApp1

#endif	// SPHERE_APP_1_H