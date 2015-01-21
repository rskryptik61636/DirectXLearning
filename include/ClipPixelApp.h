// header file for ClipPixelApp which extends GenericApp

#ifndef CLIP_PIXEL_APP_H
#define CLIP_PIXEL_APP_H

#include "GenericApp.h"

// forward declare object classes
class TextureBox;
class TexturedPeaksAndValleysV1;
class TexturedWavesV1;

class ClipPixelApp : public GenericApp
{
public:
	ClipPixelApp(HINSTANCE hInstance);
	ClipPixelApp(HINSTANCE hInstance, std::wstring shaderFilename);
	virtual ~ClipPixelApp();

	// @TODO: override virtual base class funcs iff necessary
	virtual void updateScene(float dt);

protected:
	virtual void createResources();
	virtual void createObjects();
	virtual void drawObjects();

	virtual void setShaderVariables();
	virtual void getShaderVariables();

	// @TODO: override protected virtual base class funcs iff necessary

protected:

	// scene objects
	TextureBox *mBox;
	TexturedPeaksAndValleysV1 *mTerrain;
	TexturedWavesV1 *mWaves;

	// light variables
	Light mParallelLight;

	// shader variables
	DXVector2 mWavesTexOffset;	// waves texture offset

	// rasterizer and blend states
	/*ID3D11RasterizerState *mNoCullRS;
	ID3D11BlendState *mTransparentBS;*/
	RasterizerStatePtr mNoCullRS;
	BlendStatePtr mTransparentBS;

	// texture resource views
	/*ID3D11ShaderResourceView *mTerrainRV;
	ID3D11ShaderResourceView *mWavesRV;
	ID3D11ShaderResourceView *mBoxRV;
	ID3D11ShaderResourceView *mDefaultSpecRV;*/
	ShaderResourceViewPtr mTerrainRV;
	ShaderResourceViewPtr mWavesRV;
	ShaderResourceViewPtr mBoxRV;
	ShaderResourceViewPtr mDefaultSpecRV;

	// shader effects matrix variables
	ID3DX11EffectMatrixVariable *mfxWVPVar;
	ID3DX11EffectMatrixVariable *mfxWorldVar;
	ID3DX11EffectVariable *mfxEyePosVar;
	ID3DX11EffectVariable *mfxLightVar;
	ID3DX11EffectShaderResourceVariable *mfxDiffuseMapVar;
	ID3DX11EffectShaderResourceVariable *mfxSpecMapVar;
	ID3DX11EffectMatrixVariable *mfxTexMtxVar;

	// object specific world matrices
	DXMatrix mCrateWorld;
	DXMatrix mWavesWorld;
	DXMatrix mTerrainWorld;

};

#endif	// CLIP_PIXEL_APP_H