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
	float range;

	// added for animation
	float radius;
	float angle;
	float animationSpeed;
	//float pad3[3];	// not used
};

// Variant of the Light struct without the padding variables for use in Structured Buffers.
struct SLight
{
	SLight()
	{
		ZeroMemory(this, sizeof(SLight));
	}

	DXVector3 position;
	//float pad1;      // not used
	DXVector3 dir;
	//float pad2;      // not used
	DXColor ambient;
	DXColor diffuse;
	DXColor specular;
	DXVector3 att;
	float spotPow;
	float range;

	// added for animation
	float radius;
	float angle;
	float animationSpeed;
};

// Updated with default param structs for parallel, point and spot lights (Rohit S, Feb-16-2014)
struct ParallelLightParams
{
	// Default ctor.
	ParallelLightParams() : 
		dir(0.57735f, -0.57735f, 0.57735f),
		ambient(0.4f, 0.4f, 0.4f, 1.0f),
		diffuse(WHITE),
		specular(WHITE),
		radius(0.0f),
		angle(0.0f),
		animationSpeed(0.0f)
	{}

	DXVector3 dir;
	DXColor ambient;
	DXColor diffuse;
	DXColor specular;

	// added for animation
	float radius;
	float angle;
	float animationSpeed;
};

struct PointLightParams
{
	DXVector3 position;
	DXColor ambient;
	DXColor diffuse;
	DXColor specular;
	DXVector3 att; 
	float range;

	// added for animation
	float radius;
	float angle;
	float animationSpeed;
};

struct SpotLightParams
{
	DXVector3 position;
	DXVector3 dir;
	DXColor ambient;
	DXColor diffuse;
	DXColor specular;
	DXVector3 att;
	float spotPow;
	float range;

	// added for animation
	float radius;
	float angle;
	float animationSpeed;
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