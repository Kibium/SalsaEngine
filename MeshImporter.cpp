#include "MeshImporter.h"

#include "Application.h"
#include "ModuleFilesystem.h"

using namespace std;

bool MeshImporter::Import(const char * file, const MeshData & mesh, string & output_file)
{
	unsigned int ranges[2] = { mesh.nIndices, mesh.nVertices };

	unsigned int size = sizeof(ranges)					//ranges
		+ sizeof(unsigned int) * mesh.nIndices		//indices
		+ sizeof(float) * mesh.nVertices * 3			//vertex positions
		+ sizeof(float) * mesh.nVertices * 3			//vertex normals
		+ sizeof(float) * mesh.nVertices * 2;		//vertex texture coord
	//	+ sizeof(float) * 6;							//AABB
	//TODO: add AABB and color to save and to load

	char* data = new char[size]; // Allocate
	char* cursor = data;

	unsigned int bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);

	cursor += bytes; // Store indices
	bytes = sizeof(unsigned int) * mesh.nIndices;
	memcpy(cursor, mesh.indices, bytes);

	cursor += bytes; // Store vertex positions
	bytes = sizeof(float) * mesh.nVertices * 3;
	memcpy(cursor, mesh.positions, bytes);

	cursor += bytes; // Store vertex normals
	bytes = sizeof(float) * mesh.nVertices * 3;
	memcpy(cursor, mesh.normals, bytes);

	cursor += bytes; // Store vertex texture coords
	bytes = sizeof(float) * mesh.nVertices * 2;
	memcpy(cursor, mesh.texture_coords, bytes);

	return Import(file, data, size, output_file);
}

bool MeshImporter::Import(const char * file, const void * buffer, unsigned int size, std::string & output_file)
{
	if (!App->fs->IsDirectory("../Library"))
		App->fs->MakeDirectory("../Library");
	if (!App->fs->IsDirectory("../Library/Meshes"))
		App->fs->MakeDirectory("../Library/Meshes");

	string filename = file; filename += ".mesh";
	output_file = file;

	return App->fs->Save("../Library/Meshes/", filename.c_str(), buffer, size);
}

bool MeshImporter::Load(const char * exported_file, MeshData & mesh)
{
	char* buffer;
	string mesh_file = exported_file; mesh_file += ".mesh";

	if (!App->fs->Load("../Library/Meshes/", mesh_file.c_str()))
		return false;

	char* cursor = buffer;

	unsigned int ranges[2]; //Load ranges
	unsigned int bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);

	mesh.nIndices = ranges[0];
	mesh.nVertices = ranges[1];

	cursor += bytes; // Load indices
	bytes = sizeof(unsigned int) * mesh.nIndices;
	mesh.indices = new unsigned int[mesh.nIndices];
	memcpy(mesh.indices, cursor, bytes);

	cursor += bytes; // Load vertex positions
	bytes = sizeof(float) * mesh.nVertices * 3;
	mesh.positions = new float[mesh.nVertices * 3];
	memcpy(mesh.positions, cursor, bytes);

	cursor += bytes; // Load vertex normals
	bytes = sizeof(float) * mesh.nVertices * 3;
	mesh.normals = new float[mesh.nVertices * 3];
	memcpy(mesh.normals, cursor, bytes);

	cursor += bytes; // Load vertex texture coords
	bytes = sizeof(float) * mesh.nVertices * 2;
	mesh.texture_coords = new float[mesh.nVertices * 2];
	memcpy(mesh.texture_coords, cursor, bytes);

	mesh.name = exported_file;

	return true;
}