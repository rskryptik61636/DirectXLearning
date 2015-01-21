// header file for FogApp which extends GenericApp

#ifndef FOG_APP_H
#define FOG_APP_H

#include "GenericApp.h"
#include "Waves.h"

// forward declare all Object classes
class TextureBox;
class TexturedPeaksAndValleysV1;
class TexturedWavesV1;

class FogApp : public GenericApp
{
public:
	FogApp(HINSTANCE hInstance, std::wstring shaderFilename);
	virtual ~FogApp();

	// only need to override updateScene
	virtual void updateScene(float dt);

protected:
	// declare pure virtual funcs which will be defined
	virtual void createResources();
	virtual void createObjects();
	virtual void drawObjects();

	virtual void getShaderVariables();
	virtual void setShaderVariables();

protected:
	// scene objects
	std::shared_ptr<TextureBox> mBox;
	std::shared_ptr<TexturedWavesV1> mWaves;	// @TODO: reinstate once performance test is done
	//std::shared_ptr<Waves> mWaves;
	std::shared_ptr<TexturedPeaksAndValleysV1> mTerrain;

	// scene light
	Light mParallelLight;

	// texture offset variable
	DXVector2 mWavesTextureOffset;

	// rasterizer and blend state
	RasterizerStatePtr mNoCullRasterizerState;
	BlendStatePtr mTransparentBlendState;

	// shader resource variables
	ShaderResourceViewPtr mTerrainResourceView;
	ShaderResourceViewPtr mWavesResourceView;
	ShaderResourceViewPtr mBoxResourceView;
	ShaderResourceViewPtr mDefaultSpecularResourceView;

	// effect variables
	EffectMatrixVariablePtr mWVPMatrixVariable;
	EffectMatrixVariablePtr mWorldMatrixVariable;
	EffectMatrixVariablePtr mTextureMatrixVariable;
	EffectVariablePtr mEyePositionVariable;
	EffectVariablePtr mLightVariable;
	EffectShaderResourceVariablePtr mDiffuseMapVariable;
	EffectShaderResourceVariablePtr mSpecularMapVariable;

	// scene objects' world matrix variables
	DXMatrix mBoxWorldMatrix;
	DXMatrix mWavesWorldMatrix;
	DXMatrix mTerrainWorldMatrix;
};

#endif	// FOG_APP_H