// Source file for the SimpleBox class

#include "SimpleBox.h"

// Default ctor
SimpleBox::SimpleBox() : ObjectV2<SimpleVertex>()
{}

// Dtor
SimpleBox::~SimpleBox()
{}

// Init function in which the color of the box is specified
void SimpleBox::initBox(ID3D11Device *pDevice, float scale, DXColor color)
{
	// init the box and set the color
	m_color = color;
	init(pDevice, scale);	
}

// define vertex buffer
void SimpleBox::initVertexBuffer()
{
	// create a box using the GeometryGenerator and use its vertices
	GeometryGenerator geometryGenerator;
	geometryGenerator.CreateBox(1.0f, 1.0f, 1.0f, m_meshData);

	// fill the vertex list with the vertices of the mesh data
	mNumVertices = m_meshData.Vertices.size();
	mVertexList.resize(mNumVertices);
	for (std::size_t i = 0; i < mNumVertices; ++i)
	{
		mVertexList[i].position = m_meshData.Vertices[i].Position;
		mVertexList[i].color = m_color;
	}
}

// define index buffer
void SimpleBox::initIndexBuffer()
{
	// set the no. of faces and vertices per face from the mesh data
	const std::size_t nIndices(m_meshData.Indices.size());
	mNumVerticesPerFace = 3;	mNumFaces = nIndices / mNumVerticesPerFace;

	// fill the index list with the indices of the mesh data
	mIndexList.resize(nIndices);
	std::copy(m_meshData.Indices.begin(), m_meshData.Indices.end(), mIndexList.begin());
}