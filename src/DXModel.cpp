// Source file containing the implementation of classes which are used to handle mesh models.

#include "DXModel.h"

#include "DXShaderUtils.h"

// Param ctor.
// @param modelPath	Absolute path of the model file.
DXBasicModel::DXBasicModel(const npath modelPath, const unsigned int uiImportOptions /*= aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder |
																					aiProcess_FlipUVs | aiProcessPreset_TargetRealtime_Quality*/)
: ObjectV2<Vertex>(), m_modelPath(modelPath)
{
	Assimp::Importer importer;
	importer.ReadFile(m_modelPath.file_string(), uiImportOptions);
	m_pModel.reset(importer.GetOrphanedScene());	// NOTE: V. V. IMP to ensure that GetOrphanedScene() is called as that will result
													// in m_pModel taking ownership of the aiScene object. Otherwise, the aiScene object
													// will be destroyed once it goes out of scope.
	//m_pModel = importer.ReadFile(m_modelPath.file_string(), importOptions);
	if (!m_pModel.get())
	{
		// @TODO: figure out how this needs to be handled
		/*std::cerr << "Error importing " << argv[1] << " because " << importer.GetErrorString() << std::endl;
		return 1;*/
	}	
}

// Dtor.
DXBasicModel::~DXBasicModel()	{}

// Overriding the init function to populate it with the contents of the aiScene object.
void DXBasicModel::init(ID3D11Device *device, float scale)
{
	// Set the internal device and device context pointers.
	mD3dDevice = device;
	mD3dDevice->GetImmediateContext(&mD3dDeviceContext.p);

	// Load all the textures available in the model.
	for (unsigned int i = 0; i < m_pModel->mNumMaterials; ++i)
	{
		aiString strTexPath;
		aiReturn status(AI_SUCCESS);

		for (int iTexIndex(0); m_pModel->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, iTexIndex, &strTexPath) == AI_SUCCESS; ++iTexIndex)
		{
			const std::string currTex(strTexPath.data[0] == '.' ? std::string(&strTexPath.data[2]) : std::string(strTexPath.data));
			const npath currTexPath1(m_modelPath.parent_path() / npath(currTex));
			const wpath currTexPath(narrowToWidePath(currTexPath1));
			HR(loadTexture(mD3dDevice, mD3dDeviceContext, currTexPath, NULL, &m_modelDiffuseTextures[strTexPath.data].p));
		}

		for (int iTexIndex(0); m_pModel->mMaterials[i]->GetTexture(aiTextureType_SPECULAR, iTexIndex, &strTexPath) == AI_SUCCESS; ++iTexIndex)
		{
			const std::string currTex(strTexPath.data[0] == '.' ? std::string(&strTexPath.data[2]) : std::string(strTexPath.data));
			const npath currTexPath1(m_modelPath.parent_path() / npath(currTex));
			const wpath currTexPath(narrowToWidePath(currTexPath1));
			HR(loadTexture(mD3dDevice, mD3dDeviceContext, currTexPath, NULL, &m_modelSpecularTextures[strTexPath.data].p));
		}

		for (int iTexIndex(0); m_pModel->mMaterials[i]->GetTexture(aiTextureType_HEIGHT, iTexIndex, &strTexPath) == AI_SUCCESS; ++iTexIndex)
		{
			const std::string currTex(strTexPath.data[0] == '.' ? std::string(&strTexPath.data[2]) : std::string(strTexPath.data));
			const npath currTexPath1(m_modelPath.parent_path() / npath(currTex));
			const wpath currTexPath(narrowToWidePath(currTexPath1));
			HR(loadTexture(mD3dDevice, mD3dDeviceContext, currTexPath, NULL, &m_modelNormalTextures[strTexPath.data].p));
		}
	}

	// Initialize the vertex and index lists.
	UINT iCurrVertexOffset(0), iCurrIndexOffset(0);
	m_modelMeshes.resize(m_pModel->mNumMeshes);
	for (unsigned int i = 0; i < m_pModel->mNumMeshes; ++i)
	{
		// Add all the vertices of the current mesh.
		const aiMesh *pCurrMesh = m_pModel->mMeshes[i];
		for (unsigned int j = 0; j < pCurrMesh->mNumVertices; ++j)
		{
			Vertex currVertex;
			currVertex.position = DXVector3(pCurrMesh->mVertices[j].x, pCurrMesh->mVertices[j].y, pCurrMesh->mVertices[j].z);

			currVertex.tangent = (pCurrMesh->HasTangentsAndBitangents() ?
				DXVector3(pCurrMesh->mTangents[j].x, pCurrMesh->mTangents[j].y, pCurrMesh->mTangents[j].z) :
				DXVector3(0, 0, 0));

			currVertex.binormal = (pCurrMesh->HasTangentsAndBitangents() ?
				DXVector3(pCurrMesh->mBitangents[j].x, pCurrMesh->mBitangents[j].y, pCurrMesh->mBitangents[j].z) :
				DXVector3(0, 0, 0));

			currVertex.normal = (pCurrMesh->HasNormals() ?
				DXVector3(pCurrMesh->mNormals[j].x, pCurrMesh->mNormals[j].y, pCurrMesh->mNormals[j].z) :
				DXVector3(0, 0, 0));

			// NOTE: only dealing with the first channel of texcoords for now as that is the way it is being dealt with in the demo app
			currVertex.texCoord = (pCurrMesh->HasTextureCoords(0) ?
				DXVector2(pCurrMesh->mTextureCoords[0][j].x, pCurrMesh->mTextureCoords[0][j].y) :
				DXVector2(0, 0));

			// NOTE: only dealing with the first channel of colors for now as that is the way it is being dealt with in the demo app
			currVertex.color = (pCurrMesh->HasVertexColors(0) ?
				DXColor(pCurrMesh->mColors[0][j].r, pCurrMesh->mColors[0][j].g, pCurrMesh->mColors[0][j].b, pCurrMesh->mColors[0][j].a) :
				BLACK);

			mVertexList.push_back(currVertex);
		}

		// Set the vertex count and vertex start index of the current mesh.
		m_modelMeshes[i].vertexCount = pCurrMesh->mNumVertices;
		m_modelMeshes[i].vertexStart = iCurrVertexOffset;
		iCurrVertexOffset += m_modelMeshes[i].vertexCount;

		// Add all the face indices of the current mesh.
		for (unsigned int j = 0; j < pCurrMesh->mNumFaces; ++j)
		{
			const aiFace &currFace = pCurrMesh->mFaces[j];
			for (unsigned int k = 0; k < currFace.mNumIndices; ++k)
				mIndexList.push_back(currFace.mIndices[k]);
		}

		// Set the index count and start index of the current mesh.
		m_modelMeshes[i].indexCount = pCurrMesh->mNumFaces * pCurrMesh->mFaces[0].mNumIndices;
		m_modelMeshes[i].indexStart = iCurrIndexOffset;
		iCurrIndexOffset += m_modelMeshes[i].indexCount;

		// Set the material index of the current mesh.
		m_modelMeshes[i].materialIndex = pCurrMesh->mMaterialIndex;
	}

	// Construct the node tree of the model.
	recursiveNodeConstructor(m_pModel->mRootNode, m_modelRootNode);

	// @TODO: add implementation here

	// Create the vertex and index buffers.
	createBuffers();
}

// This version of the draw method will not be used.
void DXBasicModel::draw()
{}

// Draws the specified mesh.
void DXBasicModel::draw(const UINT uiMeshIndex)
{
	mD3dDeviceContext->DrawIndexed(m_modelMeshes[uiMeshIndex].indexCount,
									m_modelMeshes[uiMeshIndex].indexStart, 
									m_modelMeshes[uiMeshIndex].vertexStart);
}

// Recursive function which constructs the model's node tree.
void DXBasicModel::recursiveNodeConstructor(aiNode *pCurrNode, DXModelNode &currNode)
{
	//aiNode *pCurrNode = m_pModel->mRootNode;
	if (pCurrNode)
	{
		// Store the transformation of the current node.
		// NOTE: need the transpose of the matrix as aiNode stores it in col-major format.
		const aiMatrix4x4 &trans = pCurrNode->mTransformation.Transpose();
		currNode.transformation = DXMatrix(trans.a1, trans.a2, trans.a3, trans.a4,
			trans.b1, trans.b2, trans.b3, trans.b4,
			trans.c1, trans.c2, trans.c3, trans.c4,
			trans.d1, trans.d2, trans.d3, trans.d4);

		// Populate the mesh indices of the current node.
		currNode.meshIndexes.resize(pCurrNode->mNumMeshes);
		for (std::size_t i = 0; i < currNode.meshIndexes.size(); ++i)
			currNode.meshIndexes[i] = pCurrNode->mMeshes[i];

		// Populate the children of the current node.
		currNode.childNodes.resize(pCurrNode->mNumChildren);
		for (std::size_t i = 0; i < currNode.childNodes.size(); ++i)
			recursiveNodeConstructor(pCurrNode->mChildren[i], currNode.childNodes[i]);
	}
}

// Accessor function which returns the diffuse texture corresponding to the given mesh material index.
const ShaderResourceViewPtr DXBasicModel::getDiffuseTexture(const UINT uiMeshMaterialIndex)
{
	// Get the material corresponding to the given mesh material index.
	const aiMaterial *pMaterial = m_pModel->mMaterials[uiMeshMaterialIndex];
	
	// Find the key which will be used to index into the diffuse texture associative array.
	aiString key;
	if (!pMaterial || pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &key) != AI_SUCCESS)
		return ShaderResourceViewPtr();

	// Return the diffuse texture
	return m_modelDiffuseTextures[key.data];
}

// Accessor function which returns the specular texture corresponding to the given mesh material index.
const ShaderResourceViewPtr DXBasicModel::getSpecularTexture(const UINT uiMeshMaterialIndex)
{
	// Get the material corresponding to the given mesh material index.
	const aiMaterial *pMaterial = m_pModel->mMaterials[uiMeshMaterialIndex];

	// Find the key which will be used to index into the specular texture associative array.
	aiString key;
	if (!pMaterial || pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &key) != AI_SUCCESS)
		return ShaderResourceViewPtr();

	// Return the specular texture
	return m_modelSpecularTextures[key.data];
}

// Accessor function which returns the normal texture corresponding to the given mesh material index.
const ShaderResourceViewPtr DXBasicModel::getNormalTexture(const UINT uiMeshMaterialIndex)
{
	// Get the material corresponding to the given mesh material index.
	const aiMaterial *pMaterial = m_pModel->mMaterials[uiMeshMaterialIndex];

	// Find the key which will be used to index into the normal texture associative array.
	aiString key;
	if (!pMaterial || pMaterial->GetTexture(aiTextureType_HEIGHT, 0, &key) != AI_SUCCESS)
		return ShaderResourceViewPtr();

	// Return the normal texture
	return m_modelNormalTextures[key.data];
}

// Method to return the composite transformation matrix
DXMatrix BasicModelInstance::getTransform()
{
	return DXMatrix::CreateTranslation(position) * DXMatrix::CreateScale(scale) *
		DXMatrix::CreateFromAxisAngle(DXVector3(orientation.x, orientation.y, orientation.z), orientation.w);
}