// Vertex shader for particle systems in forward rendering

#include "particleSystemCommon.hlsli"

// Particles buffer
StructuredBuffer<Particle> gParticles : register(t0);

ForwardVSOut forwardParticleSystemVS( uint id : SV_VertexID )
{
	// Declare the output param
	ForwardVSOut vOut;

	// Get the current particle's world space position from the particles buffer.
	Particle particle = gParticles[id];
	vOut.posW = particle.initPosition;	// Set the world space position in the output param as it will be transformed
														// to homogeneous clip space in the corresponding geometry shader.

	// Set the pixel colour and modify its opacity based on the particle's age.
	vOut.colour = particle.colour;
	/*float opacity = 1.0f - lerp(0.0f, 1.0f, particle.age / gLifetime);
	vOut.colour = float3(particle.colour.xyz, opacity);*/

	return vOut;
}