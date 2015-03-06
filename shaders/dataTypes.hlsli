// Vertex types and other associated data types.

// Complete vertex - all required information is available.

// Vertex shader input struct
struct COMPLETE_VS_IN
{
	float3 posL	: POSITION;
	float3 normalL	: NORMAL;
	float2 texC	: TEXCOORD;
	float3 tangentL	: TANGENT;
	float3 binormalL : BINORMAL;
};

// Vertex shader output struct
struct COMPLETE_VS_OUT
{
	float4 posH	: SV_POSITION;
	float3 posW	: POSITION;
	float3 normalW	: NORMAL;
	float2 texC	: TEXCOORD;
	float3 tangentW	: TANGENT;
	float3 binormalW : BINORMAL;
};

// Debug vertex - used to debug a texture by rendering it onto a quad.

// Vertex shader input
struct DEBUG_VS_IN
{
	float3 posL	: POSITION;	// object space position
	float3 normalL	: NORMAL;
	float2 texC	: TEXCOORD;
	float3 tangentL	: TANGENT;
};

// Vertex shader output
struct DEBUG_VS_OUT
{
	float4 posH	: SV_POSITION;	// homogeneous clip space position
	float2 texC	: TEXCOORD;		// transformed texture coordinate
};

// Particle system struct, maps to the ParticleSystemEffect::Particle struct
struct Particle
{
	// World space initial position
	float3 initPosition;

	// Age (starts from 0 and is incremented by a given time step, should be killed once it exceeds the maximum particle lifetime)
	float age;

	// Initial velocity
	float3 initVelocity;

	// Particle type (0 = emitter, 1 = particle)
	uint type;

	// Particle colour
	float4 colour;

	// Particle size
	float3 size;
};

// Particle system vertex shader output type
struct PARTICLE_VS_OUT
{
	float3 posW : POSITION;

	float4 colour : COLOR;

	// @TODO: add additional elements as required
};

// Particle system geometry shader output type
struct PARTICLE_GS_OUT
{
	float3 posW : POSITION;

	float4 posH : SV_Position;

	float4 colour : COLOR;

	float2 texC : TEXCOORD;

	// @TODO: add additional elements as required
};

// Skybox vertex shader input struct
struct SKYBOX_VS_IN
{
	float3 posL	: POSITION;
};

// Skybox vertex shader output struct
struct SKYBOX_VS_OUT
{
	float4 posH	: SV_Position;
	float3 texC : TEXCOORD;
};

// Deferred shading GBuffer outputs.

// Deferred pixel shader output struct
struct BASIC_DEFERRED_PS_OUT
{
	float4 posW	: SV_TARGET0;
	float4 normalW	: SV_TARGET1;
	float4 diffuse	: SV_TARGET2;
	float4 specular	: SV_TARGET3;
};