#include "JsonConfig.h"
#include "Globals.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>

JsonConfig::JsonConfig() {
	// define the document as an object rather than an array
	document.SetObject();
}

JsonConfig::~JsonConfig() {
}

void JsonConfig::AddInt(const char *name, const int value) {
}

int JsonConfig::GetInt(const char *name, int value) {
	return 0;
}

void JsonConfig::SaveJson(const char *fileName) {
	
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

void JsonConfig::LoadJson(const char *fileName) {
	//FILE* pFile = fopen(fileName, "rb");
	//char buffer[65536];
	//FileReadStream is(pFile, buffer, sizeof(buffer));
	//document.ParseStream<0, UTF8<>, FileReadStream>(is);
}
