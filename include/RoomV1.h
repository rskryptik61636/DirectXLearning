// header file for the Room object

#ifndef ROOM_V1_H
#define ROOM_V1_H

#include "ObjectV2.h"

class RoomV1 : public ObjectV2<Vertex>
{
public:
	RoomV1();
	virtual ~RoomV1();

	virtual void initVertexBuffer();	// func to init vertex buffer
	virtual void initIndexBuffer();		// func to init index buffer (redundant, we don't have an index buffer)

	// overriding the init function since we don't use an index buffer
	virtual void init(ID3D11Device *device, float scale);	

	// overriding the draw function (will leave it blank)
	virtual void draw();

	// functions to draw the room and the mirror
	virtual void drawRoom(ID3DX11EffectPass *pass, 
						const EffectShaderResourceVariablePtr &diffuseVar, 
						const ShaderResourceViewPtr &wallRV,
						const ShaderResourceViewPtr &floorRV);	// works with the Effects framework

	virtual void drawRoom(const ShaderResourceViewPtr &wallRV,
						const UINT wallBindPoint,
						const ShaderResourceViewPtr &floorRV,
						const UINT floorBindPoint);	// works without the Effects framework

	virtual void drawFloor();
	virtual void drawWall();
	
	virtual void drawMirror(ID3DX11EffectPass *pass, const EffectShaderResourceVariablePtr &mirrorVar, const ShaderResourceViewPtr &mirrorRV);	// works with the Effects framework
	
	virtual void drawMirror(const ShaderResourceViewPtr &mirrorRV, const UINT mirrorBindPoint);	// works without the Effects framework

	virtual void drawMirror();
};
typedef std::unique_ptr<RoomV1> RoomV1Ptr;	// smart ptr for RoomV1 instances

#endif	// ROOM_V1_H