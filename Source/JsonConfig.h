#ifndef __JSONCONFIG_H__
#define __JSONCONFIG_H__

#include "rapidjson/document.h"
#include "MathGeoLib.h"
#include "GameObject.h"

class JsonConfig {
public:
	JsonConfig();
	~JsonConfig();

public:
	void SaveJson(const char *fileName);
	void SaveGameObject(const GameObject& obj);

	void LoadJson(const char *fileName);

private:
	rapidjson::Document document; 
	rapidjson::Document::AllocatorType *allocator = nullptr; 
	rapidjson::Value gameObjects;
};

#endif // __JSONCONFIG_H__
