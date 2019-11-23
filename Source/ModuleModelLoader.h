#include "Module.h"
#include "Mesh.h"

#include "Importer.hpp"
#include "scene.h"
#include "postprocess.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "MathGeoLib.h"
#include "Geometry/AABB.h"

using namespace std;

class ModuleModelLoader : public Module {
public:

	vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Mesh> meshes;
	string directory;

	ModuleModelLoader();
	~ModuleModelLoader();

	void Draw();
	bool Init();
	bool CleanUp();
	void SwitchModel(const char*);
	void SwitchTexture(const char * file);
	bool model;
	AABB modelBox;

private:
	void Load(const char*);
	void processNode(aiNode*, const aiScene*);
	Mesh processMesh(aiMesh*, const aiScene*);
	vector<Texture> loadMaterialTextures(aiMaterial*, aiTextureType, string);

	string GetModelDirectory(const char * path);


};


