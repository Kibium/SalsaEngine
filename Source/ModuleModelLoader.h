#ifndef __MODULEMODELLOADER_H__
#define __MODULEMODELLOADER_H__

#include "Module.h"
#include "Model.h"

class ModuleModelLoader : public Module {
public:
	std::vector<Model*> models;

public:
	ModuleModelLoader();
	~ModuleModelLoader();

	void AddModel(const char* filePath);
	Model* GetModel(const char* filePath);

	void Draw();
	bool Init();
	bool CleanUp();

	void SwitchModel(const char*);
	void SwitchTexture(const char*);
	void RenderAABB();
	aiVector3D modelPosition;
	aiVector3D modelScale;
	aiVector3D modelRotation;
	bool model = false;
	AABB modelBox;
	int npolys = 0;
	int nvertex = 0;
	int nmeshes= 0;

private:
	void Load(const char*);
	void processNode(aiNode*, const aiScene*);
	Mesh processMesh(aiMesh*, const aiScene*);
	vector<Texture> loadMaterialTextures(aiMaterial*, aiTextureType, string);
	string GetModelDirectory(const char*);
	string GetFilename(const char*);

};
#endif // __MODULEMODELLOADER_H__


