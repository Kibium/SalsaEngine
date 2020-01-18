#include "ModuleFileSystem.h"
#include "PhysFS/include/physfs.h"
#include <Application.h>
#include "imgui.h"
#include <SDL.h>

#include <string>
#include <filesystem>

#pragma comment( lib, "Libraries/PhysFS/lib/physfs.lib" )


using namespace std;
using namespace std::tr2::sys;



ModuleFileSystem::ModuleFileSystem()
{
}


ModuleFileSystem::~ModuleFileSystem()
{
}
bool ModuleFileSystem::AddPath(const char* path_or_zip)
{
	bool ret = false;

	if (PHYSFS_mount(path_or_zip, nullptr, 1) == 0)
		LOG("File System error while adding a path or zip: %s\n", PHYSFS_getLastError());
	else
		ret = true;

	return ret;
}
bool ModuleFileSystem::Init()
{


	// needs to be created before Init so other modules can use it
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);
	SDL_free(base_path);

	if (!is_directory("../Assets"))
		create_directory("../Assets");

	if (!is_directory("../Library"))
		create_directory("../Library");

	if (!is_directory("../Library/Meshes"))
		create_directory("../Library/Meshes");

	// workaround VS string directory mess
	AddPath("../Library/Meshes");

	// Dump list of paths
	LOG("FileSystem Operations base is [%s] plus:", PHYSFS_getBaseDir());
	//LOG(GetReadPaths());

	for (auto i = PHYSFS_getSearchPath(); *i != NULL; ++i) {
		LOG("Work paths: [%s]: \n", *i);
	}

	

	// enable us to write in the game's dir area
	if (PHYSFS_setWriteDir("../Library/Meshes") == 0)
		LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError());

	LOG("GETWRITEDIR: [%s]", PHYSFS_getWriteDir());
	LOG("\n");

	return true;
}

bool ModuleFileSystem::CleanUp()
{
	return true;
}

char* ModuleFileSystem::Load(const char * path, const char * file) const
{
	string filename = path; filename += file;

	SDL_RWops *rw = SDL_RWFromFile(filename.c_str(), "rb");
	if (rw == NULL) return NULL;

	Sint64 res_size = SDL_RWsize(rw);
	char* res = new char[res_size + 1];

	Sint64 nb_read_total = 0;
	Sint64 nb_read = 1;
	char* buf = res;
	while (nb_read_total < res_size && nb_read != 0) {
		nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
		nb_read_total += nb_read;
		buf += nb_read;
	}
	SDL_RWclose(rw);
	if (nb_read_total != res_size) {
		delete[] res;
		return NULL;
	}

	res[nb_read_total] = '\0';

	return res;
}

bool ModuleFileSystem::Save(const char * path, const char * file, const void * buffer, unsigned int size, bool append) const
{
	unsigned int ret = 0;

	bool overwrite = PHYSFS_exists(file) != 0;

	string s = std::string(file) + ".mesh";

	LOG(s.c_str());
	LOG("\n");

	PHYSFS_file* fs_file = PHYSFS_openWrite(s.c_str());

	if (fs_file != nullptr)
	{
		unsigned int written = (unsigned int)PHYSFS_write(fs_file, (const void*)buffer, 1, size);
		if (written != size)
			LOG("File System error while writing to file %s: %s\n", file, PHYSFS_getLastError());
		else
		{
			if (append == true)
				LOG("Added %u data to [%s%s]\n", size, PHYSFS_getWriteDir(), file);
			//else if(overwrite == true)
				//LOG("File [%s%s] overwritten with %u bytes", PHYSFS_getWriteDir(), file, size);
			else if (overwrite == false)
				LOG("New file created [%s%s] of %u bytes\n", PHYSFS_getWriteDir(), file, size);

			ret = written;
		}

		if (PHYSFS_close(fs_file) == 0)
			LOG("File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
		LOG("File System error while opening file %s: %s\n", file, PHYSFS_getLastError());

	return ret;
}

bool ModuleFileSystem::Remove(const char * file)
{
	return remove(file) == 0;
}

bool ModuleFileSystem::Exists(const char * file) const
{
	SDL_RWops *rw = SDL_RWFromFile(file, "rb");
	return rw != NULL;
}

bool ModuleFileSystem::MakeDirectory(const char * directory)
{
	return PHYSFS_mkdir(directory);
}

bool ModuleFileSystem::IsDirectory(const char * directory) const
{
	return is_directory(directory);
}

bool ModuleFileSystem::Copy(const char * source, const char * destination)
{
	char* buffer = Load("", source);
	if (buffer == NULL)
		return NULL;

	size_t len = SDL_strlen(buffer);
	bool succes = Save("", destination, buffer, len);
	delete[] buffer;
	return succes;
}