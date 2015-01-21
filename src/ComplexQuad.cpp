// Source file for the ComplexQuad class

#include "ComplexQuad.h"

// default ctor
ComplexQuad::ComplexQuad() : ObjectV2<Vertex>()	{}

// define vertex buffer
void ComplexQuad::initVertexBuffer()
{
	// use the GeometryGenerator to create a full-screen quad
	GeometryGenerator geometryGenerator;
	geometryGenerator.CreateFullscreenQuad(m_meshData);

	// extract the TangentVertex relavant data from it
	mNumVertices = m_meshData.Vertices.size();
	mVertexList.resize(mNumVertices);
	for (std::size_t i = 0; i < mVertexList.size(); ++i)
	{
		mVertexList[i].position = m_meshData.Vertices[i].Position;
		mVertexList[i].normal = m_meshData.Vertices[i].Normal;
		mVertexList[i].tangent = m_meshData.Vertices[i].TangentU;
		mVertexList[i].binormal = mVertexList[i].normal.Cross(mVertexList[i].tangent);
		mVertexList[i].texCoord = m_meshData.Vertices[i].TexC;
		mVertexList[i].color = BLACK;	// The mesh data does not contain a colour, we can just default to black since it isn't important.
	}
}

// define index buffer
void ComplexQuad::initIndexBuffer()
{
	// extract the indices list from the mesh data member
	mIndexList.resize(m_meshData.Indices.size());
	std::copy(m_meshData.Indices.begin(), m_meshData.Indices.end(), mIndexList.begin());

	// set the no. of faces and triangles per face for use by the init() method
	mNumVerticesPerFace = 3;	mNumFaces = mIndexList.size() / mNumVerticesPerFace;
}