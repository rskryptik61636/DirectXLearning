// Entry point for the ModelsTest1 application.

#include <iostream>
#include <string>
#include <map>

#include <AssimpInclude.h>

#include <DXShaderUtils.h>

int main(int argc, char *argv[])
{
	// Load the model (argv[1]) using Assimp::Importer with the appropriate options for use in DX.
	Assimp::Importer importer;
	const unsigned int importOptions(aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | 
									aiProcess_FlipUVs | aiProcessPreset_TargetRealtime_Quality);
	const aiScene *scene = importer.ReadFile(argv[1], importOptions);
	if (!scene)
	{
		std::cerr << "Error importing " << argv[1] << " because " << importer.GetErrorString() << std::endl;
		return 1;
	}
	else
		std::cout << argv[1] << " loaded successfully!" << std::endl;

	// Load all the textures available in the model.
	std::map<std::string, ShaderResourceViewPtr> modelTextures;
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		aiString strTexPath;
		aiReturn status(AI_SUCCESS);

		for (int iTexIndex(0); scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, iTexIndex, &strTexPath) == AI_SUCCESS; ++iTexIndex)
		{
			const std::string strCurrTexPath(strTexPath.data);
			const wpath currTexPath(strCurrTexPath.begin(), strCurrTexPath.end());
			//HR(loadTexture())
		}
	}
	
	return 0;
}