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
	Model(const char *filePath);
	~Model();

public:
	const char *filePath = nullptr;
	std::string GetFileName(const char *);
private:
	void Load(const char*);
	void ProcessNode(aiNode*, const aiScene*);
	Mesh* ProcessMesh(aiMesh*, const aiScene*, const std::string&);
	std::string GetModelDirectory(const char*);
	

private:
	std::string fileName;
	std::string directory;
	int totalMeshes = 0;
	Material mat;
	math::AABB boundingBox;
	math::AABB modelBox;

};

#endif // __MODEL_H__
