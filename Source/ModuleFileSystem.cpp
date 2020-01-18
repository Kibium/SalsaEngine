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

bool ModuleFileSystem::Init()
{
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);

	if (!exists("../Assets"))
		create_directory("../Assets");
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

bool ModuleFileSystem::Save(const char * path, const char * file, const void * buffer, unsigned int size) const
{
	string filename = path; filename += file;

	SDL_RWops *rw;
	
	rw = SDL_RWFromFile(filename.c_str(), "w");

	if (rw == NULL) return NULL;

	Sint64 nb_write_total = SDL_RWwrite(rw, buffer, 1, size);
	SDL_RWclose(rw);

	if (nb_write_total != size)
		return NULL;


	return 1;
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
	return create_directory(directory);
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