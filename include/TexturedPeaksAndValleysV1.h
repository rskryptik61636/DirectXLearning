// header file for TexturedPeaksAndValleysV1 which extends ObjectV2

#ifndef TEXTURED_PEAKS_AND_VALLEYS_V1_H
#define TEXTURED_PEAKS_AND_VALLEYS_V1_H

#include "ObjectV2.h"

class TexturedPeaksAndValleysV1 : public ObjectV2<TextureVertex>
{
public:
	TexturedPeaksAndValleysV1();
	TexturedPeaksAndValleysV1(DWORD nRows, DWORD nCols, float dx);
	virtual ~TexturedPeaksAndValleysV1();

	float getHeight(float x, float z)	const;

	// custom init function - this is the one what will be used (NOTE: may not need after all if we use an appropriate constructor)
	//virtual void init(ID3D11Device *device, DWORD m, DWORD n, float dx);

	// base class init function (NOTE: base class version will do)
	// virtual void init(ID3D11Device *device, float scale);//	{}

	// vertex and index buffer init functions
	virtual void initVertexBuffer();
	virtual void initIndexBuffer();

	// update function
	// virtual void update(float dt);

private:
	DWORD mNumRows;
	DWORD mNumCols;
	float mDX;
};

#endif	// TEXTURED_PEAKS_AND_VALLEYS_V1_H