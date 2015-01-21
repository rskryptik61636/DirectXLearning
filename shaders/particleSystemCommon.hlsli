// Header containing common structs for particle system effects.

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

// Forward rendering vertex shader output type
struct ForwardVSOut
{
	float3 posW : POSITION;

	float4 colour : COLOR;

	// @TODO: add additional elements as required
};

// Forward rendering geometry shader output type
struct ForwardGSOut
{
	float3 posW : POSITION;

	float4 posH : SV_Position;

	float4 colour : COLOR;

	float2 texC : TEXCOORD;

	// @TODO: add additional elements as required
};

// Helper function to pick a random number from a texture containing random numbers 
// using the given selection criteria r1 and r2 which should be in the [0,1] range.
float4 randomNum(Texture1D<float4> randomNums, SamplerState randomSampler, float r1, float r2)
{
	// Compute a random sampling offset based on the selection criteria.
	float offset = saturate(r1 + r2);

	// Return the random number which is located at the computed offset in the texture.
	return randomNums.SampleLevel(randomSampler, offset, 0);
}