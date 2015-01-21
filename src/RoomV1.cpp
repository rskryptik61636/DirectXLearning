// source file for the RoomV1 object

#include "RoomV1.h"

// default ctor
// the room has a total of 30 vertices and no faces and vertices/face are defined
// -as we do not use indexed drawing to render the room
RoomV1::RoomV1() : ObjectV2(30,0,0)		{} 

// dtor
RoomV1::~RoomV1()	{}

// func to init vertex buffer
void RoomV1::initVertexBuffer()
{
	// resize the vertex buffer to the appropriate size
	ObjectV2::mVertexList.resize(ObjectV2::mNumVertices);

	// define the room vertices
	// Floor: Observe we tile texture coordinates.
	mVertexList[0] = TextureVertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f);
	mVertexList[1] = TextureVertex(-7.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	mVertexList[2] = TextureVertex( 7.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f);
	
	mVertexList[3] = TextureVertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f);
	mVertexList[4] = TextureVertex( 7.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f);
	mVertexList[5] = TextureVertex( 7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 4.0f, 4.0f);

	// Wall: Observe we tile texture coordinates, and that we
	// leave a gap in the middle for the mirror.
	mVertexList[6]  = TextureVertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
	mVertexList[7]  = TextureVertex(-7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	mVertexList[8]  = TextureVertex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f);
	
	mVertexList[9]  = TextureVertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
	mVertexList[10] = TextureVertex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f);
	mVertexList[11] = TextureVertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 2.0f);

	mVertexList[12] = TextureVertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
	mVertexList[13] = TextureVertex(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	mVertexList[14] = TextureVertex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f);
	
	mVertexList[15] = TextureVertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
	mVertexList[16] = TextureVertex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f);
	mVertexList[17] = TextureVertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 2.0f);

	mVertexList[18] = TextureVertex(-7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	mVertexList[19] = TextureVertex(-7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	mVertexList[20] = TextureVertex( 7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f);
	
	mVertexList[21] = TextureVertex(-7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	mVertexList[22] = TextureVertex( 7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f);
	mVertexList[23] = TextureVertex( 7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 1.0f);

	// Mirror
	mVertexList[24] = TextureVertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	mVertexList[25] = TextureVertex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	mVertexList[26] = TextureVertex( 2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	
	mVertexList[27] = TextureVertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	mVertexList[28] = TextureVertex( 2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	mVertexList[29] = TextureVertex( 2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
}

// func to init index buffer (leave blank)
void RoomV1::initIndexBuffer()	{}

// overriden init function
void RoomV1::init(ID3D11Device *device, float scale)
{
	// set vertexScale
	ObjectV2::vertexScale = scale; 

	// initialize the vertex buffer (NOTE: index buffer is absent)
	initVertexBuffer();

	// scale up the vertices by vertexScale
	for(size_t i = 0; i < ObjectV2::mVertexList.size(); ++i)
		ObjectV2::mVertexList[i].position *= vertexScale;

	// set the D3D device
	ObjectV2::mD3dDevice = device;

	// create the vertex buffer description
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = ObjectV2::mNumVertices * sizeof(TextureVertex);
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

	// create the vertex init data
	D3D11_SUBRESOURCE_DATA vertexBufferInitData;
	vertexBufferInitData.pSysMem = &ObjectV2::mVertexList[0];

	// create the vertex buffer
	HR( ObjectV2::mD3dDevice->CreateBuffer( &vertexBufferDesc, &vertexBufferInitData, &(ObjectV2::mVb) ) );
	
	// NOTE: the index buffer is absent
}

// overriden draw function (blank)
void RoomV1::draw()	{}

// function to draw the room
void RoomV1::drawRoom(ID3DX11EffectPass *pass, 
						const EffectShaderResourceVariablePtr &diffuseVar, 
						const ShaderResourceViewPtr &wallRV,
						const ShaderResourceViewPtr &floorRV)
{
	// draw the floor of the room
	UINT vertexCount = 6, startLocation = 0;
	const UINT stride = sizeof(TextureVertex), offset = 0;
	diffuseVar->SetResource(floorRV);

	ID3D11DeviceContext *pDeviceContext;
	ObjectV2::mD3dDevice->GetImmediateContext(&pDeviceContext);
	pass->Apply(0, pDeviceContext);
	pDeviceContext->IASetVertexBuffers(0, 1, &mVb, &stride, &offset);
	pDeviceContext->Draw(vertexCount, startLocation);

	// draw the wall of the room
	vertexCount = 18, startLocation = 6;
	diffuseVar->SetResource(wallRV);
	pass->Apply(0, pDeviceContext);
	pDeviceContext->Draw(vertexCount, startLocation);
	pDeviceContext->Release();	// release as GetImmediateContext does a deep copy of the rendering context
}

// function to draw the room
void RoomV1::drawRoom(const ShaderResourceViewPtr &wallRV,
						const UINT wallBindPoint,
						const ShaderResourceViewPtr &floorRV,
						const UINT floorBindPoint)	// works without the Effects framework
{
	// draw the floor of the room
	UINT vertexCount = 6, startLocation = 0;
	const UINT stride = sizeof(TextureVertex), offset = 0;
	//diffuseVar->SetResource(floorRV);

	// get the immediate rendering context and bind the vertex buffer to the input assembler stage
	ID3D11DeviceContext *pDeviceContext;
	ObjectV2::mD3dDevice->GetImmediateContext(&pDeviceContext);
	pDeviceContext->IASetVertexBuffers(0, 1, &mVb, &stride, &offset);

	// bind the wallRV to the pixel shader at the given bind point and draw the floor
	pDeviceContext->PSSetShaderResources(floorBindPoint, 1, &floorRV.p);
	pDeviceContext->Draw(vertexCount, startLocation);

	//// draw the wall of the room
	vertexCount = 18, startLocation = 6;
	//diffuseVar->SetResource(wallRV);
	pDeviceContext->PSSetShaderResources(wallBindPoint, 1, &wallRV.p);
	pDeviceContext->Draw(vertexCount, startLocation);
	pDeviceContext->Release();	// release as GetImmediateContext does a deep copy of the rendering context
}

// function to draw the floor
void RoomV1::drawFloor()
{
	// draw the floor of the room
	const UINT vertexCount = 6, startLocation = 0;
	const UINT stride = sizeof(TextureVertex), offset = 0;
	//diffuseVar->SetResource(floorRV);

	// get the immediate rendering context and bind the vertex buffer to the input assembler stage
	ID3D11DeviceContext *pDeviceContext;
	ObjectV2::mD3dDevice->GetImmediateContext(&pDeviceContext);
	pDeviceContext->IASetVertexBuffers(0, 1, &mVb, &stride, &offset);

	// bind the wallRV to the pixel shader at the given bind point and draw the floor
	pDeviceContext->Draw(vertexCount, startLocation);
}

// function to draw the wall
void RoomV1::drawWall()
{
	//// draw the wall of the room
	const UINT vertexCount = 18, startLocation = 6;
	//diffuseVar->SetResource(wallRV);
	// get the immediate rendering context and bind the vertex buffer to the input assembler stage
	ID3D11DeviceContext *pDeviceContext;
	ObjectV2::mD3dDevice->GetImmediateContext(&pDeviceContext);
	pDeviceContext->Draw(vertexCount, startLocation);
	pDeviceContext->Release();	// release as GetImmediateContext does a deep copy of the rendering context
}

// function to draw the mirror
void RoomV1::drawMirror(ID3DX11EffectPass *pass, const EffectShaderResourceVariablePtr &mirrorVar, const ShaderResourceViewPtr &mirrorRV)
{
	// draw the mirror
	UINT vertexCount = 6, startLocation = 24;
	const UINT stride = sizeof(TextureVertex), offset = 0;
	mirrorVar->SetResource(mirrorRV);

	ID3D11DeviceContext *pDeviceContext;
	ObjectV2::mD3dDevice->GetImmediateContext(&pDeviceContext);
	pDeviceContext->IASetVertexBuffers(0, 1, &mVb, &stride, &offset);
	pass->Apply(0, pDeviceContext);
	pDeviceContext->Draw(vertexCount, startLocation);
	pDeviceContext->Release();	// release as GetImmediateContext does a deep copy of the rendering context
}

// function to draw the mirror
void RoomV1::drawMirror(const ShaderResourceViewPtr &mirrorRV, const UINT mirrorBindPoint)	// works without the Effects framework
{
	// draw the mirror
	UINT vertexCount = 6, startLocation = 24;
	const UINT stride = sizeof(TextureVertex), offset = 0;
	//mirrorVar->SetResource(mirrorRV);

	// get the immediate rendering context, bind the vertex buffer to the input assembler stage, bind the mirrorRV at the given bind point and draw the mirror
	ID3D11DeviceContext *pDeviceContext;
	ObjectV2::mD3dDevice->GetImmediateContext(&pDeviceContext);
	pDeviceContext->IASetVertexBuffers(0, 1, &mVb, &stride, &offset);
	pDeviceContext->PSSetShaderResources(mirrorBindPoint, 1, &mirrorRV.p);
	pDeviceContext->Draw(vertexCount, startLocation);
	pDeviceContext->Release();	// release as GetImmediateContext does a deep copy of the rendering context
}

// function to draw the mirror
void RoomV1::drawMirror()
{
	// draw the mirror
	const UINT vertexCount = 6, startLocation = 24;
	const UINT stride = sizeof(TextureVertex), offset = 0;
	//mirrorVar->SetResource(mirrorRV);

	// get the immediate rendering context, bind the vertex buffer to the input assembler stage, bind the mirrorRV at the given bind point and draw the mirror
	ID3D11DeviceContext *pDeviceContext;
	ObjectV2::mD3dDevice->GetImmediateContext(&pDeviceContext);
	pDeviceContext->IASetVertexBuffers(0, 1, &mVb, &stride, &offset);
	pDeviceContext->Draw(vertexCount, startLocation);
	pDeviceContext->Release();	// release as GetImmediateContext does a deep copy of the rendering context
}