// Header file for the SimpleBox class
#ifndef SIMPLE_BOX_H
#define SIMPLE_BOX_H

#include "ObjectV2.h"
#include "Vertex.h"
#include "GeometryGenerator.h"

// Box of SimpleVertex vertices
class SimpleBox : public ObjectV2<SimpleVertex>
{
public:

	// Default ctor
	SimpleBox();

	// Dtor
	~SimpleBox();

	// Init function in which the color of the box is specified
	void initBox(ID3D11Device *pDevice, float scale, DXColor color);

	// define vertex buffer
	void initVertexBuffer();

	// define index buffer
	void initIndexBuffer();

private:

	// color of the box
	DXColor m_color;

	// mesh data for the box
	GeometryGenerator::MeshData m_meshData;

};	// end of class SimpleBox
typedef std::unique_ptr<SimpleBox> SimpleBoxPtr;

#endif	// SIMPLE_BOX_H