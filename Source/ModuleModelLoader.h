#ifndef __MODULEMODELLOADER_H__
#define __MODULEMODELLOADER_H__

#include "Module.h"
#include "Mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Logger.hpp"
#include "assimp/LogStream.hpp"
#include "MathGeoLib.h"
#include "Geometry/AABB.h"

using namespace std;
class myStream : public Assimp::LogStream
{
public:
	myStream(){}
	~myStream(){}
	void write(const char* message)
	{
		LOG("ASSIMP:: %s\n", message);
	}
};
class ModuleModelLoader : public Module {
public:

	vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Mesh> meshes;
	string directory;
	vector<int> textureWidth;
	vector<int> textureHeight;
	ModuleModelLoader();
	~ModuleModelLoader();

	void Draw();
	bool Init();
	bool CleanUp();
	void SwitchModel(const char*);
	void SwitchTexture(const char*);
	void RenderAABB();

	bool model;
	AABB modelBox;
	int npolys;
	int nvertex;
	int nmeshes;

private:
	void Load(const char*);
	void processNode(aiNode*, const aiScene*);
	Mesh processMesh(aiMesh*, const aiScene*);
	vector<Texture> loadMaterialTextures(aiMaterial*, aiTextureType, string);
	string GetModelDirectory(const char*);
	string GetFilename(const char*);




};
#endif // __MODULEMODELLOADER_H__


