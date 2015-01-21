

#ifndef VERTEX_H
#define VERTEX_H

struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z, 
		float nx, float ny, float nz, 
		float u, float v)
		: position(x,y,z), normal(nx,ny,nz), texC(u,v){}

	DXVector3 position;
	DXVector3 normal;
	DXVector2 texC;
};

#endif // VERTEX_H

