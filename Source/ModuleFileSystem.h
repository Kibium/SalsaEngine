#pragma once
#include "Module.h"
#include "Globals.h"


class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem();
	~ModuleFileSystem();
	bool Init();
	bool CleanUp();

	char* Load(const char* path, const char* file) const;
	bool Save(const char* path, const char* file, const void* buffer, unsigned int size, bool append = false) const;
	bool Remove(const char* file);
	bool Exists(const char* file) const;
	bool MakeDirectory(const char* directory);
	bool IsDirectory(const char* directory) const;
	bool Copy(const char* source, const char* destination);
	bool AddPath(const char* path_or_zip);

};

