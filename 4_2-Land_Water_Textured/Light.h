//=======================================================================================
// Light.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef LIGHT_H
#define LIGHT_H

#include "d3dUtil.h"

struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}

	DXVector3 position;
	float pad1;      // not used
	DXVector3 dir;
	float pad2;      // not used
	DXColor ambient;
	DXColor diffuse;
	DXColor specular;
	DXVector3 att;
	float spotPow;
};

struct Material
{
	Material()
	{
		ZeroMemory(this, sizeof(Material));
	}

	DXColor ambient;
	DXColor diffuse;
	DXColor specular;
	float specPower;
};

#endif // LIGHT_H