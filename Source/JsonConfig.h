#ifndef __JSONCONFIG_H__
#define __JSONCONFIG_H__

#include "rapidjson/document.h"
#include "MathGeoLib.h"

class JsonConfig {
public:
	JsonConfig();
	~JsonConfig();

public:
	void AddBool(const char *name, bool value);
	void AddInt(const char *name, const int value);
	void AddUInt(const char *name, unsigned int value);
	void AddFloat(const char *name, const float value);
	void AddFloat3(const char *name, const float3& value);
	void AddFloat4(const char *name, const float4& value);
	void AddString(const char *name, const std::string& value);

public:
	bool GetBool(const char *name, bool value);
	int GetInt(const char *name, int value);
	unsigned int GetUInt(const char *name, unsigned int value);
	float GetFloat(const char *name, float value);
	float3& GetFloat3(const char *name, float3& value);
	float4& GetFloat4(const char *name, float4& value);
	std::string& GetString(const char *name, std::string& value);

private:
	// document is the root of a json message
	rapidjson::Document document;

public:
	void SaveJson(const char *fileName);
	void LoadJson(const char *fileName);
};

#endif // __JSONCONFIG_H__
