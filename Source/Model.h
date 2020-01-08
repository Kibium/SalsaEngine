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
	bool isActive = true;
	const char *filePath = nullptr;
	string name;
	vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Mesh> meshes;
	string directory;
	vector<int> textureWidth;
	vector<int> textureHeight;
	Model();
	Model(const char *filePath);
	~Model();

	void ProcessName();
	void Draw();
	void SwitchModel();
	void SwitchTexture(const char*);
	void RenderAABB();
	aiVector3D modelPosition;
	aiVector3D modelScale;
	aiVector3D modelRotation;
	bool model = false;
	AABB modelBox;
	int npolys = 0;
	int nvertex = 0;
	int nmeshes = 0;

private:

	void Load(const char*);
	void processNode(aiNode*, const aiScene*);
	Mesh processMesh(aiMesh*, const aiScene*);
	vector<Texture> loadMaterialTextures(aiMaterial*, aiTextureType, string);
	string GetModelDirectory(const char*);
	string GetFilename(const char*);

};
#endif // __MODEL_H__
