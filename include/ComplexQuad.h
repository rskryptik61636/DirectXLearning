// header file for the ComplexQuad class
#ifndef COMPLEX_QUAD_H
#define COMPLEX_QUAD_H

#include "ObjectV2.h"

#include "GeometryGenerator.h"

class ComplexQuad : public ObjectV2<Vertex>
{
public:

	// default ctor
	ComplexQuad();

	// define vertex buffer
	void initVertexBuffer();

	// define index buffer
	void initIndexBuffer();

	// @TODO: define public methods here

protected:

	// Mesh data instance which will be used to store the quad's vertex and index information
	GeometryGenerator::MeshData m_meshData;

};	// end of class ComplexQuad
typedef std::unique_ptr<ComplexQuad> ComplexQuadPtr;

#endif	// COMPLEX_QUAD_H