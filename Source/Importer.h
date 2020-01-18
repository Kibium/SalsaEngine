#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include <string>

class Importer
{
	virtual bool Import(const char* path, const char* file, std::string& output_file)
	{
		return false;
	}
	virtual bool Import(const char* file, const void* buffer, unsigned int size, std::string& output_file)
	{
		return false;
	}
};

#endif 
