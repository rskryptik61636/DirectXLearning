// Source file for the TangentBox class

#include "TangentBox.h"

// Default ctor
TangentBox::TangentBox() : ObjectV2<TangentVertex>()
{}

// Dtor
TangentBox::~TangentBox()
{}

// Init function with an option to specify whether the face normals need to be reversed
// (useful to render a room in which case the winding order of the triangles will be reversed)
void TangentBox::initBox(ID3D11Device *pDevice, float scale, bool bFlipNormals /*= false*/)
{
	// set the flip normals flag
	setFlipNormals(bFlipNormals);

	// call the default init function
	init(pDevice, scale);
}

// Function which initializes the vertex buffer
void TangentBox::initVertexBuffer()
{
	// generate the mesh data for the box
	GeometryGenerator geometryGenerator;
	geometryGenerator.CreateBox(1.0f, 1.0f, 1.0f, m_pBoxMeshData);

	// set the no. of vertices and fill the vertex list
	mNumVertices = m_pBoxMeshData.Vertices.size();
	mVertexList.resize(mNumVertices);
	for (std::size_t i = 0; i < mNumVertices; ++i)
	{
		mVertexList[i].position = m_pBoxMeshData.Vertices[i].Position;
		mVertexList[i].normal = getFlipNormals() ? -m_pBoxMeshData.Vertices[i].Normal : m_pBoxMeshData.Vertices[i].Normal;	// invert the normals if specified
		mVertexList[i].texC = m_pBoxMeshData.Vertices[i].TexC;
		mVertexList[i].tangent = getFlipNormals() ? -m_pBoxMeshData.Vertices[i].TangentU : m_pBoxMeshData.Vertices[i].TangentU;	// inverting the normals also involves inverting the tangent vectors
	}
}

// Function which initializes the index buffer
void TangentBox::initIndexBuffer()
{
	// set the no. of faces and vertices per face
	const std::size_t nIndices(m_pBoxMeshData.Indices.size());
	mNumVerticesPerFace = 3;	
	mNumFaces = nIndices / mNumVerticesPerFace;
	
	// update the index list with that of the mesh data
	mIndexList.resize(nIndices);
	std::copy(m_pBoxMeshData.Indices.begin(), m_pBoxMeshData.Indices.end(), mIndexList.begin());
}