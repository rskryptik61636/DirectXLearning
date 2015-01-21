// Header file for the SceneBuilder class
#ifndef SCENE_BUILDER_H
#define SCENE_BUILDER_H

#include <rapidxml.hpp>

#include "d3dUtil.h"

#include "DXCamera.h"

#include "Light.h"

#include <DXStringUtils.h>

#include <DXModel.h>

#include <ParticleSystemEffect.h>

// Builds scene entities from a given scene description file
class SceneBuilder
{
public:

	// Param ctor
	SceneBuilder(const std::string strSceneFilePath);

	// Copy ctor
	SceneBuilder(SceneBuilder &sceneBuilder);

	// @TODO: need to refactor the build functions to accept params via structs
	// Preferably, the structs should be defined within their respective
	// component's classes

	// Builds a camera from the given scene file using the given default params
	// if the corresponding params were not found in the scene file
	void buildCamera(DXCameraPtr &pCamera, const float aspectRatio, 
					const DXVector3 defaultEyePos = DXVector3(0.0f, 20.0f, -50.0f), 
					const DXVector3 defaultLookAt = DXVector3(0.0f, 0.0f, 0.0f),
					const DXVector3 defaultUp = DXVector3(0.0f, 1.0f, 0.0f),
					const float defaultFOVAngle = 0.25f * PI,
					const float defaultNearPlane = 1.0f,
					const float defaultFarPlane = 1000.0f,
					const float defaultMotionFactor = 50.0f,
					const float defaultRotationFactor = 1.0f);

	// Builds parallel lights from the given scene file
	template<typename LightT>
	void buildParallelLights(const ParallelLightParams &defaultParams, std::vector<LightT> &parallelLights);
	
	// Builds point lights from the given scene file
	template<typename LightT>
	void buildPointLights(const PointLightParams &defaultParams, std::vector<LightT> &pointLights);
	
	// Builds spot lights from the given scene file
	template<typename LightT>
	void buildSpotLights(const SpotLightParams &defaultParams, std::vector<LightT> &spotLights);

	// Builds models from the given scene file
	void buildBasicModels(BasicModelDirectory &basicModelDirectory);

	// Builds particle systems from the given scene file.
	void buildParticleSystems(const wpath &shaderRoot, const DevicePtr &pDevice, const DeviceContextPtr &pDeviceContext, ParticleSystemDirectory &particleSystemDirectory);

	// Gets the texture root path (wide-string, returns empty if not found)
	const wpath getTextureRootW();

	// Gets the shader root path (wide-string, returns empty if not found)
	const wpath getShaderRootW();

	// Gets the name of the scene (wide-string, returns empty if not found)
	const std::string getSceneName();

private:

	// Initializes the scene builder
	void init();

	// Utility function to find a given XML node in the scene file
	rapidxml::xml_node<>* findNode(const char *pszNode, /*const*/ rapidxml::xml_node<> **ppParentNode);

private:

	// scene file path
	std::string m_strSceneFilePath;

	// scene file contents
	std::string m_strSceneFileContents;

	// pointer to the top-level Scene node in the scene file
	rapidxml::xml_node<> *m_pSceneNode;

};	// end of class SceneBuilder
typedef std::unique_ptr<SceneBuilder> SceneBuilderPtr;

// Builds parallel lights from the given scene file
template<typename LightT>
void SceneBuilder::buildParallelLights(const ParallelLightParams &defaultParams, std::vector<LightT> &parallelLights)
{
	// Find the Lights node
	rapidxml::xml_node<> *pLightsNode = findNode("Lights", &m_pSceneNode);
	for (rapidxml::xml_node<> *pNode = pLightsNode->first_node(); pNode; pNode = pNode->next_sibling())
	{
		if (!_stricmp(pNode->name(), "ParallelLight"))	// Create a parallel light out of the current node
		{
			// Init the current set of parallel light params to the default params
			ParallelLightParams parallelLightParams(defaultParams);

			for (rapidxml::xml_attribute<> *pAttr = pNode->first_attribute(); pAttr; pAttr = pAttr->next_attribute())
			{
				if (!_stricmp(pAttr->name(), "Direction"))
				{
					stringToThreeVector(pAttr->value(), parallelLightParams.dir);
				}
				else if (!_stricmp(pAttr->name(), "Ambient"))
				{
					stringToColor(pAttr->value(), parallelLightParams.ambient);
				}
				else if (!_stricmp(pAttr->name(), "Diffuse"))
				{
					stringToColor(pAttr->value(), parallelLightParams.diffuse);
				}
				else if (!_stricmp(pAttr->name(), "Specular"))
				{
					stringToColor(pAttr->value(), parallelLightParams.specular);
				}
				else if (!_stricmp(pAttr->name(), "Radius"))
				{
					parallelLightParams.radius = static_cast<float>(atof(pAttr->value()));
				}
				else if (!_stricmp(pAttr->name(), "Angle"))
				{
					parallelLightParams.angle = static_cast<float>(atof(pAttr->value()));
				}
				else if (!_stricmp(pAttr->name(), "AnimationSpeed"))
				{
					parallelLightParams.animationSpeed = static_cast<float>(atof(pAttr->value()));
				}
			}
			LightT parallelLight;
			parallelLight.dir = parallelLightParams.dir;
			parallelLight.ambient = parallelLightParams.ambient;
			parallelLight.diffuse = parallelLightParams.diffuse;
			parallelLight.specular = parallelLightParams.specular;
			parallelLight.radius = parallelLightParams.radius;
			parallelLight.angle = parallelLightParams.angle;
			parallelLight.animationSpeed = parallelLightParams.animationSpeed;
			parallelLights.push_back(parallelLight);
		}
	}
}

// Builds point lights from the given scene file
template<typename LightT>
void SceneBuilder::buildPointLights(const PointLightParams &defaultParams, std::vector<LightT> &pointLights)
{
	// Find the Lights node
	rapidxml::xml_node<> *pLightsNode = findNode("Lights", &m_pSceneNode);
	for (rapidxml::xml_node<> *pNode = pLightsNode->first_node(); pNode; pNode = pNode->next_sibling())
	{
		if (!_stricmp(pNode->name(), "PointLight"))	// Create a point light out of the current node
		{
			// Init the current set of point light params to the default params
			PointLightParams pointLightParams(defaultParams);

			for (rapidxml::xml_attribute<> *pAttr = pNode->first_attribute(); pAttr; pAttr = pAttr->next_attribute())
			{
				if (!_stricmp(pAttr->name(), "Position"))
				{
					stringToThreeVector(pAttr->value(), pointLightParams.position);
				}
				else if (!_stricmp(pAttr->name(), "Ambient"))
				{
					stringToColor(pAttr->value(), pointLightParams.ambient);
				}
				else if (!_stricmp(pAttr->name(), "Diffuse"))
				{
					stringToColor(pAttr->value(), pointLightParams.diffuse);
				}
				else if (!_stricmp(pAttr->name(), "Specular"))
				{
					stringToColor(pAttr->value(), pointLightParams.specular);
				}
				else if (!_stricmp(pAttr->name(), "Attenuation"))
				{
					stringToThreeVector(pAttr->value(), pointLightParams.att);
				}
				else if (!_stricmp(pAttr->name(), "Range"))
				{
					pointLightParams.range = static_cast<float>(atof(pAttr->value()));
				}
				else if (!_stricmp(pAttr->name(), "Radius"))
				{
					pointLightParams.radius = static_cast<float>(atof(pAttr->value()));
				}
				else if (!_stricmp(pAttr->name(), "Angle"))
				{
					pointLightParams.angle = static_cast<float>(atof(pAttr->value()));
				}
				else if (!_stricmp(pAttr->name(), "AnimationSpeed"))
				{
					pointLightParams.animationSpeed = static_cast<float>(atof(pAttr->value()));
				}
			}
			LightT pointLight;
			pointLight.position = pointLightParams.position;
			pointLight.ambient = pointLightParams.ambient;
			pointLight.diffuse = pointLightParams.diffuse;
			pointLight.specular = pointLightParams.specular;
			pointLight.att = pointLightParams.att;
			pointLight.range = pointLightParams.range;
			pointLight.radius = pointLightParams.radius;
			pointLight.angle = pointLightParams.angle;
			pointLight.animationSpeed = pointLightParams.animationSpeed;
			pointLights.push_back(pointLight);
		}
	}
}

// Builds spot lights from the given scene file
template<typename LightT>
void SceneBuilder::buildSpotLights(const SpotLightParams &defaultParams, std::vector<LightT> &spotLights)
{
	// Find the Lights node
	rapidxml::xml_node<> *pLightsNode = findNode("Lights", &m_pSceneNode);
	for (rapidxml::xml_node<> *pNode = pLightsNode->first_node(); pNode; pNode = pNode->next_sibling())
	{
		if (!_stricmp(pNode->name(), "SpotLight"))	// Create a spot light out of the current node
		{
			// Init the current set of spot light params to the default params
			SpotLightParams spotLightParams(defaultParams);

			for (rapidxml::xml_attribute<> *pAttr = pNode->first_attribute(); pAttr; pAttr = pAttr->next_attribute())
			{
				if (!_stricmp(pAttr->name(), "Position"))
				{
					stringToThreeVector(pAttr->value(), spotLightParams.position);
				}
				else if (!_stricmp(pAttr->name(), "LookAt"))	// Compute the spot light's direction: normalize(lookAt - position) [WARNING: Position must be defined before LookAt in the XML file]
				{
					DXVector3 lookAt;
					stringToThreeVector(pAttr->value(), lookAt);
					DXVector3 dir(lookAt - spotLightParams.position);
					dir.Normalize();
					spotLightParams.dir = dir;
				}
				else if (!_stricmp(pAttr->name(), "Ambient"))
				{
					stringToColor(pAttr->value(), spotLightParams.ambient);
				}
				else if (!_stricmp(pAttr->name(), "Diffuse"))
				{
					stringToColor(pAttr->value(), spotLightParams.diffuse);
				}
				else if (!_stricmp(pAttr->name(), "Specular"))
				{
					stringToColor(pAttr->value(), spotLightParams.specular);
				}
				else if (!_stricmp(pAttr->name(), "Attenuation"))
				{
					stringToThreeVector(pAttr->value(), spotLightParams.att);
				}
				else if (!_stricmp(pAttr->name(), "Range"))
				{
					spotLightParams.range = static_cast<float>(atof(pAttr->value()));
				}
				else if (!_stricmp(pAttr->name(), "Power"))
				{
					spotLightParams.spotPow = static_cast<float>(atof(pAttr->value()));
				}
				else if (!_stricmp(pAttr->name(), "Radius"))
				{
					spotLightParams.radius = static_cast<float>(atof(pAttr->value()));
				}
				else if (!_stricmp(pAttr->name(), "Angle"))
				{
					spotLightParams.angle = static_cast<float>(atof(pAttr->value()));
				}
				else if (!_stricmp(pAttr->name(), "AnimationSpeed"))
				{
					spotLightParams.animationSpeed = static_cast<float>(atof(pAttr->value()));
				}
			}
			LightT spotLight;
			spotLight.position = spotLightParams.position;
			spotLight.dir = spotLightParams.dir;
			spotLight.ambient = spotLightParams.ambient;
			spotLight.diffuse = spotLightParams.diffuse;
			spotLight.specular = spotLightParams.specular;
			spotLight.att = spotLightParams.att;
			spotLight.range = spotLightParams.range;
			spotLight.spotPow = spotLightParams.spotPow;
			spotLight.radius = spotLightParams.radius;
			spotLight.angle = spotLightParams.angle;
			spotLight.animationSpeed = spotLightParams.animationSpeed;
			spotLights.push_back(spotLight);
		}
	}
}

#endif	// SCENE_BUILDER_H