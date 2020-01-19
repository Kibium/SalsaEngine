#include "TexImporter.h"

#include "Application.h"
#include "ModuleFilesystem.h"
#include "PhysFS/include/physfs.h"
#pragma comment( lib, "Libraries/PhysFS/lib/physfs.lib" )

bool TexImporter::Import(const char* path, const char* file, std::string &output) {

	return false;

}

bool TexImporter::Import(const char* file, const void * buffer, unsigned int size, std::string & output_file)
{

	std::string filename = file + output_file;
	filename = filename.substr(0, filename.size() - 4);
	filename += ".dds";
	output_file = file + output_file;

	LOG(filename.c_str());

	return App->fs->Save("../Library/Textures/", filename.c_str(), buffer, size, false);
}