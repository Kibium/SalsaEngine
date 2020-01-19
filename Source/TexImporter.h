#ifndef __TEXIMPORTER_H__
#define __TEXIMPORTER_H__

#include "Importer.h"
#include <vector>
#include "MathGeoLib.h"


class TexImporter : public Importer
{
public:
	bool Import(const char* file, const void * buffer, unsigned int size, std::string &output_file);
	bool Import(const char* path, const char* file, std::string &output);
	bool Load(const char* exported_file);
};

#endif
