// Source file for the TangentQuad class

#include "TangentQuad.h"

// default ctor
TangentQuad::TangentQuad() : ObjectV2<TangentVertex>()	{}

// define vertex buffer
void TangentQuad::initVertexBuffer()
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
		mVertexList[i].texC = m_meshData.Vertices[i].TexC;
	}
}

// define index buffer
void TangentQuad::initIndexBuffer()
{
	// extract the indices list from the mesh data member
	mIndexList.resize(m_meshData.Indices.size());
	std::copy(m_meshData.Indices.begin(), m_meshData.Indices.end(), mIndexList.begin());

	// set the no. of faces and triangles per face for use by the init() method
	mNumVerticesPerFace = 3;	mNumFaces = mIndexList.size() / mNumVerticesPerFace;
}