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
	std::string fileName, name;
	bool isActive = true;
	const char *filePath = nullptr;
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

private:
	void Load(const char*);
	void processNode(aiNode*, const aiScene*);
	Mesh* processMesh(aiMesh*, const aiScene*, const std::string&name);
	string GetModelDirectory(const char*);
	string GetFilename(const char*);

};

#endif // __MODEL_H__
