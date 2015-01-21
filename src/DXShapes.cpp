// Contains geometric shapes which can be drawn.

#include <DXShapes.h>

// NOTE: The definitions of the other geometric shape classes are all a part of DXEngine(d).lib

// Param ctor
ComplexSphere::ComplexSphere(ID3D11Device *pDevice, const float fRadius, const UINT nSlices /*= 30*/, const UINT nStacks /*= 30*/, const DXColor colour /*= WHITE*/)
: ObjectV2<Vertex>(), m_fRadius(fRadius), m_nSlices(nSlices), m_nStacks(nStacks), m_colour(colour)
{
	// Initialize the sphere (RAII)
	init(pDevice);
}

// Dtor
ComplexSphere::~ComplexSphere()	
{}

// Initialize the vertex buffer. (overriden virtual function)
void ComplexSphere::initVertexBuffer()
{
	// Generate a sphere of the given no. of slices and stacks.
	GeometryGenerator geometryGenerator;
	geometryGenerator.CreateSphere(m_fRadius, m_nSlices, m_nStacks, m_meshData);

	// Init the vertex buffer.
	mVertexList.resize(m_meshData.Vertices.size());
	for (std::size_t i = 0; i < mVertexList.size(); ++i)
	{
		mVertexList[i].color = m_colour;
		mVertexList[i].normal = m_meshData.Vertices[i].Normal;
		mVertexList[i].position = m_meshData.Vertices[i].Position;
		mVertexList[i].tangent = m_meshData.Vertices[i].TangentU;
		mVertexList[i].texCoord = m_meshData.Vertices[i].TexC;
		mVertexList[i].binormal = m_meshData.Vertices[i].Normal.Cross(m_meshData.Vertices[i].TangentU);	// binormal = cross(normal, tangent)
	}
}

// Initialize the index buffer. (overriden virtual function)
void ComplexSphere::initIndexBuffer()
{
	// set the no. of faces and vertices per face
	const std::size_t nIndices(m_meshData.Indices.size());
	mNumVerticesPerFace = 3;
	mNumFaces = nIndices / mNumVerticesPerFace;

	// update the index list with that of the mesh data
	mIndexList.resize(nIndices);
	std::copy(m_meshData.Indices.begin(), m_meshData.Indices.end(), mIndexList.begin());
}