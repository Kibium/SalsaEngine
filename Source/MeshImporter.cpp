#include "MeshImporter.h"

#include "Application.h"
#include "ModuleFilesystem.h"
#include "PhysFS/include/physfs.h"
#pragma comment( lib, "Libraries/PhysFS/lib/physfs.lib" )


using namespace std;

bool MeshImporter::Import(const char * file, const MeshData & mesh, string & output_file)
{

	LOG("We're saving %d positions\n", mesh.positions.size());

	unsigned int ranges[2] = { mesh.nIndices, mesh.nVertices };

	unsigned int size = sizeof(ranges)					//ranges
		+ sizeof(unsigned int) * mesh.nIndices		//indices
		+ sizeof(float) * mesh.nVertices * 3			//vertex positions
		+ sizeof(float) * mesh.nVertices * 2;		//vertex texture coord

	char* data = new char[size]; // Allocate
	char* cursor = data;

	unsigned int bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);

	cursor += bytes; // Store indices
	bytes = sizeof(unsigned int) * mesh.nIndices;
	memcpy(cursor, &mesh.indices.front(), bytes);

	cursor += bytes; // Store vertex positions
	bytes = sizeof(float3) * mesh.nVertices;
	memcpy(cursor, &mesh.positions.front(), bytes);

	cursor += bytes; // Store vertex texture coords
	bytes = sizeof(float) * mesh.nVertices * 2;
	memcpy(cursor, &mesh.texture_coords.front(), bytes);

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

	return App->fs->Save("../Library/Meshes/", file, buffer, size);
}

bool MeshImporter::Load(const char * exported_file, MeshData &mesh)
{
	string mesh_file = exported_file; 
	mesh_file += ".mesh";
	
	string s = "../Library/Meshes/" + mesh_file;

	if (!App->fs->Exists(s.c_str()))
		return false;

	else {

		char* buffer = App->fs->Load("../Library/Meshes/", mesh_file.c_str());
		LOG("Loading model...\n");

		char* cursor = buffer;

		unsigned int ranges[2]; //Load ranges
		unsigned int bytes = sizeof(ranges);
		memcpy(ranges, cursor, bytes);

		mesh.nIndices = ranges[0];
		mesh.nVertices = ranges[1];

		cursor += bytes; // Load indices
		bytes = sizeof(unsigned int) * mesh.nIndices;
		mesh.indices.resize(mesh.nIndices);
		memcpy(&mesh.indices.front(), cursor, bytes);

		cursor += bytes; // Load vertex positions
		bytes = sizeof(float) * mesh.nVertices * 3;
		mesh.positions.resize(mesh.nVertices * 3);
		memcpy(&mesh.positions.front(), cursor, bytes);


		cursor += bytes; // Load vertex texture coords
		bytes = sizeof(float) * mesh.nVertices * 2;
		mesh.texture_coords.resize(mesh.nVertices * 2);
		memcpy(&mesh.texture_coords.front(), cursor, bytes);

		mesh.name = exported_file;

		return true;
	}
	
}