// Compute shader to update the particles of the particle system

// Disclaimer: Based off of the ParticleStorm demo in the Hieroglyph 3 engine (http://hieroglyph3.codeplex.com/)

#include "particleSystemCommon.hlsli"

// Constant buffer containing per object constants.
cbuffer cbPerObject	: register(b0)
{
	uint gNumParticles;	// total no. of particles in the current simulation state

	float gTimeStep;	// time step

	float3 gAccel;	// particle acceleration

	float gLifetime;	// max particle lifetime	
}

// Current simulation state's particles buffer
ConsumeStructuredBuffer<Particle> gCurrentSimParticles : register(u0);

// New simulation state's particles buffer
AppendStructuredBuffer<Particle> gNewSimParticles : register(u1);

// No. of particles per thread group (NOTE: The corresponding no. must be updated in ParticleSystemEffect if this is changed)
#define N 512

[numthreads(N, 1, 1)]
void particleSystemUpdateCS( uint3 DTid : SV_DispatchThreadID )
{
	// NOTE: This should work with whatever X,Y,Z thread groups are dispatched.
	uint currId = DTid.x + DTid.y * N + DTid.z * N * N;

	// Only run the current thread if it is likely to process a particle.
	if (currId </*=*/ gNumParticles)
	{
		// Get a particle from the current simulation state.
		Particle particle = gCurrentSimParticles.Consume();

		// Update the particle's age and position. (Position = 0.5 * t^2 * initAccel + t * initVel + initPos)
		// NOTE: The initial position is being updated at every step since that is what is used to determine the world space position
		//		 during shading. Update as necessary.
		particle.age += gTimeStep;
		float3 posW = (0.5f * particle.age * particle.age * gAccel) + (particle.age * particle.initVelocity) + particle.initPosition;
		particle.initPosition = posW;

		// Add the current particle to the next simulation state if its age hasn't exceeded the max particle lifetime.
		if (particle.age < gLifetime)
		{
			gNewSimParticles.Append(particle);
		}
	}
}