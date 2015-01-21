#include "TexturedPeaksAndValleysV1.h"

TexturedPeaksAndValleysV1::TexturedPeaksAndValleysV1() 
	: mNumRows(0), mNumCols(0), mDX(0), ObjectV2()	{}

TexturedPeaksAndValleysV1::TexturedPeaksAndValleysV1(DWORD nRows, DWORD nCols, float dx) 
	: mNumRows(nRows), mNumCols(nCols), mDX(dx), ObjectV2(nRows*nCols,(nRows-1)*(nCols-1)*2,3)
{
	// NOTE: following block for clarity's sake
	/*ObjectV2::mNumVertices = mNumRows * mNumCols;
	ObjectV2::mNumFaces = (mNumRows-1) * (mNumCols-1) * 2;
	ObjectV2::mNumVerticesPerFace = 3;*/
}

TexturedPeaksAndValleysV1::~TexturedPeaksAndValleysV1()	{}

float TexturedPeaksAndValleysV1::getHeight(float x, float z)	const
{
	return 0.3f*( z*sinf(0.1f*x) + x*cosf(0.1f*z) );
}

void TexturedPeaksAndValleysV1::initVertexBuffer()
{
	// resize vertex list to the appropriate size
	mVertexList.resize(mNumVertices);
	float halfWidth = (mNumCols-1) * mDX * 0.5f;
	float halfDepth = (mNumRows-1) * mDX * 0.5f;

	float du = 1.0f / (mNumCols-1);
	float dv = 1.0f / (mNumRows-1);
	for(DWORD i = 0; i < mNumRows; ++i)
	{
		float z = halfDepth - i*mDX;
		for(DWORD j = 0; j < mNumCols; ++j)
		{
			DWORD currIndex = i*mNumCols + j;
			float x = -halfWidth + j*mDX;

			// generate a mountain range
			float y = getHeight(x,z);

			// set the current vertex's position, texcoord and normal
			mVertexList[currIndex].position = DXVector3(x,y,z);
			mVertexList[currIndex].texC.x = j * du;
			mVertexList[currIndex].texC.y = i * dv;

			// calculate the normal
			DXVector3 normal;
			normal.x = -0.03f*z*cosf(0.1f*x) - 0.3f*cosf(0.1f*z);
			normal.y = 1.0f;
			normal.z = -0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z);
			normal.Normalize();
			mVertexList[currIndex].normal = normal;
			//D3DXVec3Normalize(&mVertexList[currIndex].normal, &normal);	// @TODO: remove when done testing
		}
	}
}

void TexturedPeaksAndValleysV1::initIndexBuffer()
{
	// resize the index buffer list
	mIndexList.resize(mNumFaces * mNumVerticesPerFace);

	// Iterate over each quad and compute indices.
	int k = 0;
	for(DWORD i = 0; i < mNumRows-1; ++i)
	{
		for(DWORD j = 0; j < mNumCols-1; ++j)
		{
			mIndexList[k]   = i*mNumCols+j;
			mIndexList[k+1] = i*mNumCols+j+1;
			mIndexList[k+2] = (i+1)*mNumCols+j;

			mIndexList[k+3] = (i+1)*mNumCols+j;
			mIndexList[k+4] = i*mNumCols+j+1;
			mIndexList[k+5] = (i+1)*mNumCols+j+1;

			k += 6; // next quad
		}
	}
}