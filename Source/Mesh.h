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
};

struct Texture {
	unsigned int id;
	string type;
	string path;
};

class Mesh {
public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO;
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	void Draw();

private:
	unsigned int VBO, EBO;
	void setupMesh();
};
#endif
