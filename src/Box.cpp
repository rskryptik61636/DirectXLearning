// Box class source file

#include "Box.h"
#include "Vertex.h"
#include "GeometryGenerator.h"

Box::Box() : mNumVertices(0), mNumFaces(0), mD3dDevice(), mD3dDeviceContext(), mVb(), mIb()
{}

Box::~Box()
{}

void Box::init(const DevicePtr &device, float scale)
{
	mD3dDevice = device;
	mD3dDevice->GetImmediateContext(&mD3dDeviceContext.p);

	// TODO: Remove when done testing.
#if 0
	mNumVertices = 8;
	mNumFaces = 12;

	// set of vertices to be rendered
	Vertex vertices[] =
	{
		{ DXVector3(-1.0f, -1.0f, -1.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector2(0.0f, 0.0f), WHITE },		// bottom face
		{ DXVector3(-1.0f, +1.0f, -1.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector2(0.0f, 0.0f), BLACK },
		{ DXVector3(+1.0f, +1.0f, -1.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector2(0.0f, 0.0f), RED },
		{ DXVector3(+1.0f, -1.0f, -1.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector2(0.0f, 0.0f), GREEN },

		{ DXVector3(-1.0f, -1.0f, +1.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector2(0.0f, 0.0f), BLUE },		// top face
		{ DXVector3(-1.0f, +1.0f, +1.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector2(0.0f, 0.0f), YELLOW },
		{ DXVector3(+1.0f, +1.0f, +1.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector2(0.0f, 0.0f), CYAN },
		{ DXVector3(+1.0f, -1.0f, +1.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector3(0.0f, 0.0f, 0.0f), DXVector2(0.0f, 0.0f), MAGENTA }
	};

	// scale the box
	for (int i = 0; i < mNumVertices; ++i)
		vertices[i].position *= scale;
#endif // 0

	// generate the mesh data for the box
	GeometryGenerator geometryGenerator;
	GeometryGenerator::MeshData boxMeshData;
	geometryGenerator.CreateBox(1.0f, 1.0f, 1.0f, boxMeshData);

	// set the no. of vertices and fill the vertex list
	mNumVertices = boxMeshData.Vertices.size();
	mNumFaces = boxMeshData.Indices.size() / 3;
	//vertices.resize(mNumVertices);
	std::vector<Vertex> vertices(mNumVertices);
	for (std::size_t i = 0; i < mNumVertices; ++i)
	{
		vertices[i].position = boxMeshData.Vertices[i].Position;
		//vertices[i].normal = getFlipNormals() ? -boxMeshData.Vertices[i].Normal : boxMeshData.Vertices[i].Normal;	// invert the normals if specified
		vertices[i].normal = boxMeshData.Vertices[i].Normal;	// invert the normals if specified
		vertices[i].texCoord = boxMeshData.Vertices[i].TexC;
		//vertices[i].tangent = getFlipNormals() ? -boxMeshData.Vertices[i].TangentU : boxMeshData.Vertices[i].TangentU;	// inverting the normals also involves inverting the tangent vectors
		vertices[i].tangent = boxMeshData.Vertices[i].TangentU;	// inverting the normals also involves inverting the tangent vectors
		vertices[i].binormal = vertices[i].normal.Cross(vertices[i].tangent);	// TODO: Verify that the binormal is pointing the right way.
		vertices[i].color = BLACK;
	}

	// set the vertex buffer description
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	// create subresource data for the vertices
	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem = vertices.data();

	// create the vertex buffer
	HR(mD3dDevice->CreateBuffer(&vbd,&vInitData,&mVb));	

	// Create the index buffer

	// TODO: Remove when done testing.
#if 0
	DWORD indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};
#endif // 0


	// set the index buffer description
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = boxMeshData.Indices.size() * sizeof(UINT); /*sizeof(DWORD) * mNumFaces * 3;*/
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	// create subresource data for the index buffer
	D3D11_SUBRESOURCE_DATA iInitData;
	iInitData.pSysMem = boxMeshData.Indices.data() /*indices*/;

	// create the index buffer
	HR(mD3dDevice->CreateBuffer(&ibd,&iInitData,&mIb));
}

void Box::setIndexAndVertexBuffers()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mD3dDeviceContext->IASetVertexBuffers(0, 1, &mVb.p, &stride, &offset);	// set the vertex buffer for position
	//mD3dDeviceContext->IASetVertexBuffers(1, 1, &mVb, &stride, &offset);	// set the vertex buffer for color
	mD3dDeviceContext->IASetIndexBuffer(mIb.p, DXGI_FORMAT_R32_UINT, 0);	// set the index buffer
}

void Box::draw()
{
	mD3dDeviceContext->DrawIndexed(mNumFaces * 3, 0, 0);					// draw the box
}