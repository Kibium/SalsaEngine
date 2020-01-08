#include "Model.h"
#include "Application.h"
#include "ModuleShader.h"
#include "ModuleTexture.h"
#include "ModuleCamera.h"
#include "ComponentCamera.h"
#include "ModuleScene.h"
#include "assimp/DefaultLogger.hpp"
#include <assimp/cimport.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <algorithm>
using namespace Assimp;

Model::Model() {
}

Model::Model(const char *filePath) : filePath(filePath) {
	SwitchModel();
	ProcessName();
}

Model::~Model() {
}

void Model::ProcessName() {
	// Process name
	std::string fileName = filePath;

	for (std::string::iterator it = fileName.end() - 1; it != fileName.begin(); --it) {
		if ((*it) != '\\')
			name += (*it);
		else
			break;
	}
	std::reverse(name.begin(), name.end());
	name.pop_back();
	name.pop_back();
	name.pop_back();
	name.pop_back();
}

void Model::Draw() {
	if (isActive) {
		for (int i = 0; i < meshes.size(); ++i)
			meshes[i].Draw();
	}
}

void Model::SwitchModel() {
	if (model) {
		textures_loaded.clear();
		meshes.clear();
		directory.clear();
		nmeshes = 0;
		npolys = 0;
		nvertex = 0;
		model = false;
	}
	LOG("\nNew model added. \n Loading Model %s \n", filePath);
	Load(filePath);
}

void Model::SwitchTexture(const char *file) {
	vector<Texture> textures;
	if (model) {
		textures_loaded.clear();
	}
	LOG("\nNew Texture added. \n Loading Texture %s \n", file);
	Texture texture;
	texture.id = App->texture->Load((char*)file);
	texture.type = "texture_diffuse";
	texture.path = file;
	textures.push_back(texture);
	textures_loaded.push_back(texture);

	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].textures = textures;

}

void Model::Load(const char* path) {
	DefaultLogger::create("", Logger::VERBOSE);
	const unsigned int severity = Logger::Debugging | Logger::Info | Logger::Err | Logger::Warn;
	DefaultLogger::get()->attachStream(new myStream(), severity);
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcessPreset_TargetRealtime_MaxQuality);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		LOG("ERROR::ASSIMP:: %s \n", importer.GetErrorString());
		return;
	}
	modelBox.SetNegativeInfinity();
	directory = GetModelDirectory(path);
	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);

	model = true;
	DefaultLogger::kill();

}

void Model::processNode(aiNode *node, const aiScene *scene) {
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
		nmeshes += 1;
	}

	node->mTransformation.Decompose(modelScale, modelRotation, modelPosition);

	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
	App->scene->camera->Focus();
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;
		vertex.Position = float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

		vertex.TexCoords = float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

		//LOG("Texture %d coord x %f coord y %f \n", i, mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		vertices.push_back(vertex);
	}
	modelBox.Enclose((float3*)mesh->mVertices, mesh->mNumVertices);

	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	npolys += mesh->mNumFaces;
	nvertex += indices.size();

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	// return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName) {
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip) {   // if texture hasn't been loaded already, load it
			Texture texture;
			std::string newPath = directory.c_str() + string(str.C_Str());
			const char* pathing = newPath.c_str();
			texture.id = App->texture->Load((char*)pathing);
			if (!App->texture->loaded) {
				LOG("Loading Texture from model's directory\n");
				std::string filename = directory.c_str() + GetFilename(str.C_Str());
				const char* file = filename.c_str();
				texture.id = App->texture->Load((char*)file);
				texture.path = file;
				if (!App->texture->loaded) {
					LOG("Loading Texture from Textures directory\n");
					string fromFolder = "Models/Textures/" + GetFilename(str.C_Str());
					const char* folder = fromFolder.c_str();
					texture.id = App->texture->Load((char*)folder);
					texture.path = folder;
				}
			}
			else {
				texture.path = str.C_Str();
			}
			if (App->texture->loaded) {
				LOG("DEVIL:: Texture Loaded Succesfully\n");
			}
			else {
				LOG("DEVIL::ERROR  Loading texture. File not found \n");
			}
			texture.type = typeName;
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesary load duplicate textures.
		}
	}//
	return textures;
}
string Model::GetModelDirectory(const char *path) {
	std::string dir = std::string(path);

	std::size_t currentDir = dir.find_last_of("/\\");
	std::string modelDir = dir.substr(0, currentDir + 1);

	return modelDir;
}
string Model::GetFilename(const char *path) {
	std::string dir = std::string(path);

	std::size_t currentDir = dir.find_last_of("/\\");
	std::string filename = dir.substr(currentDir + 1);

	return filename;
}
void Model::RenderAABB() {

}