#include "TextureBox.h"
#include "Vertex.h"

TextureBox::TextureBox() : ObjectV2()
{
}

TextureBox::~TextureBox()
{
}

void TextureBox::initVertexBuffer()
{
	// set vertex type
	//ObjectV2::vertexType = COMPLEX_VERTEX;

	// initialize vertices
	ObjectV2::mNumVertices = 24;
	ObjectV2::mVertexList.resize(ObjectV2::mNumVertices);

	// @TODO: the object type is not TextureVertex, remove when done testing
#if 0
	// set binomial, tangent and color for all vertices to fixed values
	for(size_t i = 0; i < mVertexList.size(); ++i)
	{
		mVertexList[i].binormal = DXVector3(0.0f,0.0f,0.0f);
		mVertexList[i].tangent = DXVector3(0.0f,0.0f,0.0f);
		mVertexList[i].color = WHITE;
	}
#endif	// 0

	// front face vertex data
	mVertexList[0].position = DXVector3(-1.0f,-1.0f,-1.0f);
	mVertexList[0].normal = DXVector3(0.0f,0.0f,-1.0f);
	mVertexList[0].texC = DXVector2(0.0f,1.0f);
	/*mVertexList[0].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[0].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[0].color = WHITE;*/

	mVertexList[1].position = DXVector3(-1.0f,1.0f,-1.0f);
	mVertexList[1].normal = DXVector3(0.0f,0.0f,-1.0f);
	mVertexList[1].texC = DXVector2(0.0f,0.0f);
	/*mVertexList[1].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[1].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[1].color = WHITE;*/

	mVertexList[2].position = DXVector3(1.0f,1.0f,-1.0f);
	mVertexList[2].normal = DXVector3(0.0f,0.0f,-1.0f);
	mVertexList[2].texC = DXVector2(1.0f,0.0f);
	/*mVertexList[2].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[2].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[2].color = WHITE;*/

	mVertexList[3].position = DXVector3(1.0f,-1.0f,-1.0f);
	mVertexList[3].normal = DXVector3(0.0f,0.0f,-1.0f);
	mVertexList[3].texC = DXVector2(1.0f,1.0f);
	/*mVertexList[3].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[3].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[3].color = WHITE;*/

	// back face vertex data
	mVertexList[4].position = DXVector3(-1.0f,-1.0f,1.0f);
	mVertexList[4].normal = DXVector3(0.0f,0.0f,1.0f);
	mVertexList[4].texC = DXVector2(1.0f,1.0f);
	/*mVertexList[4].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[4].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[4].color = WHITE;*/

	mVertexList[5].position = DXVector3(1.0f,-1.0f,1.0f);
	mVertexList[5].normal = DXVector3(0.0f,0.0f,1.0f);
	mVertexList[5].texC = DXVector2(0.0f,1.0f);
	/*mVertexList[5].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[5].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[5].color = WHITE;*/

	mVertexList[6].position = DXVector3(1.0f,1.0f,1.0f);
	mVertexList[6].normal = DXVector3(0.0f,0.0f,1.0f);
	mVertexList[6].texC = DXVector2(0.0f,0.0f);
	/*mVertexList[6].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[6].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[6].color = WHITE;*/

	mVertexList[7].position = DXVector3(-1.0f,1.0f,1.0f);
	mVertexList[7].normal = DXVector3(0.0f,0.0f,1.0f);
	mVertexList[7].texC = DXVector2(1.0f,0.0f);
	/*mVertexList[7].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[7].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[7].color = WHITE;*/

	// top face vertex data
	mVertexList[8].position = DXVector3(-1.0f,1.0f,-1.0f);
	mVertexList[8].normal = DXVector3(0.0f,1.0f,0.0f);
	mVertexList[8].texC = DXVector2(0.0f,1.0f);
	/*mVertexList[8].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[8].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[8].color = WHITE;*/

	mVertexList[9].position = DXVector3(-1.0f,1.0f,1.0f);
	mVertexList[9].normal = DXVector3(0.0f,1.0f,0.0f);
	mVertexList[9].texC = DXVector2(0.0f,0.0f);
	/*mVertexList[9].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[9].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[9].color = WHITE;*/

	mVertexList[10].position = DXVector3(1.0f,1.0f,1.0f);
	mVertexList[10].normal = DXVector3(0.0f,1.0f,0.0);
	mVertexList[10].texC = DXVector2(1.0f,0.0f);
	/*mVertexList[10].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[10].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[10].color = WHITE;*/

	mVertexList[11].position = DXVector3(1.0f,1.0f,-1.0f);
	mVertexList[11].normal = DXVector3(0.0f,1.0f,0.0f);
	mVertexList[11].texC = DXVector2(1.0f,1.0f);
	/*mVertexList[11].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[11].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[11].color = WHITE;*/

	// bottom face vertex data
	mVertexList[12].position = DXVector3(-1.0f,-1.0f,-1.0f);
	mVertexList[12].normal = DXVector3(0.0f,-1.0f,0.0f);
	mVertexList[12].texC = DXVector2(1.0f,1.0f);
	/*mVertexList[12].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[12].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[12].color = WHITE;*/

	mVertexList[13].position = DXVector3(1.0f,-1.0f,-1.0f);
	mVertexList[13].normal = DXVector3(0.0f,-1.0f,0.0f);
	mVertexList[13].texC = DXVector2(0.0f,1.0f);
	/*mVertexList[13].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[13].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[13].color = WHITE;*/

	mVertexList[14].position = DXVector3(1.0f,-1.0f,1.0f);
	mVertexList[14].normal = DXVector3(0.0f,-1.0f,0.0);
	mVertexList[14].texC = DXVector2(0.0f,0.0f);
	/*mVertexList[14].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[14].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[14].color = WHITE;*/

	mVertexList[15].position = DXVector3(-1.0f,-1.0f,1.0f);
	mVertexList[15].normal = DXVector3(0.0f,-1.0f,0.0f);
	mVertexList[15].texC = DXVector2(1.0f,0.0f);
	/*mVertexList[15].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[15].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[15].color = WHITE;*/

	// left face vertex data
	mVertexList[16].position = DXVector3(-1.0f,-1.0f,1.0f);
	mVertexList[16].normal = DXVector3(-1.0f,0.0f,0.0f);
	mVertexList[16].texC = DXVector2(0.0f,1.0f);
	/*mVertexList[16].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[16].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[16].color = WHITE;*/

	mVertexList[17].position = DXVector3(-1.0f,1.0f,1.0f);
	mVertexList[17].normal = DXVector3(-1.0f,0.0f,0.0f);
	mVertexList[17].texC = DXVector2(0.0f,0.0f);
	/*mVertexList[17].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[17].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[17].color = WHITE;*/

	mVertexList[18].position = DXVector3(-1.0f,1.0f,-1.0f);
	mVertexList[18].normal = DXVector3(-1.0f,0.0f,0.0);
	mVertexList[18].texC = DXVector2(1.0f,0.0f);
	/*mVertexList[18].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[18].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[18].color = WHITE;*/

	mVertexList[19].position = DXVector3(-1.0f,-1.0f,-1.0f);
	mVertexList[19].normal = DXVector3(-1.0f,0.0f,0.0f);
	mVertexList[19].texC = DXVector2(1.0f,1.0f);
	/*mVertexList[19].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[19].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[19].color = WHITE;*/

	// right face vertex data
	mVertexList[20].position = DXVector3(1.0f,-1.0f,-1.0f);
	mVertexList[20].normal = DXVector3(1.0f,0.0f,0.0f);
	mVertexList[20].texC = DXVector2(0.0f,1.0f);
	/*mVertexList[20].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[20].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[20].color = WHITE;*/

	mVertexList[21].position = DXVector3(1.0f,1.0f,-1.0f);
	mVertexList[21].normal = DXVector3(1.0f,0.0f,0.0f);
	mVertexList[21].texC = DXVector2(0.0f,0.0f);
	/*mVertexList[21].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[21].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[21].color = WHITE;*/

	mVertexList[22].position = DXVector3(1.0f,1.0f,1.0f);
	mVertexList[22].normal = DXVector3(1.0f,0.0f,0.0);
	mVertexList[22].texC = DXVector2(1.0f,0.0f);
	/*mVertexList[22].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[22].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[22].color = WHITE;*/

	mVertexList[23].position = DXVector3(1.0f,-1.0f,1.0f);
	mVertexList[23].normal = DXVector3(1.0f,0.0f,0.0f);
	mVertexList[23].texC = DXVector2(1.0f,1.0f);
	/*mVertexList[23].binormal = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[23].tangent = DXVector3(0.0f,0.0f,0.0f);
	mVertexList[23].color = WHITE;*/

	// scale the object (the vertices are already being scaled in the base class, this is a bug)
#if 0
	for(size_t i = 0; i < mVertexList.size(); ++i)
	{
		mVertexList[i].position *= vertexScale;
		mVertexList[i].texC *= 1.0f;		// playing around with texture co-ordinates (experimentation)
	}
#endif	// 0
}

void TextureBox::initIndexBuffer()
{
	// set the number of indices
	ObjectV2::mNumFaces = 12;
	ObjectV2::mNumVerticesPerFace = 3;

	// set the indices
	DWORD indexList[36];

	// Fill in the front face index data
	indexList[0] = 0; indexList[1] = 1; indexList[2] = 2;
	indexList[3] = 0; indexList[4] = 2; indexList[5] = 3;

	// Fill in the back face index data
	indexList[6] = 4; indexList[7]  = 5; indexList[8]  = 6;
	indexList[9] = 4; indexList[10] = 6; indexList[11] = 7;

	// Fill in the top face index data
	indexList[12] = 8; indexList[13] =  9; indexList[14] = 10;
	indexList[15] = 8; indexList[16] = 10; indexList[17] = 11;

	// Fill in the bottom face index data
	indexList[18] = 12; indexList[19] = 13; indexList[20] = 14;
	indexList[21] = 12; indexList[22] = 14; indexList[23] = 15;

	// Fill in the left face index data
	indexList[24] = 16; indexList[25] = 17; indexList[26] = 18;
	indexList[27] = 16; indexList[28] = 18; indexList[29] = 19;

	// Fill in the right face index data
	indexList[30] = 20; indexList[31] = 21; indexList[32] = 22;
	indexList[33] = 20; indexList[34] = 22; indexList[35] = 23;

	// fill the index list
	ObjectV2::mIndexList.resize(mNumFaces * mNumVerticesPerFace);
	for(size_t i = 0; i < ObjectV2::mIndexList.size(); ++i)
		ObjectV2::mIndexList[i] = indexList[i];
}

// variant of draw which sets the diffuse and specular maps in the pixel shader before drawing the box
void TextureBox::drawBox(const UINT diffuseMapBindPoint, const ShaderResourceViewPtr &diffuseMapRV,
	const UINT specMapBindPoint, const ShaderResourceViewPtr &specMapRV)
{
	// get the immediate rendering context of the current device
	DeviceContextPtr pDeviceContext;
	mD3dDevice->GetImmediateContext(&pDeviceContext.p);

	// set the diffuse and specular map resources in the pixel shader
	const UINT bindStartPoint = diffuseMapBindPoint < specMapBindPoint ? diffuseMapBindPoint : specMapBindPoint;
	ID3D11ShaderResourceView *const ppShaderRV[] = { diffuseMapRV.p, specMapRV.p };
	pDeviceContext->PSSetShaderResources(bindStartPoint, 2, ppShaderRV);

	// release the immediate rendering context
	pDeviceContext.Release();

	// call the base class draw method
	ObjectV2::draw();
}