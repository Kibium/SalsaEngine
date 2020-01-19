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
#include "MeshImporter.h"


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
	Model();
	Model::Model(const char * filePath, std::vector<string>& files, bool addToGameObjects = true);
	//Model::Model(const char *filePath, bool addToGameObjects = true);
	~Model();

	

public:
	int index = 0;
	const char *filePath = nullptr;
	std::string GetFileName(const char *);
	std::vector<Mesh*> meshes;
	Mesh* ProcessMesh(MeshData * data);

private:
	//void Load(const char*);
	void Load(const char * path, std::vector<string>& files);
	void ProcessNode(aiNode*, const aiScene*, std::vector<string> &files);
	Mesh* ProcessMesh(aiMesh*, const aiScene*, const std::string&, std::vector<string> &files);
	std::string GetModelDirectory(const char*);
	

private:
	std::string fileName;
	std::string directory;
	int totalMeshes = 0;
	Material mat;
	math::AABB boundingBox;
	math::AABB modelBox;
	MeshImporter i;
	bool addToGameObjects = true;
};

#endif // __MODEL_H__
