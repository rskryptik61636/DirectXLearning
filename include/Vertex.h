#ifndef VERTEX_H
#define VERTEX_H

#include "d3dUtil.h"

typedef enum 
{
	SIMPLE_VERTEX,
	LIGHT_VERTEX,
	COMPLEX_VERTEX,
	TEXTURE_VERTEX,
	TANGENT_VERTEX
}VertexType;

struct SimpleVertex
{
	DXVector3 position;
	DXColor color;

	SimpleVertex()
	{
		position.x = position.y = position.z = 0;
		color.x = color.y = color.z = color.w = 0;
	}

	SimpleVertex(DXVector3 pos, DXColor col)	: position(pos), color(col)
	{}

	SimpleVertex(float x, float y, float z, float r, float g, float b, float a)
	{
		position.x = x;
		position.y = y;
		position.z = z;

		color.x = r;
		color.y = g;
		color.z = b;
		color.w = a;
	}

	static int GetNumElements()
	{
		return 2;
	}

	static D3D11_INPUT_ELEMENT_DESC *GetElementDesc()
	{
		static D3D11_INPUT_ELEMENT_DESC simpleVertexDesc[] = 
		{
			{	
				"POSITION",						// semantic name
				0,								// semantic index
				DXGI_FORMAT_R32G32B32_FLOAT,	// format
				0,								// input slot
				0,								// aligned byte offset
				D3D11_INPUT_PER_VERTEX_DATA,	// input slot class
				0								// instance data step rate
			},

			{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}	
		};
		return simpleVertexDesc;
	}
};

struct Vertex //: public SimpleVertex
{
	Vertex()	{}

	Vertex(
		DXVector3 pos,
		DXVector3 norm,
		DXVector2 tex,
		DXVector3 tan,
		DXVector3 bin,
		DXColor col)
		: position(pos),
		normal(norm),
		texCoord(tex),
		tangent(tan),
		binormal(bin),
		color(col)
	{}

	// Out of order ctor to accommodate one client who was initializing it this way.
	Vertex(
		DXVector3 pos,
		DXVector3 norm,
		DXVector3 tan,
		DXVector3 bin,
		DXVector2 tex,
		DXColor col)
		: position(pos),
		normal(norm),
		texCoord(tex),
		tangent(tan),
		binormal(bin),
		color(col)
	{}

	Vertex(
		float posx, float posy, float posz,
		float normx, float normy, float normz,
		float texx, float texy,
		float tanx, float tany, float tanz,
		float binx, float biny, float binz,
		float colr, float colg, float colb, float cola)
		: position(posx, posy, posz),
		normal(normx, normy, normz),
		texCoord(texx, texy),
		tangent(tanx, tany, tanz),
		binormal(binx, biny, binz),
		color(colr, colg, colb, cola)
	{}

	DXVector3 position;
	DXVector3 normal;
	DXVector2 texCoord;
	DXVector3 tangent;
	DXVector3 binormal;	
	DXColor color;

	static int GetNumElements()
	{
		return 6;
	}

	// NOTE: Re-arranged the vertex description so that it aligns with other vertex types which are subsets of it. (Mar-11-2014)
	static D3D11_INPUT_ELEMENT_DESC *GetElementDesc()
	{
		static D3D11_INPUT_ELEMENT_DESC complexVertexDesc[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		return complexVertexDesc;
	}
};

struct LightVertex
{
	DXVector3 position;
	DXVector3 normal;
	DXColor   diffuse;
	DXColor   spec; // (r, g, b, specPower);

	static int GetNumElements()
	{
		return 4;
	}

	static D3D11_INPUT_ELEMENT_DESC *GetElementDesc()
	{
		static D3D11_INPUT_ELEMENT_DESC lightVertexDesc[] = 
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"DIFFUSE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		return lightVertexDesc;
	}
};

struct TextureVertex //: public SimpleVertex
{
	TextureVertex() /*: SimpleVertex()*/	{}
	TextureVertex(float x, float y, float z, 
		float nx, float ny, float nz, 
		float u, float v)
		: position(x,y,z), normal(nx,ny,nz), texC(u,v){}

	DXVector3 position;
	DXVector3 normal;
	DXVector2 texC;

	static int GetNumElements()
	{
		return 3;
	}

	static D3D11_INPUT_ELEMENT_DESC *GetElementDesc()
	{
		static D3D11_INPUT_ELEMENT_DESC textureVertexDesc[] = 
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		return textureVertexDesc;
	}
};

struct TangentVertex
{
	TangentVertex()	{}

	TangentVertex(const DXVector3 &pos, const DXVector3 &norm, const DXVector2 &texCoord, const DXVector3 &tangent1)
		: position(pos), normal(norm), texC(texCoord), tangent(tangent1)	{}

	DXVector3 position;
	DXVector3 normal;
	DXVector2 texC;
	DXVector3 tangent;

	static int GetNumElements()
	{
		return 4;
	}

	static D3D11_INPUT_ELEMENT_DESC *GetElementDesc()
	{
		static D3D11_INPUT_ELEMENT_DESC tangentVertexDesc[] = 
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		return tangentVertexDesc;
	}
};

// helper function which returns the vertex layout description of the given VertexType
static void GetVertexDesc(const VertexType vertexType, D3D11_INPUT_ELEMENT_DESC **vertexElementsDesc, UINT &nElements)
{
	switch (vertexType)
	{
	case SIMPLE_VERTEX:
		*vertexElementsDesc = SimpleVertex::GetElementDesc();
		nElements = SimpleVertex::GetNumElements();
		break;

	case LIGHT_VERTEX:
		*vertexElementsDesc = LightVertex::GetElementDesc();
		nElements = LightVertex::GetNumElements();
		break;

	case COMPLEX_VERTEX:
		*vertexElementsDesc = Vertex::GetElementDesc();
		nElements = Vertex::GetNumElements();
		break;

	case TEXTURE_VERTEX:
		*vertexElementsDesc = TextureVertex::GetElementDesc();
		nElements = TextureVertex::GetNumElements();
		break;

	case TANGENT_VERTEX:
		*vertexElementsDesc = TangentVertex::GetElementDesc();
		nElements = TangentVertex::GetNumElements();
		break;
	}
}

#endif	// VERTEX_H