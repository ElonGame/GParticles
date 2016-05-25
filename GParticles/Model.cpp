#include "Model.h"




Model::Model()
{
}

Model::~Model()
{
}

void Model::draw()
{
	for (auto m : meshes)
	{
		m.draw();
	}
}

void Model::loadModel(std::string path)
{
	// initialize importer, transform model primitives to triangles and flip texCoords y axis
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Utils::exitMessage("Assimp Error", importer.GetErrorString());
	}

	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);

	//std::vector<Vertex> concVertices = meshes[5].vertices;
	//std::vector<GLuint> concIndices = meshes[5].indices;
	//std::vector<Texture> concTextures = meshes[5].textures;

	//concVertices.insert(concVertices.end(), meshes[6].vertices.begin(), meshes[6].vertices.end());
	//concIndices.insert(concIndices.end(), meshes[6].indices.begin(), meshes[6].indices.end());
	//concTextures.insert(concTextures.end(), meshes[6].textures.begin(), meshes[6].textures.end());

	//int offset = meshes[5].indices.size();
	//for (int i = offset; i < concIndices.size(); i++)
	//{
	//	concIndices[i] = concIndices[i] + offset;
	//}

	//Mesh(concVertices, concIndices, concTextures);
	////meshes[5] = meshes[6];
	//meshes.pop_back();
	//meshes.pop_back();
	//meshes.push_back(Mesh(concVertices, concIndices, concTextures));
}

void Model::processNode(aiNode * node, const aiScene * scene)
{
	// process all the node's meshes (if any)
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	// process vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	// process indices
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	
	// process material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material,
									aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = this->loadMaterialTextures(material,
									aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;

	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str, textureName;
		mat->GetTexture(type, i, &textureName);
		str.Set(directory + "/" + textureName.C_Str());
		Texture texture(str.C_Str(), typeName);
		textures.push_back(texture);
	}

	return textures;
}
