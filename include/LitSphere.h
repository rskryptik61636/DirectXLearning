// header file for the LitSphere class

#ifndef LIT_SPHERE_H
#define LIT_SPHERE_H

#include "ObjectV2.h"

class LitSphere : public ObjectV2<LightVertex>
{
public:
	LitSphere();
	~LitSphere();

	void initVertexBuffer();
	void initIndexBuffer();
	void init(ID3D11Device *device, float scale);	// overriding base class version

private:
	void computeNormals();	// func to compute normals
};

#endif	// LIT_SPHERE_H