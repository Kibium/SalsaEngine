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

};
#endif // __MODULEMODELLOADER_H__


