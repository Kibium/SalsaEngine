#ifndef __MODEL_H__
#define __MODEL_H__

#include "Globals.h"
#include "Mesh.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Logger.hpp"
#include "assimp/LogStream.hpp"
#include "MathGeoLib.h"
#include "Geometry/AABB.h"
#include "ModuleTexture.h"
#include <string>
using namespace std;

class myStream : public Assimp::LogStream {
public:
	myStream() {}
	~myStream() {}
	void write(const char* message) {
		LOG("ASSIMP:: %s\n", message);
	}
};

class Model {

public:
	std::string fileName;
	bool isActive = true;
	const char *filePath = nullptr;
	string name;
	vector<Texture> textures_loaded;
	vector<Mesh*> meshes;
	string directory, model_name;
	vector<int> textureWidth;
	vector<int> textureHeight;
	aiVector3D modelPosition;
	aiVector3D modelScale;
	aiVector3D modelRotation;
	bool model = false;
	AABB modelBox;
	AABB boundingBox;
	int npolys = 0;
	int nvertex = 0;
	int nmeshes = 0;
	bool load_once = false;
	Material mat; 

public:
	Model();
	Model(const char *filePath);
	~Model();

	void ProcessName();
	void Draw();
	void SwitchModel();
	void SwitchTexture(const char*);
	void RenderAABB();

private:
	void Load(const char*);
	void LoadTexture(vector<Texture>& v, TextureType type);
	void processNode(aiNode*, const aiScene*);
	Mesh* processMesh(aiMesh*, const aiScene*);
	vector<Texture> loadMaterialTextures(aiMaterial*, aiTextureType, string);
	string GetModelDirectory(const char*);
	string GetFilename(const char*);
	bool item_exists(const char* path);

};

#endif // __MODEL_H__
