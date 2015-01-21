// header file for the SimpleQuad class
#ifndef SIMPLE_QUAD_H
#define SIMPLE_QUAD_H

#include "ObjectV2.h"

class SimpleQuad : public ObjectV2<SimpleVertex>
{
public:

	// param ctor 1, takes in the 4 quad points in CW order as well as the color of each point
	SimpleQuad(const std::vector<SimpleVertex> pts);

	// define vertex buffer
	void initVertexBuffer();

	// define index buffer
	void initIndexBuffer();

	// override the init method to make the vertex buffer dynamic
	void init(ID3D11Device *device, float scale);

	// updates the color of all the points with the given color
	void updateColor(const DXColor color);

	// @TODO: define public methods here

protected:

	// @TODO: define protected methods here

public:

	// @TODO: define public members here

protected:

	// quad vertices
	std::vector<SimpleVertex> m_pts;

	// @TODO: define protected members here

};	// end of class SimpleQuad
typedef std::unique_ptr<SimpleQuad> SimpleQuadPtr;

#endif	// SIMPLE_QUAD_H