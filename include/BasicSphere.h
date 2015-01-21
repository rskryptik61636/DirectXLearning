// header file for basic sphere class 

#ifndef BASIC_SPHERE_H
#define BASIC_SPHERE_H

#include "ObjectV2.h"

class BasicSphere : public ObjectV2<SimpleVertex>
{
public:
	BasicSphere();
	virtual ~BasicSphere();

	virtual void initVertexBuffer();
	virtual void initIndexBuffer();

#if 0
protected:
	//void defineIcosahedron();

	// GPO: add functions to do recursive subdivision here

protected:
	std::vector<SimpleVertex> icosahedronVertices;
	std::vector<DWORD> icosahedronIndices;

	DWORD mNumIcosahedronVertices;
	DWORD mNumIcosahedronFaces;
#endif	// 0

};

#endif	// BASIC_SPHERE_H