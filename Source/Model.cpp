#include "Model.h"
#include "Application.h"
#include "ModuleShader.h"
#include "ModuleTexture.h"
#include "ModuleModelLoader.h"
#include "ModuleCamera.h"
#include "ComponentCamera.h"
#include "ModuleScene.h"
#include "assimp/DefaultLogger.hpp"
#include <assimp/cimport.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <algorithm>
#include "GameObject.h"
using namespace Assimp;

Model::Model() {
}

Model::Model(const char *filePath) : filePath(filePath), fileName(filePath) {
	LOG("FILEPATH: %s\n", filePath);
	ProcessName();
	Load(filePath);
}

Model::~Model() {
}

void Model::ProcessName() {
	// Process name
	fileName = filePath;

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
			meshes[i]->Draw();
	}

	glUniform3f(glGetUniformLocation(App->shader->def_program, "light.ambient"), 0.2f, 0.2f, 0.2f);
	glUniform3f(glGetUniformLocation(App->shader->def_program, "light.position"), App->model->light.pos.x, App->model->light.pos.y, App->model->light.pos.z);
	glUniform3f(glGetUniformLocation(App->shader->def_program, "light.diffuse"), 1, 1, 1);
	glUniform3f(glGetUniformLocation(App->shader->def_program, "light.specular"), 0.8f, 0.8f, 0.8f);

	glUniform4f(glGetUniformLocation(App->shader->def_program, "material.diff_color"), mat.diffuse_color.x, mat.diffuse_color.y, mat.diffuse_color.z, mat.diffuse_color.w);
	glUniform4f(glGetUniformLocation(App->shader->def_program, "material.spec_color"), mat.specular_color.x, mat.specular_color.y, mat.specular_color.z, mat.specular_color.w);
	glUniform4f(glGetUniformLocation(App->shader->def_program, "material.occ_color"), mat.occlusion_color.x, mat.occlusion_color.y, mat.occlusion_color.z, mat.occlusion_color.w);
	glUniform4f(glGetUniformLocation(App->shader->def_program, "material.emi_color"), mat.emissive_color.x, mat.emissive_color.y, mat.emissive_color.z, mat.emissive_color.w);

	glUniform1f(glGetUniformLocation(App->shader->def_program, "material.shininess"), mat.shininess);

	glUniform1f(glGetUniformLocation(App->shader->def_program, "material.k_spec"), mat.k_specular);
	glUniform1f(glGetUniformLocation(App->shader->def_program, "material.k_diff"), mat.k_diffuse);
	glUniform1f(glGetUniformLocation(App->shader->def_program, "material.k_occ"), mat.k_ambient);
}

void Model::Load(const char* path) {
	DefaultLogger::create("", Logger::VERBOSE);
	const unsigned int severity = /*Logger::Debugging | Logger::Info |*/ Logger::Err /*| Logger::Warn*/;
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
	boundingBox.SetNegativeInfinity();
	directory = GetModelDirectory(path);
	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);

	model = true;
	DefaultLogger::kill();
}


void Model::processNode(aiNode *node, const aiScene *scene) {
	// process each mesh located at the current node
	int counter = 0;
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		// Create a Game Object for each mesh
		Mesh* newMesh = processMesh(mesh, scene);
		node->mTransformation.Decompose(newMesh->modelScale, newMesh->modelRotation, newMesh->modelPosition);
		auto go = App->scene->CreateGameObject();
		go->modelPath = fileName;
		go->model = newMesh;
		go->name = App->model->GetFilename(filePath) + " " + std::to_string(nmeshes);
		go->DeleteComponent(Type::TRANSFORM);
		go->CreateComponent(Type::TRANSFORM);
		go->CreateComponent(Type::MESH);
		go->CreateComponent(Type::MATERIAL);
		//newMesh->myGo = go;
		nmeshes += 1;
	}

	node->mTransformation.Decompose(modelScale, modelRotation, modelPosition);

	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
	
	App->scene->camera->Focus();
}

Mesh* Model::processMesh(aiMesh *mesh, const aiScene *scene) {
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
	boundingBox.Enclose((float3*)mesh->mVertices, mesh->mNumVertices);

	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	int polygons = mesh->mNumFaces;
	int verticesNum = indices.size();

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	/* 1. diffuse maps
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
	// return a mesh object created from the extracted mesh data*/

	Mesh* meshM = new Mesh(vertices, indices, mat, polygons, verticesNum, boundingBox, modelBox);

	if (!load_once) {
		meshM->LoadTexture(textures, DIFFUSE, std::string(filePath), GetFilename(filePath));
		meshM->LoadTexture(textures, SPECULAR,  std::string(filePath), GetFilename(filePath));
		meshM->LoadTexture(textures, OCCLUSION, std::string(filePath), GetFilename(filePath));
		load_once = true;
	}

	return meshM;
}

//vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName) {
//	vector<Texture> textures;
//	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
//		aiString str;
//		mat->GetTexture(type, i, &str);
//		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
//		bool skip = false;
//		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
//			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
//				textures.push_back(textures_loaded[j]);
//				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
//				break;
//			}
//		}
//		if (!skip) {   // if texture hasn't been loaded already, load it
//			Texture texture;
//			std::string newPath = directory.c_str() + string(str.C_Str());
//			const char* pathing = newPath.c_str();
//			texture.id = App->texture->Load((char*)pathing);
//			if (!App->texture->loaded) {
//				LOG("Loading Texture from model's directory\n");
//				std::string filename = directory.c_str() + GetFilename(str.C_Str());
//				const char* file = filename.c_str();
//				texture.id = App->texture->Load((char*)file);
//				texture.path = file;
//				if (!App->texture->loaded) {
//					LOG("Loading Texture from Textures directory\n");
//					string fromFolder = "Models/Textures/" + GetFilename(str.C_Str());
//					const char* folder = fromFolder.c_str();
//					texture.id = App->texture->Load((char*)folder);
//					texture.path = folder;
//				}
//			}
//			else {
//				texture.path = str.C_Str();
//			}
//			if (App->texture->loaded) {
//				LOG("DEVIL:: Texture Loaded Succesfully\n");
//			}
//			else {
//				LOG("DEVIL::ERROR  Loading texture. File not found \n");
//			}
//			texture.type = typeName;
//			textures.push_back(texture);
//			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesary load duplicate textures.
//		}
//	}//
//	return textures;
//}

string Model::GetModelDirectory(const char *path) {
	std::string dir = std::string(path);

	std::size_t currentDir = dir.find_last_of("/\\");
	std::string modelDir = dir.substr(0, currentDir + 1);

	return modelDir;
}
string Model::GetFilename(const char *path) {
	std::string dir = std::string(path);

	std::size_t currentDir = dir.find_last_of("/\\");
	std::string filename;
	filename = dir.substr(currentDir + 1);
	filename = filename.substr(0, filename.size() - 4);
	return filename;
}
