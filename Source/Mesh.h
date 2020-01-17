#ifndef MESH_H
#define MESH_H

#include "MathGeoLib.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

struct Vertex {
	float3 Position;
	float2 TexCoords;
	float3 Normals;
};

struct Texture {
	unsigned int id;
	string type;
	string path;
};

struct Material
{

	//Textures
	unsigned diffuse_map = 0;
	unsigned specular_map = 0;
	unsigned occlusion_map = 0;
	unsigned emissive_map = 0;

	//Paths
	string diff_path, spec_path, occ_path, emi_path;

	//Colors
	math::float4 diffuse_color = math::float4(1, 1, 1, 1);
	math::float4 specular_color = math::float4(1, 1, 1, 1);
	math::float4 occlusion_color = math::float4(1, 1, 1, 1);
	math::float4 emissive_color = math::float4(1, 1, 1, 1);

	//Constants
	float shininess = 64;

	float k_specular = 1.0f;
	float k_diffuse = 1.0f;
	float k_ambient = 1.0f;
};

class Mesh {
public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	vector<Triangle> triangles;
	Material meshMaterial;
	unsigned int VAO;
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, Material m);
	void Draw();
	vector<Vertex> GetVertices();
	

private:
	unsigned int VBO, EBO;
	void setupMesh();
};
#endif // __MESH_H__
