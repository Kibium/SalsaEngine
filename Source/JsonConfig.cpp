#include "JsonConfig.h"
#include "Globals.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>
#include "Component.h"

JsonConfig::JsonConfig() {
	document.SetObject();
	allocator = &document.GetAllocator();
	gameObjects = rapidjson::Value(rapidjson::kArrayType);
}

JsonConfig::~JsonConfig() {
}

void JsonConfig::SaveJson(const char *fileName) {

	document.AddMember("Game Objects", gameObjects, *allocator);

	// create and log json result
	rapidjson::StringBuffer strbuf;
	strbuf.Clear();
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	document.Accept(writer);
	//LOG(strbuf.GetString());
	LOG("Saved scene data!\n");

	// save json to file
	FILE* file = nullptr;
	fopen_s(&file, fileName, "wt");

	if (!file) 
		LOG("Error saving scene. Can not create %s file.", fileName);

	char *jsonFile = _strdup(strbuf.GetString());

	fwrite(jsonFile, sizeof(char), strlen(jsonFile), file);
	fclose(file);
}

void JsonConfig::SaveGameObject(const GameObject& obj) {

	// object properties
	rapidjson::Value object(rapidjson::kObjectType);
	object.AddMember("UID", obj.UUID, *allocator);
	object.AddMember("ParentUID", obj.parent->UUID, *allocator);
	object.AddMember("Name", rapidjson::Value(obj.name.c_str(), *allocator), *allocator);
	object.AddMember("Active", obj.isActive, *allocator);
	object.AddMember("Static", obj.isStatic, *allocator);
	if (obj.model != nullptr) object.AddMember("Model", *obj.model->filePath, *allocator);
	else object.AddMember("Model", "None", *allocator);

	// object's components properties
	rapidjson::Value components(rapidjson::kArrayType);
	rapidjson::Value transform(rapidjson::kObjectType);
	rapidjson::Value mesh(rapidjson::kObjectType);
	rapidjson::Value material(rapidjson::kObjectType);

	if (obj.components.size() > 0) {
		transform.AddMember("UID", obj.components[0]->UUID, *allocator);
		transform.AddMember("ParentUID", obj.components[0]->myGo->UUID, *allocator);
		transform.AddMember("Type", (int)obj.components[0]->type, *allocator);
		transform.AddMember("Active", obj.components[0]->active, *allocator);
		rapidjson::Value trans(rapidjson::kArrayType);
		trans.PushBack(obj.components[0]->position.x, *allocator).PushBack(obj.components[0]->position.y, *allocator).PushBack(obj.components[0]->position.z, *allocator);
		transform.AddMember("Translation", trans, *allocator);
		rapidjson::Value rot(rapidjson::kArrayType);
		rot.PushBack(obj.components[0]->rotation.x, *allocator).PushBack(obj.components[0]->rotation.y, *allocator).PushBack(obj.components[0]->rotation.z, *allocator);
		transform.AddMember("Rotation", rot, *allocator);
		rapidjson::Value scale(rapidjson::kArrayType);
		scale.PushBack(obj.components[0]->scale.x, *allocator).PushBack(obj.components[0]->scale.y, *allocator).PushBack(obj.components[0]->scale.z, *allocator);
		transform.AddMember("Scale", scale, *allocator);
		components.PushBack(transform, *allocator);

		if (obj.components.size() > 1) {
			mesh.AddMember("UID", obj.components[1]->UUID, *allocator);
			mesh.AddMember("ParentUID", obj.components[1]->myGo->UUID, *allocator);
			mesh.AddMember("Type", (int)obj.components[1]->type, *allocator);
			mesh.AddMember("Active", obj.components[1]->active, *allocator);
			components.PushBack(mesh, *allocator);

			if (obj.components.size() > 2) {
				material.AddMember("UID", obj.components[2]->UUID, *allocator);
				material.AddMember("ParentUID", obj.components[2]->myGo->UUID, *allocator);
				material.AddMember("Type", (int)obj.components[2]->type, *allocator);
				material.AddMember("Active", obj.components[2]->active, *allocator);
				components.PushBack(material, *allocator);
			}
		}
	}

	object.AddMember("Components", components, *allocator);
	gameObjects.PushBack(object, *allocator);
}

void JsonConfig::LoadJson(const char *fileName) {

	// load json from file
	FILE* file = nullptr;
	fopen_s(&file, fileName, "rt");

	if (!file)
		LOG("Error loasing scene. Can not read %s file.", fileName);

	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	rewind(file);

	char *json = new char[size + 1];
	fread(json, 1, size, file);
	json[size] = 0;
	fclose(file);

	// parse
	document.Parse(json);
	assert(document.IsObject());

	const rapidjson::Value& a = document["Game Objects"];
	assert(a.IsArray());
	rapidjson::SizeType i;
	static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };

	// loop game objects
	for (i = 0; i < a.Size(); i++) {
		LOG("Game Object %d:\n", i);

		for (rapidjson::Value::ConstMemberIterator itr = a[i].MemberBegin(); itr != a[i].MemberEnd(); ++itr) {
			LOG("%s : %s\n", itr->name.GetString(), kTypeNames[itr->value.GetType()]);
		}

		LOG("\n");
	}
		
	LOG("Game Objects Count: %d\n", i);

	delete json;
	LOG("Loaded scene data!\n");
}
