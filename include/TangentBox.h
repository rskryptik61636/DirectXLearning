// Header file for the TangentBox class

#include "ObjectV2.h"
#include "GeometryGenerator.h"
#include "Vertex.h"

class TangentBox : public ObjectV2<TangentVertex>
{
public:

	// Default ctor
	TangentBox();

	// Dtor
	~TangentBox();

	// Init function with an option to specify whether the face normals need to be reversed
	// (useful to render a room in which case the winding order of the triangles will be reversed)
	void initBox(ID3D11Device *pDevice, float scale, bool bFlipNormals = false);

	// Function which initializes the vertex buffer
	void initVertexBuffer();

	// Function which initializes the index buffer
	void initIndexBuffer();

private:

	// MeshData object for the box
	GeometryGenerator::MeshData m_pBoxMeshData;

	// flag specifying whether the normals need to be flipped or not (defaults to false)
	bool m_bFlipNormals;

	// accessor function for the flip normals flag
	inline bool getFlipNormals() const	{ return m_bFlipNormals; }

	// mutator function for the flip normals flag
	inline void setFlipNormals(const bool bFlipNormals)	{ m_bFlipNormals = bFlipNormals; }

};	// end of class TangentBox
typedef std::unique_ptr<TangentBox> TangentBoxPtr;