// header file for the TangentQuad class
#ifndef TANGENT_QUAD_H
#define TANGENT_QUAD_H

#include "ObjectV2.h"

#include "GeometryGenerator.h"

class TangentQuad : public ObjectV2<TangentVertex>
{
public:

	// default ctor
	TangentQuad();

	// define vertex buffer
	void initVertexBuffer();

	// define index buffer
	void initIndexBuffer();

	// @TODO: define public methods here

protected:

	// Mesh data instance which will be used to store the quad's vertex and index information
	GeometryGenerator::MeshData m_meshData;

};	// end of class TangentQuad
typedef std::unique_ptr<TangentQuad> TangentQuadPtr;

#endif	// TANGENT_QUAD_H