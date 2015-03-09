// Source file for the SceneBuilder class

// SceneBuilder header file
#include "SceneBuilder.h"

// String utils header file
#include <DXStringUtils.h>

// Stringstream class for stream parsing
#include <sstream>

// Param ctor
SceneBuilder::SceneBuilder(const std::string strSceneFilePath) : m_strSceneFilePath(strSceneFilePath)
{
	// invoke the init function
	init();
}

// Copy ctor (invoke the param ctor using a delegating ctor (https://en.wikipedia.org/wiki/C++11#Object_construction_improvement))
SceneBuilder::SceneBuilder(SceneBuilder &sceneBuilder) : SceneBuilder(sceneBuilder.m_strSceneFilePath)
{
	
}

// Builds a camera from the given scene file using the given default params
// if the corresponding params were not found in the scene file
void SceneBuilder::buildCamera(DXCameraPtr &pCamera, const float aspectRatio,
								const DXVector3 defaultEyePos /*= DXVector3(0.0f, 20.0f, -50.0f)*/,
								const DXVector3 defaultLookAt /*= DXVector3(0.0f, 0.0f, 0.0f)*/,
								const DXVector3 defaultUp /*= DXVector3(0.0f, 1.0f, 0.0f)*/,
								const float defaultFOVAngle /*= 0.25f * PI*/,
								const float defaultNearPlane /*= 1.0f*/,
								const float defaultFarPlane /*= 1000.0f*/,
								const float defaultMotionFactor /*= 50.0f*/,
								const float defaultRotationFactor /*= 1.0f*/)
{
	// set the default camera params as provided
	DXVector3 eyePos(defaultEyePos), lookAt(defaultLookAt), up(defaultUp);
	float fovAngle(defaultFOVAngle), nearPlane(defaultNearPlane), farPlane(defaultFarPlane), 
			motionFactor(defaultMotionFactor), rotationFactor(defaultRotationFactor);

	// find the Camera node
	rapidxml::xml_node<> *cameraNode = findNode("Camera", &m_pSceneNode);
	if (cameraNode)	// proceed iff the camera node was found
	{
		for (rapidxml::xml_attribute<> *attr = cameraNode->first_attribute(); attr; attr = attr->next_attribute())
		{
			if (isEqual(attr->name(), "Position"))
			{
				stringToThreeVector(attr->value(), eyePos);
			}
			else if (isEqual(attr->name(), "LookAt"))
			{
				stringToThreeVector(attr->value(), lookAt);
			}
			else if (isEqual(attr->name(), "Up"))
			{
				stringToThreeVector(attr->value(), up);
			}
			else if (isEqual(attr->name(), "FieldOfView"))
			{
				fovAngle = static_cast<float>(atof(attr->value()) * PI / 180.0f);	// need to convert from degrees into radians
			}
			else if (isEqual(attr->name(), "NearPlaneDist"))
			{
				nearPlane = static_cast<float>(atof(attr->value()));
			}
			else if (isEqual(attr->name(), "FarPlaneDist"))
			{
				farPlane = static_cast<float>(atof(attr->value()));
			}
			else if (isEqual(attr->name(), "MotionFactor"))
			{
				motionFactor = static_cast<float>(atof(attr->value()));
			}
			else if (isEqual(attr->name(), "RotationFactor"))
			{
				rotationFactor = static_cast<float>(atof(attr->value()));
			}
		}
	}

	// init the camera object
	pCamera.reset(new DXCamera(eyePos, lookAt, up, fovAngle, aspectRatio, nearPlane, farPlane, motionFactor, rotationFactor));
}

// Builds models from the given scene file
void SceneBuilder::buildBasicModels(BasicModelDirectory &basicModelDirectory)
{
	// Find the Objects node
	rapidxml::xml_node<> *pObjectsNode = findNode("Objects", &m_pSceneNode);
	for (rapidxml::xml_node<> *pNode = pObjectsNode->first_node(); pNode; pNode = pNode->next_sibling())
	{
		if (isEqual(pNode->name(), "BasicModel"))	// Parse the current node if it is a BasicModel node
		{
			BasicModelInstance basicModelInstance;
			std::string strModelName;
			for (rapidxml::xml_attribute<> *pAttr = pNode->first_attribute(); pAttr; pAttr = pAttr->next_attribute())
			{
				if (isEqual(pAttr->name(), "Name"))
				{
					strModelName = pAttr->value();
				}
				else if (isEqual(pAttr->name(), "Source"))
				{
					basicModelInstance.pModel.reset(new DXBasicModel(pAttr->value()));					
				}
				else if (isEqual(pAttr->name(), "Position"))
				{
					DXVector3 pos;
					stringToThreeVector(pAttr->value(), pos);
					basicModelInstance.position = pos;
				}
				else if (isEqual(pAttr->name(), "Scale"))
				{
					DXVector3 scale;
					stringToThreeVector(pAttr->value(), scale);
					basicModelInstance.scale = scale;
				}
				else if (isEqual(pAttr->name(), "Orientation"))
				{
					DXVector4 orient;
					stringToFourVector(pAttr->value(), orient);
					basicModelInstance.orientation = orient;
				}
			}
			basicModelDirectory[strModelName] = basicModelInstance;	// Add the created basic model instance to the directory.
		}
	}
}

// Builds particle systems from the given scene file.
void SceneBuilder::buildParticleSystems(const wpath &shaderRoot, const DevicePtr &pDevice, 
	const DeviceContextPtr &pDeviceContext, ParticleSystemDirectory &particleSystemDirectory)
{
	// Find the ParticleSystems node.
	rapidxml::xml_node<> *pRootNode = findNode("ParticleSystems", &m_pSceneNode);
	for (rapidxml::xml_node<> *pNode = pRootNode->first_node(); pNode; pNode = pNode->next_sibling())
	{
		// Create a ParticleSystemInstance for the current iteration.
		ParticleSystemInstance currInstance;
		ParticleSystemEffect::EffectParams &currParams = currInstance.params;

		// Create a rain particle system.
		if (isEqual(pNode->name(), "Rain"))
		{
			// Set the technique accordingly.
			currParams.eTech = ParticleSystemEffect::ET_RAIN;

			// Name of the current instance.
			std::string strName("");

			// Max no. of particles in the system.
			UINT nMaxParticles(0);

			// Iterate over all the attributes of the current node and set the particle system params accordingly.
			for (rapidxml::xml_attribute<> *pAttr = pNode->first_attribute(); pAttr; pAttr = pAttr->next_attribute())
			{
				if (isEqual(pAttr->name(), "Name"))	// Set the name of the current instance.
				{
					strName = pAttr->value();
				}
				else if (isEqual(pAttr->name(), "MaxParticles"))
				{
					nMaxParticles = static_cast<UINT>(atoi(pAttr->value()));
				}
				else if (isEqual(pAttr->name(), "RenderingTechnique"))
				{
					if (isEqual(pAttr->value(), "Forward"))
					{
						currParams.eRenderTech = ParticleSystemEffect::RT_FORWARD;
					}
					else if (isEqual(pAttr->value(), "Deferred"))
					{
						currParams.eRenderTech = ParticleSystemEffect::RT_DEFERRED;
					}
				}
				else if (isEqual(pAttr->name(), "Lifetime"))	// Set the particle lifetime.
				{
					currParams.lifetime = static_cast<float>(atof(pAttr->value()));
				}
				else if (isEqual(pAttr->name(), "Size"))	// Set the particle size.
				{
					currParams.size = stringToThreeVector(pAttr->value());
				}
				else if (isEqual(pAttr->name(), "EmitterMinPosW"))	// Set the particle emission area's min pos.
				{
					currParams.emitterMinPosW = stringToThreeVector(pAttr->value());
				}
				else if (isEqual(pAttr->name(), "EmitterMaxPosW"))	// Set the particle emission area's max pos.
				{
					currParams.emitterMaxPosW = stringToThreeVector(pAttr->value());
				}
				else if (isEqual(pAttr->name(), "InitialAcceleration"))	// Set the initial acceleration for all particles.
				{
					currParams.initialAcceleration = stringToThreeVector(pAttr->value());
				}
				else if (isEqual(pAttr->name(), "InitialVelocity"))	// Set the initial velocity of all particles.
				{
					currParams.initialVelocity = stringToThreeVector(pAttr->value());
				}
			}

			// Ensure that the particle system params are valid.
			assert(!strName.empty());
			assert(nMaxParticles != 0);
			assert(currParams.eTech != ParticleSystemEffect::ET_UNKNOWN);

			// Init the particle system with the given params.
			currInstance.pSystem.reset(new ParticleSystemEffect(shaderRoot, pDevice, pDeviceContext, nMaxParticles, currParams));
			currInstance.pSystem->init();

			// Add the current rain effect to the particle system directory.
			particleSystemDirectory[strName] = currInstance;
		}
		// @TODO: Add support for additional particle systems here.
		
	}
}

// @TODO: remove when done testing
#if 0

// Builds parallel lights from the given scene file
void SceneBuilder::buildParallelLights(const ParallelLightParams &defaultParams, std::vector<Light> &parallelLights)
{
	// Find the Lights node
	rapidxml::xml_node<> *pLightsNode = findNode("Lights", &m_pSceneNode);
	for (rapidxml::xml_node<> *pNode = pLightsNode->first_node(); pNode; pNode = pNode->next_sibling())
	{
		if (isEqual(pNode->name(), "ParallelLight"))	// Create a parallel light out of the current node
		{
			// Init the current set of parallel light params to the default params
			ParallelLightParams parallelLightParams(defaultParams);

			for (rapidxml::xml_attribute<> *pAttr = pNode->first_attribute(); pAttr; pAttr = pAttr->next_attribute())
			{
				if (isEqual(pAttr->name(), "Direction"))
				{
					stringToThreeVector(pAttr->value(), parallelLightParams.dir);
				}
				else if (isEqual(pAttr->name(), "Ambient"))
				{
					stringToColor(pAttr->value(), parallelLightParams.ambient);
				}
				else if (isEqual(pAttr->name(), "Diffuse"))
				{
					stringToColor(pAttr->value(), parallelLightParams.diffuse);
				}
				else if (isEqual(pAttr->name(), "Specular"))
				{
					stringToColor(pAttr->value(), parallelLightParams.specular);
				}
			}
			Light parallelLight;
			parallelLight.dir = parallelLightParams.dir;
			parallelLight.ambient = parallelLightParams.ambient;
			parallelLight.diffuse = parallelLightParams.diffuse;
			parallelLight.specular = parallelLightParams.specular;
			parallelLights.push_back(parallelLight);
		}
	}
}

// Builds point lights from the given scene file
void SceneBuilder::buildPointLights(const PointLightParams &defaultParams, std::vector<Light> &pointLights)
{
	// Find the Lights node
	rapidxml::xml_node<> *pLightsNode = findNode("Lights", &m_pSceneNode);
	for (rapidxml::xml_node<> *pNode = pLightsNode->first_node(); pNode; pNode = pNode->next_sibling())
	{
		if (isEqual(pNode->name(), "PointLight"))	// Create a point light out of the current node
		{
			// Init the current set of point light params to the default params
			PointLightParams pointLightParams(defaultParams);

			for (rapidxml::xml_attribute<> *pAttr = pNode->first_attribute(); pAttr; pAttr = pAttr->next_attribute())
			{
				if (isEqual(pAttr->name(), "Position"))
				{
					stringToThreeVector(pAttr->value(), pointLightParams.position);
				}
				else if (isEqual(pAttr->name(), "Ambient"))
				{
					stringToColor(pAttr->value(), pointLightParams.ambient);
				}
				else if (isEqual(pAttr->name(), "Diffuse"))
				{
					stringToColor(pAttr->value(), pointLightParams.diffuse);
				}
				else if (isEqual(pAttr->name(), "Specular"))
				{
					stringToColor(pAttr->value(), pointLightParams.specular);
				}
				else if (isEqual(pAttr->name(), "Attenuation"))
				{
					stringToThreeVector(pAttr->value(), pointLightParams.att);
				}
				else if (isEqual(pAttr->name(), "Range"))
				{
					pointLightParams.range = static_cast<float>(atof(pAttr->value()));
				}
			}
			Light pointLight;
			pointLight.position = pointLightParams.position;
			pointLight.ambient = pointLightParams.ambient;
			pointLight.diffuse = pointLightParams.diffuse;
			pointLight.specular = pointLightParams.specular;
			pointLight.att = pointLightParams.att;
			pointLight.range = pointLightParams.range;
			pointLights.push_back(pointLight);
		}
	}
}

// Builds spot lights from the given scene file
void SceneBuilder::buildSpotLights(const SpotLightParams &defaultParams, std::vector<Light> &spotLights)
{
	// Find the Lights node
	rapidxml::xml_node<> *pLightsNode = findNode("Lights", &m_pSceneNode);
	for (rapidxml::xml_node<> *pNode = pLightsNode->first_node(); pNode; pNode = pNode->next_sibling())
	{
		if (isEqual(pNode->name(), "SpotLight"))	// Create a spot light out of the current node
		{
			// Init the current set of spot light params to the default params
			SpotLightParams spotLightParams(defaultParams);

			for (rapidxml::xml_attribute<> *pAttr = pNode->first_attribute(); pAttr; pAttr = pAttr->next_attribute())
			{
				if (isEqual(pAttr->name(), "Position"))
				{
					stringToThreeVector(pAttr->value(), spotLightParams.position);
				}
				else if (isEqual(pAttr->value(), "Direction"))
				{
					stringToThreeVector(pAttr->value(), spotLightParams.dir);
				}
				else if (isEqual(pAttr->name(), "Ambient"))
				{
					stringToColor(pAttr->value(), spotLightParams.ambient);
				}
				else if (isEqual(pAttr->name(), "Diffuse"))
				{
					stringToColor(pAttr->value(), spotLightParams.diffuse);
				}
				else if (isEqual(pAttr->name(), "Specular"))
				{
					stringToColor(pAttr->value(), spotLightParams.specular);
				}
				else if (isEqual(pAttr->name(), "Attenuation"))
				{
					stringToThreeVector(pAttr->value(), spotLightParams.att);
				}
				else if (isEqual(pAttr->name(), "Range"))
				{
					spotLightParams.range = static_cast<float>(atof(pAttr->value()));
				}
				else if (isEqual(pAttr->name(), "Power"))
				{
					spotLightParams.spotPow = static_cast<float>(atof(pAttr->value()));
				}
			}
			Light spotLight;
			spotLight.position = spotLightParams.position;
			spotLight.dir = spotLightParams.dir;
			spotLight.ambient = spotLightParams.ambient;
			spotLight.diffuse = spotLightParams.diffuse;
			spotLight.specular = spotLightParams.specular;
			spotLight.att = spotLightParams.att;
			spotLight.range = spotLightParams.range;
			spotLight.spotPow = spotLightParams.spotPow;
			spotLights.push_back(spotLight);
		}
	}
}

#endif	// 0

// Initializes the scene builder
void SceneBuilder::init()
{
	// read the scene file's contents into a string
	std::ifstream sceneFile(m_strSceneFilePath.c_str());
	if (!sceneFile.is_open())
	{
		throw std::exception(std::string("Unable to open " + m_strSceneFilePath).c_str());
	}
	sceneFile.seekg(0, std::ios::end);
	m_strSceneFileContents.reserve(static_cast<std::size_t>(sceneFile.tellg()));
	sceneFile.seekg(0, std::ios::beg);
	m_strSceneFileContents.assign((std::istreambuf_iterator<char>(sceneFile)), std::istreambuf_iterator<char>());

	// find the top-level SceneNode
	rapidxml::xml_document<> sceneDoc;
	sceneDoc.parse<0>(const_cast<char*>(m_strSceneFileContents.c_str()));
	m_pSceneNode = sceneDoc.first_node("Scene");
	assert(m_pSceneNode != NULL);	// @TODO: find out why HR doesn't work in this case
}

// Utility function to find a given XML node in the scene file
rapidxml::xml_node<>* SceneBuilder::findNode(const char *pszNode, rapidxml::xml_node<> **ppParentNode)
{
	const rapidxml::xml_node<> *pFirstNode = *ppParentNode;
	rapidxml::xml_node<> *pNode = NULL;
	for (pNode = pFirstNode->first_node(); pNode; pNode = pNode->next_sibling())
	{
		if (isEqual(pNode->name(), pszNode))
		{
			break;
		}
	}

	return pNode;
}

// Gets the texture root path (wide-string, returns empty if not found)
const wpath SceneBuilder::getTextureRootW()
{
	// Init the texture root to empty
	wpath textureRoot(L"");

	// Find the TextureRoot node
	rapidxml::xml_node<> *pTextureRootNode = findNode("TextureRoot", &m_pSceneNode);
	if (pTextureRootNode)
	{
		std::string strTextureRoot(pTextureRootNode->value());
		textureRoot = std::wstring(strTextureRoot.begin(), strTextureRoot.end());
	}

	return textureRoot;
}

// Gets the shader root path (wide-string, returns empty if not found)
const wpath SceneBuilder::getShaderRootW()
{
	// Init the shader root to empty
	wpath shaderRoot(L"");

	// Find the ShaderRoot node
	rapidxml::xml_node<> *pShaderRootNode = findNode("ShaderRoot", &m_pSceneNode);
	if (pShaderRootNode)
	{ 
		const std::string strShaderRoot(pShaderRootNode->value());
		shaderRoot = std::wstring(strShaderRoot.begin(), strShaderRoot.end());
	}

	return shaderRoot;
}

// Gets the name of the scene (wide-string, returns empty if not found)
const std::string SceneBuilder::getSceneName()
{
	// Init the scene name to empty
	std::string strSceneName("");

	// Find the 'name' attribute of the root 'Scene' node
	for (rapidxml::xml_attribute<> *pAttr = m_pSceneNode->first_attribute(); pAttr; pAttr = pAttr->next_attribute())
	{
		if (isEqual(pAttr->name(), "name"))
		{
			strSceneName = pAttr->value();
			break;
		}
	}

	return strSceneName;
}