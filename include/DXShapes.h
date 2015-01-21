// Contains geometric shapes which can be drawn.

#ifndef DX_SHAPES_H
#define DX_SHAPES_H

// Including all existing geometric shapes.
#include "BasicSphere.h"
#include "TextureBox.h"
#include "ComplexQuad.h"
#include "DXModel.h"
#include "SimpleBox.h"
#include "TangentBox.h"
#include "TangentQuad.h"
#include "TexturedPeaksAndValleysV1.h"
#include "TexturedWavesV1.h"
#include "RoomV1.h"
#include "SimpleQuad.h"
#include "LitSphere.h"

// Sphere of Vertex type
class ComplexSphere : public ObjectV2<Vertex>
{
public:

	// Param ctor
	ComplexSphere(ID3D11Device *pDevice, const float fRadius, const UINT nSlices = 30, const UINT nStacks = 30, const DXColor colour = WHITE);

	// Dtor
	~ComplexSphere();

	// Initialize the vertex buffer. (overriden virtual function)
	void initVertexBuffer();

	// Initialize the index buffer. (overriden virtual function)
	void initIndexBuffer();

protected:

	// Radius of the sphere.
	float m_fRadius;

	// No. of slices and stacks which form the sphere.
	UINT m_nSlices, m_nStacks;

	// Colour of the sphere.
	DXColor m_colour;

	// Mesh data for the sphere
	GeometryGenerator::MeshData m_meshData;
};
typedef std::unique_ptr<ComplexSphere> ComplexSpherePtr;

#endif	// DX_SHAPES_H