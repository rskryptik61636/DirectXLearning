// Header file for the ParticleSystemEffect class

#ifndef PARTICLE_SYSTEM_EFFECT_H
#define PARTICLE_SYSTEM_EFFECT_H

// Base DXEffect class
#include "DXEffect.h"

// Class which implements particle systems
class ParticleSystemEffect : public DXEffect
{
public:

	// @TODO: keep the ones we need
#if 0
	// Can be used to specify the current pass to be rendered
	enum EffectPass
	{
		EP_SHADOW_MAP_GENERATION,
		EP_SHADOW_MAP_RENDER
	};

	// Can be used to specify the shadow mapping technique to be used
	enum EffectTechnique
	{
		ET_SIMPLE,
		ET_PCF_1_TAP,
		ET_PCF_16_TAP
	};
#endif // 0

	// Can be used to specify the particle system technique to be used
	enum EffectTechnique
	{
		ET_UNKNOWN = -1,
		ET_RAIN,
		ET_FIRE
	};

	// Particle type
	enum ParticleType
	{
		PT_EMITTER = 0,
		PT_PARTICLE = 1
	};

	// Can be used to specify the particle rendering technique to be used
	enum RenderTechnique
	{
		RT_FORWARD,
		RT_DEFERRED
	};

	// Effect params struct
	// NOTE: Remember to update SceneBuilder if additional params are added.
	struct EffectParams
	{
		// Default ctor
		EffectParams();

		// Particle system technique
		EffectTechnique eTech;

		// Particle rendering technique
		RenderTechnique eRenderTech;

		// Eye position in world space
		DXVector3 eyePosW;

		// Camera's view projection matrix;
		DXMatrix viewProj;

		// Initial acceleration
		DXVector3 initialAcceleration;

		// Initial velocity
		DXVector3 initialVelocity;

		// Lifetime of each particle
		float lifetime;

		// Emitter min/max position in world space
		DXVector3 emitterMinPosW, emitterMaxPosW;

		// Size of each particle
		DXVector3 size;

		// Particle texture
		ID3D11ShaderResourceView* pParticleTexture;

		// GBuffer position map
		ID3D11ShaderResourceView* pGBufferPositionMap;

		// GBuffer normal map
		ID3D11ShaderResourceView* pGBufferNormalMap;

		// GBuffer diffuse map
		ID3D11ShaderResourceView* pGBufferDiffuseMap;

		// GBuffer specular map
		ID3D11ShaderResourceView* pGBufferSpecularMap;
	};

	// Particle struct
	// @TODO: make protected if required
	struct Particle
	{
		// World space initial position
		DXVector3 initPosition;

		// Age (starts from 0 and is incremented by a given time step, should be killed once it exceeds the maximum particle lifetime)
		float age;

		// Initial velocity
		DXVector3 initVelocity;

		// Particle type (0 = emitter, 1 = particle)
		ParticleType type;

		// Particle colour
		DXColor colour;

		// Particle size
		DXVector3 size;
	};

	// Param ctor
	explicit ParticleSystemEffect(const wpath shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext, 
								const UINT nMaxParticles, const EffectParams &initParams);

	// Dtor
	~ParticleSystemEffect();

	// Causes the effect to be applied. (virtual, may be implemented by derived class)
	// NOTE: This version doesn't do anything, the overloaded version which accepts an EffectParams is the one which will be used.
	void apply()	{ throw std::exception("Not implemented!"); }

	// Causes the effect to be applied w/ the given params.
	void apply(const EffectParams &params);

	// Initialization function which creates all the shaders and initializes
	// all shader constant buffers, resources and sampler states.
	void init();

	// Returns a string which contains effect info
	std::wstring getEffectInfo();

	// Accessor functions for the vertex shader's byte code buffer pointer and size.
	// WARNING: should only be called after the init() function has been invoked.
	LPCVOID getVSBufferPointer()	const	{ return m_pVSBuffer; /*m_shaderBlobs["SimpleVertex"].p ? m_shaderBlobs["SimpleVertex"]->GetBufferPointer() : NULL;*/ }
	const SIZE_T getVSBufferSize()	const	{ return m_nVSBufferSize; /*m_shaderBlobs["SimpleVertex"].p ? m_shaderBlobs["SimpleVertex"]->GetBufferSize() : NULL;*/ }

	// Mutator function to set the current time step.
	/*inline*/ void setTimeStep(const float dt);//	{ m_fTimeStep = dt; m_fSpawnStep += dt; }

	// Mutator function to swap the particle system simulation states.
	void swapSimStates();

	// Unbinds all the shaders, resources and samplers that were bound to the pipeline by this effect.
	void cleanup();

	// @TODO: keep the ones that we'll need
#if 0

	// Mutator func to set the current pass
	inline void setCurrentPass(const EffectPass eCurrentPass)	{ m_eCurrentPass = eCurrentPass; }

	// Mutator func to set the current technique
	void setCurrentTechnique(const EffectTechnique eCurrentTechnique);	//{ m_eCurrentTechnique = eCurrentTechnique; }

	// mutator funcs to set the constant buffer members

	// cbPerObject first pass

	// set the first pass' light world view projection matrix (world matrix * light's view projection matrix)
	void setFirstPassLightWVP(const DXMatrix &lightWVP);

	// cbPerObject second pass

	// set the second pass' world view projection matrix
	void setSecondPassWVP(const DXMatrix &wvp);

	// set the second pass' world matrix
	void setSecondPassWorld(const DXMatrix &world);

	// set the second pass' inverse transpose of the world matrix
	void setSecondPassWorldInvTrans(const DXMatrix &worldInvTrans);

	// set the second pass' light space world view projection tangent matrix (world * light's view projection * tangent matrix)
	void setSecondPassLightWorldViewProjTangentMatrix(const std::vector<DXMatrix> &lightWorldViewProjTangent);

	// set the second pass' texture transformation matrix
	void setSecondPassTexMtx(const DXMatrix &texMtx);

	// cbPerFrame

	// set the spot lights
	void setSpotLights(const std::vector<Light> &spotLights);

	// set the world space eye pos
	void setEyePosW(const DXVector3 &eyePosW);

	// set the diffuse texture map
	void setDiffuseMap(const ShaderResourceViewPtr &pDiffuseMap);

	// set the specular light map
	void setSpecularMap(const ShaderResourceViewPtr &pSpecularMap);

	// set the normal map
	void setNormalMap(const ShaderResourceViewPtr &pNormalMap);

	// set the shadow map
	void setShadowMap(const ShaderResourceViewPtr &pShadowMap);

	// set the shadow map dims
	void setShadowMapSize(const UINT nWidth, const UINT nHeight);

	// unbind the shadow map SRV from the pixel shader
	void releaseShadowMap();


	// cbPerFrame

	// set the parallel lights
	void setParallelLights(const std::vector<Light> &parallelLights);	// no. of lights can be determined from the vector size

	// set the point lights
	void setPointLights(const std::vector<Light> &pointLights);

	// set the spot lights
	void setSpotLights(const std::vector<Light> &spotLights);

	// set the world space eye pos
	void setEyePosW(const DXVector3 &eyePosW);

	// set the diffuse texture map
	void setDiffuseMap(const ShaderResourceViewPtr &pDiffuseMap);

	// set the normal map
	void setNormalMap(const ShaderResourceViewPtr &pNormalMap);

	// set the specular light map
	void setSpecularMap(const ShaderResourceViewPtr &pSpecularMap);
#endif	// 0

	// TODO: add shader param mutator funcs here

protected:

	// NOTE: Not implementing the set* accessor methods as all the shader param setting will be handled inside the alternate apply method.

	// All the constant buffers across all shaders are set. (pure virtual, must be implemented by derived class)
	/*virtual*/ void setShaderConstantBuffers() /*=0*/	{}

	// All the shader textures(resources) are set. (pure virtual, must be implemented by derived class)
	/*virtual*/ void setShaderResources() /*=0*/	{}

	// All the shader samplers are set. (pure virtual, must be implemented by derived class)
	/*virtual*/ void setShaderSamplers() /*=0*/	{}

	// All the shaders are set. (pure virtual, must be implemented by derived class)
	/*virtual*/ void setShaders() /*=0*/	{}

	// Creates all the shaders required for the effect, basically calls to create*Shader. (pure virtual, must be implemented by derived class)
	/*virtual*/ void createShaders() /*=0*/;

	// Initializes the input layout for the effect. (pure virtual, must be implemented by derived class)
	/*virtual*/ void initInputLayout() /*= 0*/;

	// Creates and initializes the constant buffers of all shaders. (pure virtual, must be implemented by derived class)
	/*virtual*/ void initShaderConstantBuffers() /*=0*/;

	// Initializes the resources of all shaders. (pure virtual, must be implemented by derived class)
	/*virtual*/ void initShaderResources() /*=0*/;

	// Initializes the sampler states of all shaders. (pure virtual, must be implemented by derived class)
	/*virtual*/ void initShaderSamplers() /*=0*/;

	// Returns the structure count of the given unordered access view.
	UINT getStructureCount(ID3D11UnorderedAccessView *pUAV);

	// Displays the structure count of the current and new simulation states.
	void displaySimStates();

protected:

	// Shaders and their respective blobs.
	std::map<std::string, VertexShaderPtr> m_vertexShaders;
	std::map<std::string, GeometryShaderPtr> m_geometryShaders;
	std::map<std::string, PixelShaderPtr> m_pixelShaders;
	std::map<std::string, ComputeShaderPtr> m_computeShaders;
	std::map<std::string, BlobPtr> m_shaderBlobs;

	// Buffers.
	std::map<std::string, BufferPtr> m_buffers;
	std::map<std::string, ShaderConstantBuffer> m_constantBuffers;
	ShaderStructuredBuffer<Particle> m_particlesBuffer, m_particlesBuffer2;

	// Current and next simulation states for the particle system.
	ShaderStructuredBuffer<Particle> *m_pCurrentSimParticles, *m_pNewSimParticles;

	// Sampler states.
	std::map<std::string, SamplerStatePtr> m_samplerStates;

	// Shader resources
	std::map<std::string, ShaderResource> m_resources;

	// Particle system initialization params.
	EffectParams m_initParams;

	// Max no. of particles in the system.
	UINT m_nMaxParticles;

	// No. of particles updated at a time (NOTE: Has to match the value specified in particleSystemUpdateCS)
	UINT m_nUpdateParticles;

	// No. of particles spawned at a time (NOTE: Has to match the value specified in the particleSystemInsertCS)
	UINT m_nSpawnParticles;

	// Max lifetime of each particle.
	//float m_fLifetime;	// @TODO: remove when done testing

	// Current timestep which is used to update each particle's age.
	float m_fTimeStep;

	// Interval (and the corresponding counter) at which new particles are to be spawned.
	float m_fSpawnTime, m_fSpawnStep;

	// Default particle colour.
	DXColor m_particlesColour;

	// Workarounds to keep the getVSBuffer(Pointer/Size) accessor methods happy
	LPCVOID m_pVSBuffer;
	UINT m_nVSBufferSize;

	// @TODO: keep the ones we need
#if 0
	// First pass shadow map generation vertex shader
	VertexShaderPtr m_pFirstPassVertexShader;
	BlobPtr m_pFirstPassVertexByteCode;
	ShaderConstantBuffer m_cbPerObjectFirstPass;

	// First pass shadow map generation pixel shader
	PixelShaderPtr m_pFirstPassDummyPixelShader;
	BlobPtr m_pFirstPassDummyPixelByteCode;

	// Second pass shadow map generation vertex shader
	VertexShaderPtr m_pSecondPassVertexShader;
	BlobPtr m_pSecondPassVertexByteCode;
	ShaderConstantBuffer m_cbPerObjectSecondPass;

	// Second pass simple shadow map generation pixel shader
	PixelShaderPtr m_pSecondPassSimplePixelShader;
	BlobPtr m_pSecondPassSimplePixelByteCode;
	ShaderConstantBuffer m_cbPerFrameSecondPassSimple;
	ShaderResource m_resDiffuseMap, m_resSpecularMap, m_resNormalMap, m_resShadowMap;
	ShaderSamplerState m_samTriLinear, m_samShadow;
	UINT m_iSamplerStartBindPoint;
	std::vector<ID3D11SamplerState*> m_ppSamplers;

	// Second pass PCF based shadow map generation pixel shader
	PixelShaderPtr m_pSecondPassPCFPixelShader;
	BlobPtr m_pSecondPassPCFPixelByteCode;
	ShaderConstantBuffer m_cbPerFrameSecondPassPCF;
	ShaderResource m_resDiffuseMapPCF, m_resSpecularMapPCF, m_resNormalMapPCF, m_resShadowMapPCF;
	ShaderSamplerState m_samTriLinearPCF, m_samShadowPCF;
	UINT m_iSamplerStartBindPointPCF;
	std::vector<ID3D11SamplerState*> m_ppSamplersPCF;

	// Current pass being rendered
	EffectPass m_eCurrentPass;

	// Current filtering method
	EffectTechnique m_eCurrentTechnique;

	// TODO: really need to figure this out
	static const int nMaxSpotLights;
#endif // 0

};
typedef std::shared_ptr<ParticleSystemEffect> ParticleSystemEffectPtr;

// Pairing between a particle system and its initial state.
struct ParticleSystemInstance
{
	ParticleSystemEffect::EffectParams params;
	ParticleSystemEffectPtr pSystem;
};	// end of struct ParticleSystemInstance
typedef std::map<std::string, ParticleSystemInstance> ParticleSystemDirectory;

#endif	// PARTICLE_SYSTEM_EFFECT_H