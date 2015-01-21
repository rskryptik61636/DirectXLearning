// Compute shader to update the particles of the particle system

// Disclaimer: Based off of the ParticleStorm demo in the Hieroglyph 3 engine (http://hieroglyph3.codeplex.com/)

#include "particleSystemCommon.hlsli"

// Constant buffer containing per object constants.
cbuffer cbPerObject	: register(b0)
{
	float4 gColour;	// particle colour

	float gTimeStep;	// time step

	float3 gSize;	// particle size

	float3 gEmitterPosMinW;	// emitter min position in world space

	float3 gEmitterPosMaxW;	// emitter max position in world space
}

// Particles buffer
AppendStructuredBuffer<Particle> gParticles : register(u0);

// Texture containing random no's for sampling
Texture1D<float4> gRandomNums : register(t0);

// Random no. texture sampler
SamplerState gRandomSampler : register(s0);

// No. of particles spawned per insertion (NOTE: The corresponding no. must be updated in ParticleSystemEffect if this is changed)
#define N 8	

[numthreads(N, 1, 1)]
void particleSystemInsertCS( uint3 DTid : SV_DispatchThreadID )
{
	// Spawn a new particle and initialize its state.
	Particle particle;

	particle.colour = gColour;	// Set the particle colour to the passed in colour.
	particle.size = gSize;	// Set the particle size to the passed in size.

	particle.age = 0.0f;	// Reset the age.
	float particleOffset = (float)DTid.x / (float)N;	// Compute the index offset of the current particle.

	// Set the initial position of the particle to a random location inside the particle emission area.
	float4 offset = randomNum(gRandomNums, gRandomSampler, particleOffset, gTimeStep/**10000.0f*/);
		particle.initPosition = float3(lerp(gEmitterPosMinW.x, gEmitterPosMaxW.x, offset.x),
		lerp(gEmitterPosMinW.y, gEmitterPosMaxW.y, offset.y),
		lerp(gEmitterPosMinW.z, gEmitterPosMaxW.z, offset.z));

	particle.initVelocity = float3(0.0f, 0.0f, 0.0f);	// NOTE: Assuming particles are stationary at birth for now. Can change this behaviour as necessary.
	particle.type = 1;	// Hardcode the particle type to particle for now

	// Append the spawned particle to the particle list.
	gParticles.Append(particle);
}