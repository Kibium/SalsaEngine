#include "Mesh.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleShader.h"
#include "ModuleModelLoader.h"
#include "MathGeoLib.h"
#include "Geometry/AABB.h"
#include <glew.h>

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, Material m, int polygons, int totalVertices, AABB bb, AABB mb)
	: npolys(polygons), nvertex(totalVertices), boundingBox(bb), modelBox(mb) {
	this->vertices = vertices;
	this->indices = indices;
	this->meshMaterial = m;

	//Creating triangle data
	for (int i = 0; i < indices.size(); i += 3) {
		Triangle t = Triangle(vertices[indices[i]].Position, vertices[indices[i + 1]].Position, vertices[indices[i + 2]].Position);
		triangles.push_back(t);
	}
	//LOG("Tris: %d\n", triangles.size());
	setupMesh();

}
Mesh::Mesh(vector<float3> vertices, vector<unsigned int> indices, vector<float2> texture_coords, int numVertex) {
	vector<Vertex> auxVertex;
	//vector<float3> newVertex;
	//modelBox.Enclose(newVertex, numVertex);
	//boundingBox.Enclose((float3*)mesh->mVertices, mesh->mNumVertices);

	for (unsigned int i = 0; i < vertices.size(); ++i) {
		Vertex vertex;
		vertex.Position = vertices[i];
		vertex.TexCoords = texture_coords[i];
		auxVertex.push_back(vertex);
	}
	this->vertices = auxVertex;
	this->indices = indices;




	setupMesh();

}

vector<Vertex> Mesh::GetVertices() {
	return vertices;
}

void Mesh::LoadTexture(vector<Texture>& v, TextureType type, const std::string& directory) {
	string dir;
	Texture tex;

	switch (type) {
	case DIFFUSE:
		dir = directory + "Diffuse.png";
		tex.id = App->texture->Load(dir.c_str());

		if (item_exists(dir.c_str())) {
			tex.type = "diffuse";
			tex.path = dir;
			v.push_back(tex);

			mat.diffuse_map = tex.id;
			mat.diff_path = dir;
		}
		break;

	case SPECULAR:
		dir = directory + "Specular.tif";
		tex.id = App->texture->Load(dir.c_str());

		if (item_exists(dir.c_str())) {
			tex.type = "specular";
			tex.path = dir;
			v.push_back(tex);

			mat.specular_map = tex.id;
			mat.spec_path = dir;
		}
		break;

	case OCCLUSION:
		dir = directory + "Occlusion.png";
		tex.id = App->texture->Load(dir.c_str());

		if (item_exists(dir.c_str())) {
			tex.type = "occlusion";
			tex.path = dir;
			v.push_back(tex);

			mat.occlusion_map = tex.id;
			mat.occ_path = dir;
		}
		break;
	}

}

bool Mesh::item_exists(const char * path) {

	FILE *file = nullptr;

	fopen_s(&file, path, "rb");
	if (file)
		return true;

	else
		return false;
}

void Mesh::Draw() {
	glUniform3f(glGetUniformLocation(App->shader->def_program, "light.ambient"), 0.2f, 0.2f, 0.2f);
	glUniform3f(glGetUniformLocation(App->shader->def_program, "light.position"), App->model->light.pos.x, App->model->light.pos.y, App->model->light.pos.z);
	glUniform3f(glGetUniformLocation(App->shader->def_program, "light.diffuse"), 1, 1, 1);
	glUniform3f(glGetUniformLocation(App->shader->def_program, "light.specular"), 0.8f, 0.8f, 0.8f);

	glUniform4f(glGetUniformLocation(App->shader->def_program, "material.diff_color"), mat.diffuse_color.x, mat.diffuse_color.y, mat.diffuse_color.z, mat.diffuse_color.w);
	glUniform4f(glGetUniformLocation(App->shader->def_program, "material.spec_color"), mat.specular_color.x, mat.specular_color.y, mat.specular_color.z, mat.specular_color.w);
	glUniform4f(glGetUniformLocation(App->shader->def_program, "material.occ_color"), mat.occlusion_color.x, mat.occlusion_color.y, mat.occlusion_color.z, mat.occlusion_color.w);
	glUniform4f(glGetUniformLocation(App->shader->def_program, "material.emi_color"), mat.emissive_color.x, mat.emissive_color.y, mat.emissive_color.z, mat.emissive_color.w);

	glUniform1f(glGetUniformLocation(App->shader->def_program, "material.shininess"), mat.shininess);

	glUniform1f(glGetUniformLocation(App->shader->def_program, "material.k_spec"), mat.k_specular);
	glUniform1f(glGetUniformLocation(App->shader->def_program, "material.k_diff"), mat.k_diffuse);
	glUniform1f(glGetUniformLocation(App->shader->def_program, "material.k_occ"), mat.k_ambient);

	//Assigning "ids" to textures
	glUniform1i(glGetUniformLocation(App->shader->def_program, "material.diff_map"), 0);
	glUniform1i(glGetUniformLocation(App->shader->def_program, "material.spec_map"), 1);
	glUniform1i(glGetUniformLocation(App->shader->def_program, "material.occ_map"), 2);
	glUniform1i(glGetUniformLocation(App->shader->def_program, "material.emi_map"), 3);

	//applying textures to the variables deppending on its id
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mat.diffuse_map);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mat.specular_map);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mat.occlusion_map);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, mat.emissive_map);

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh() {
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// vertex texture coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	// vertex normal coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normals));

	glBindVertexArray(0);

}
