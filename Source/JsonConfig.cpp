#include "JsonConfig.h"
#include "Globals.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>
#include "Component.h"

JsonConfig::JsonConfig() {
	// define the document as an object rather than an array
	document.SetObject();
	allocator = &document.GetAllocator();
}

JsonConfig::~JsonConfig() {
}

void JsonConfig::AddInt(const char *name, const int value) {
}

int JsonConfig::GetInt(const char *name, int value) {
	return 0;
}

void JsonConfig::SaveJson(const char *fileName) {

	// create and log json result
	rapidjson::StringBuffer strbuf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	document.Accept(writer);
	LOG(strbuf.GetString());

	// save json to file
	FILE* file = nullptr;
	fopen_s(&file, fileName, "wt");

	if (!file) 
		LOG("Error saving scene. Can not create %s file.", fileName);

	fwrite(strbuf.GetString(), sizeof(char), strlen(strbuf.GetString()), file);
	fclose(file);

}

void JsonConfig::SaveJsonTest(const char *fileName) {
	
	// create a rapidjson array type with similar syntax to std::vector
	rapidjson::Value array(rapidjson::kArrayType);

	// must pass an allocator when the object may need to allocate memory
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	// chain methods as rapidjson provides a fluent interface when modifying its objects
	array.PushBack("hello", allocator).PushBack("world", allocator);//"array":["hello","world"]

	document.AddMember("Name", "XYZ", allocator);
	document.AddMember("Rollnumer", 2, allocator);
	document.AddMember("array", array, allocator);

	// create a rapidjson object type
	rapidjson::Value object(rapidjson::kObjectType);
	object.AddMember("Math", "50", allocator);
	object.AddMember("Science", "70", allocator);
	object.AddMember("English", "50", allocator);
	object.AddMember("Social Science", "70", allocator);
	document.AddMember("Marks", object, allocator);
	//	fromScratch["object"]["hello"] = "Yourname";

	// create and log json result
	rapidjson::StringBuffer strbuf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	document.Accept(writer);
	LOG(strbuf.GetString());

	// save json to file
	FILE* file = nullptr;
	fopen_s(&file, fileName, "wt");
	if (!file) {
		LOG("Error saving scene. Can not create %s file.", fileName);
		return;
	}
	fwrite(strbuf.GetString(), sizeof(char), strlen(strbuf.GetString()), file);
	fclose(file);
}

void JsonConfig::SaveGameObject(const GameObject& obj) {

	// object properties
	rapidjson::Value object(rapidjson::kObjectType);
	object.AddMember("UID", obj.UUID, *allocator);
	object.AddMember("ParentUID", obj.parent->UUID, *allocator);
	object.AddMember("Name", rapidjson::Value(obj.name.c_str(), *allocator), *allocator);

	rapidjson::Value trans(rapidjson::kArrayType);
	trans.PushBack((int)obj.components[0]->position.x, *allocator).PushBack((int)obj.components[0]->position.y, *allocator).PushBack((int)obj.components[0]->position.z, *allocator);
	object.AddMember("Translation", trans, *allocator);

	rapidjson::Value scale(rapidjson::kArrayType);
	scale.PushBack((int)obj.components[0]->scale.x, *allocator).PushBack((int)obj.components[0]->scale.y, *allocator).PushBack((int)obj.components[0]->scale.z, *allocator);
	object.AddMember("Scale", scale, *allocator);

	rapidjson::Value rot(rapidjson::kArrayType);
	rot.PushBack((int)obj.components[0]->rotation.x, *allocator).PushBack((int)obj.components[0]->rotation.y, *allocator).PushBack((int)obj.components[0]->rotation.z, *allocator);
	object.AddMember("Rotation", rot, *allocator);

	// object's components properties
	rapidjson::Value components(rapidjson::kObjectType);
	if (obj.components.size() > 0) {
		components.AddMember("UID", obj.components[0]->UUID, *allocator);
		components.AddMember("ParentUID", obj.components[0]->myGo->UUID, *allocator);
		components.AddMember("Type", (int)obj.components[0]->type, *allocator);
		components.AddMember("Active", obj.components[0]->active, *allocator);

		if (obj.components.size() > 1) {
			components.AddMember("UID", obj.components[1]->UUID, *allocator);
			components.AddMember("ParentUID", obj.components[1]->myGo->UUID, *allocator);
			components.AddMember("Type", (int)obj.components[1]->type, *allocator);
			components.AddMember("Active", obj.components[1]->active, *allocator);

			if (obj.components.size() > 2) {
				components.AddMember("UID", obj.components[2]->UUID, *allocator);
				components.AddMember("ParentUID", obj.components[2]->myGo->UUID, *allocator);
				components.AddMember("Type", (int)obj.components[2]->type, *allocator);
				components.AddMember("Active", obj.components[2]->active, *allocator);
			}
		}
	}

	object.AddMember("Components", components, *allocator);

	document.AddMember("Game Objects", object, *allocator);

}

void JsonConfig::LoadJson(const char *fileName) {
}
