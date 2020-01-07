#include "ModuleModelLoader.h"

ModuleModelLoader::ModuleModelLoader() {
}

ModuleModelLoader::~ModuleModelLoader() {
	if (models.size() > 0) {
		for (int i = 0; i < models.size(); ++i)
			delete models[i];
	}
}

void ModuleModelLoader::AddModel(const char *filePath) {
	Model *model = new Model(filePath);
	models.push_back(model);
}

Model* ModuleModelLoader::GetModel(const char *filePath) {
	if (filePath == nullptr || models.size() < 1) return nullptr;

	for (int i = 0; i < models.size(); ++i) {
		if (filePath == models[i]->filePath) {
			return models[i];
		}
	}

	return nullptr;
}

void ModuleModelLoader::Draw() {
	if (models.size() > 0) {
		for (int i = 0; i < models.size(); ++i)
			models[i]->Draw();
	}
}

bool ModuleModelLoader::Init() {
	LOG("Init Model Loader\n");
	return true;
}

bool ModuleModelLoader::CleanUp() {
	return true;
}
