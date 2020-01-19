#ifndef __MESHIMPORTER_H__
#define __MESHIMPORTER_H__

#include "Importer.h"
#include <vector>
#include "MathGeoLib.h"

struct MeshData
{
	std::string name;

	unsigned int nIndices = 0;
	unsigned int nVertices = 0;
	

	//what defines the order of the triangles' vertices
	std::vector <unsigned int> indices;

	//Its positions
	std::vector <float3> positions;

	std::vector <float2> texture_coords;
};

class MeshImporter : public Importer
{
public:
	bool Import(const char* file, const MeshData & mesh, std::string& output_file);
	bool Import(const char* file, const void* buffer, unsigned int size, std::string& output_file);
	bool Load(const char* exported_file, MeshData & mesh);
};

#endif