// Common structs and textures shared by the deferred shading effects' pixel shaders

#include "lighthelper.fx"

// Parallel lights structured buffer.
StructuredBuffer<Light> gParallelLights : register(t0);

// Point lights structured buffer.
StructuredBuffer<Light> gPointLights : register(t1);

// Spot lights structured buffer.
StructuredBuffer<Light> gSpotLights : register(t2);

// GBuffer position map.
Texture2D<float4> gPositionMap : register(t3);

// GBuffer normal map.
Texture2D<float4> gNormalMap : register(t4);

// GBuffer diffuse map.
Texture2D<float4> gDiffuseMap : register(t5);

// GBuffer specular map.
Texture2D<float4> gSpecularMap : register(t6);