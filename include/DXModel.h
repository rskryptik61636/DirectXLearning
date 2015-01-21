// Header file containing classes which are used to handle mesh models.
#ifndef DX_MODEL_H
#define DX_MODEL_H

#include "ObjectV2.h"

// @TODO: consider forward declaring Assimp classes
#include "AssimpInclude.h"

// Represents a mesh subset of a model. It contains vertex and index counts and references
// into the model's vertex and index lists.
struct DXModelMesh	
{
	UINT vertexCount, vertexStart, indexCount, indexStart, materialIndex;
};	// end of struct DXMesh

// Represents a node of a model. A node consists of one or more meshes and one or more children nodes.
struct DXModelNode
{
	DXMatrix transformation;
	std::vector<UINT> meshIndexes;
	std::vector<DXModelNode> childNodes;
};

// Represents a basic model w/o animation capabilities.
class DXBasicModel : public ObjectV2<Vertex>
{
public:

	// Param ctor.
	// @param modelPath	Absolute path of the model file.
	DXBasicModel(const npath modelPath, const unsigned int uiImportOptions = aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder |
																			aiProcess_FlipUVs | aiProcessPreset_TargetRealtime_Quality);

	// Dtor.
	~DXBasicModel();

	void initVertexBuffer()	{}		// pure virtual func to define vertex list (not implementing since the vertex and list will be populated in the init function)
	void initIndexBuffer()	{}			// pure virtual func to define index list (not implementing since the vertex and list will be populated in the init function)

	// Overriding the init function to populate it with the contents of the aiScene object.
	void init(ID3D11Device *device, float scale);

	// This version of the draw method will not be used.
	void draw();

	// Draws the specified mesh.
	void draw(const UINT uiMeshIndex);

	// Accessor function for the model's root node. Required to traverse the node tree in order to draw the model.
	const DXModelNode& rootNode() const	{ return m_modelRootNode; }

	// Accessor function to return the material index of the given mesh.
	const UINT getMaterialIndex(const UINT uiMeshIndex)	const	{ return m_modelMeshes[uiMeshIndex].materialIndex; }

	// Accessor function which returns the diffuse texture corresponding to the given mesh material index.
	const ShaderResourceViewPtr getDiffuseTexture(const UINT uiMeshMaterialIndex);

	// Accessor function which returns the specular texture corresponding to the given mesh material index.
	const ShaderResourceViewPtr getSpecularTexture(const UINT uiMeshMaterialIndex);

	// Accessor function which returns the normal texture corresponding to the given mesh material index.
	const ShaderResourceViewPtr getNormalTexture(const UINT uiMeshMaterialIndex);

protected:

	// Recursive function which constructs the model's node tree.
	void recursiveNodeConstructor(aiNode *pCurrNode, DXModelNode &currNode);

protected:

	// Model path
	npath m_modelPath;

	// Model
	std::unique_ptr<const aiScene> m_pModel;
	/*const aiScene *m_pModel;*/

	// Model textures
	std::map<std::string, ShaderResourceViewPtr> m_modelDiffuseTextures, m_modelSpecularTextures, m_modelNormalTextures;

	// Model meshes
	std::vector<DXModelMesh> m_modelMeshes;

	// Model root node
	DXModelNode m_modelRootNode;

};	// end of class DXBasicModel
typedef std::shared_ptr<DXBasicModel> BasicModelPtr;	// NOTE: declaring as a shared_ptr since it causes a problem as a unique_ptr
														// when being copy constructed in the SceneBuilder::buildBasicModels method.
		
														// TODO: see if there is a way around this if it causes performance issues.

// Represents an instance of a basic model
struct BasicModelInstance
{
	// Method to return the composite transformation matrix
	DXMatrix getTransform();

	BasicModelPtr pModel;	// model
	DXVector3 position;	// world space position
	DXVector3 scale;	// model scale
	DXVector4 orientation;	// model orientation (nx, ny, nz, angle)
};	// end of struct BasicModelInstance
typedef std::map<std::string, BasicModelInstance> BasicModelDirectory;	// Associative array to map names to BasicModelInstance-s

#endif	// DX_MODEL_H