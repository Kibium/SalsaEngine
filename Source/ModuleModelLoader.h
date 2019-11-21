#include "Module.h"
#include "Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Importer.hpp"
#include "scene.h"
#include "postprocess.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

#define BAKERHOUSEFBX "BakerHouse.fbx"
#define BAKERHOUSEPNG "Baker_house.png"



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
	bool model;
private:
	void Load(const char*);
	void processNode(aiNode*, const aiScene*);
	Mesh processMesh(aiMesh*, const aiScene*);
	vector<Texture> loadMaterialTextures(aiMaterial*, aiTextureType, string);

	string GetModelDirectory(const char * path);


};


