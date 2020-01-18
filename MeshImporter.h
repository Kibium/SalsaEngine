#ifndef __MESHIMPORTER_H__
#define __MESHIMPORTER_H__

#include "Importer.h"

struct MeshData
{
	std::string name;

	unsigned int nIndices;
	unsigned int nVertices;

	//what defines the order of the triangles' vertices
	unsigned int* indices = nullptr;

	//Its positions
	float* positions = nullptr;

	//Self explanatory xd
	float* normals = nullptr;

	float* texture_coords = nullptr;	
};

class MeshImporter : public Importer
{
public:
	bool Import(const char* file, const MeshData & mesh, std::string& output_file);
	bool Import(const char* file, const void* buffer, unsigned int size, std::string& output_file);
	bool Load(const char* exported_file, MeshData & mesh);
};

#endif