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
#include "MeshImporter.h"
using namespace std;
using namespace Assimp;

Model::Model() {
}

Model::Model(const char *filePath, bool addToGameObjects) : filePath(filePath), addToGameObjects(addToGameObjects) {
	fileName = GetFileName(filePath);
	Load(filePath);
}

Model::~Model() {
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
	ProcessNode(scene->mRootNode, scene);

	DefaultLogger::kill();
}

void Model::ProcessNode(aiNode *node, const aiScene *scene) {
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];


		// Create a Game Object for each mesh

		Mesh* newMesh = ProcessMesh(mesh, scene, node->mName.C_Str());
		node->mTransformation.Decompose(newMesh->modelScale, newMesh->modelRotation, newMesh->modelPosition);

		auto go = App->scene->CreateGameObject();
		go->modelPath = filePath;
		go->model = newMesh;
		go->modelContainer = this;
		go->name = node->mName.C_Str();
		go->DeleteComponent(Type::TRANSFORM);
		go->CreateComponent(Type::TRANSFORM);
		go->CreateComponent(Type::MESH);
		go->CreateComponent(Type::MATERIAL);
		go->modelIndex = totalMeshes;
		meshes.push_back(newMesh);
		totalMeshes++;


		if (!addToGameObjects)
			App->scene->DeleteGameObject(go);
	}

	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene);
	}

	App->scene->camera->Focus();
}

Mesh* Model::ProcessMesh(aiMesh *mesh, const aiScene *scene, const std::string& name) {
	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	MeshData data;
	

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;
		vertex.Position = float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		
		vertex.TexCoords = float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		vertices.push_back(vertex);

		data.positions.push_back(vertex.Position);
		data.texture_coords.push_back(vertex.TexCoords);
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

	Mesh* meshM = new Mesh(vertices, indices, mat, polygons, verticesNum, boundingBox, modelBox);
	std::string meshName = name;

	meshM->LoadTexture(textures, DIFFUSE, std::string(meshName));
	meshM->LoadTexture(textures, SPECULAR, std::string(meshName));
	meshM->LoadTexture(textures, OCCLUSION, std::string(meshName));

	data.indices = indices;
	
	data.nIndices = indices.size();
	data.nVertices = vertices.size();

	MeshImporter importer;
	string s;
	importer.Import(meshName.c_str(), data, s);
	
	LOG("%d\n", vertices.size());

	return meshM;
}

string Model::GetModelDirectory(const char *path) {
	std::string dir = std::string(path);
	std::size_t currentDir = dir.find_last_of("/\\");
	std::string modelDir = dir.substr(0, currentDir + 1);
	return modelDir;
}

string Model::GetFileName(const char *path) {
	std::string dir = std::string(path);
	std::size_t currentDir = dir.find_last_of("/\\");
	std::string filename;
	filename = dir.substr(currentDir + 1);
	filename = filename.substr(0, filename.size() - 4);
	return filename;
}
