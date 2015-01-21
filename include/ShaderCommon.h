// header file containing shader resouces such as constant buffer structs
#ifndef SHADER_COMMON_H
#define SHADER_COMMON_H

#include "d3dUtil.h"

#include "Light.h"

// V. V. IMP: all constant buffer structs must be aligned on a 16-byte boundary

// struct for the tex vertex shader constant buffer
struct TexVSBuffer
{
	// default ctor, clears the struct memory
	TexVSBuffer()
	{
		ZeroMemory(this, sizeof(TexVSBuffer));
	}

	DXMatrix gWorld;
	DXMatrix gWVP;
	DXMatrix gTexMtx;	
};

// struct for the tex pixel shader constatn buffer
struct TexPSBuffer
{
	// default ctor, clears the struct memory
	TexPSBuffer()
	{
		ZeroMemory(this, sizeof(TexPSBuffer));
	}

	Light gLight;
	DXVector3 gEyePosW;
	//float pad1;				// used for padding
	float gCurrTime;		
	float pad2[3];				// used for padding to ensure we are 16-byte boundary aligned
};

#endif	// SHADER_COMMON_H