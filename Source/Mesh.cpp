#include "Mesh.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleShader.h"

#include <glew.h>




Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, Material m)
{

	this->vertices = vertices;
	this->indices = indices;
	this->meshMaterial = m;

	glUniform4f(glGetUniformLocation(App->shader->def_program, "material.diff_color"), meshMaterial.diffuse_color.x, meshMaterial.diffuse_color.y, meshMaterial.diffuse_color.z, meshMaterial.diffuse_color.w);
	glUniform4f(glGetUniformLocation(App->shader->def_program, "material.spec_color"), meshMaterial.specular_color.x, meshMaterial.specular_color.y, meshMaterial.specular_color.z, meshMaterial.specular_color.w);
	glUniform4f(glGetUniformLocation(App->shader->def_program, "material.occ_color"), meshMaterial.occlusion_color.x, meshMaterial.occlusion_color.y, meshMaterial.occlusion_color.z, meshMaterial.occlusion_color.w);
	glUniform4f(glGetUniformLocation(App->shader->def_program, "material.emi_color"), meshMaterial.emissive_color.x, meshMaterial.emissive_color.y, meshMaterial.emissive_color.z, meshMaterial.emissive_color.w);
	
	glUniform1f(glGetUniformLocation(App->shader->def_program, "material.shininess"), meshMaterial.shininess);

	glUniform1f(glGetUniformLocation(App->shader->def_program, "material.k_spec"), meshMaterial.k_specular);
	glUniform1f(glGetUniformLocation(App->shader->def_program, "material.k_diff"), meshMaterial.k_diffuse);
	glUniform1f(glGetUniformLocation(App->shader->def_program, "material.k_occ"), meshMaterial.k_ambient);
	



	setupMesh();


}

void Mesh::Draw()
{

	//Assigning "ids" to textures
	glUniform1i(glGetUniformLocation(App->shader->def_program, "material.diff_map"), 0);
	glUniform1i(glGetUniformLocation(App->shader->def_program, "material.spec_map"), 1);
	glUniform1i(glGetUniformLocation(App->shader->def_program, "material.occ_map"), 2);
	glUniform1i(glGetUniformLocation(App->shader->def_program, "material.emi_map"), 3);

	//applying textures to the variables deppending on its id
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, meshMaterial.diffuse_map);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, meshMaterial.specular_map);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, meshMaterial.occlusion_map);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, meshMaterial.emissive_map);

	

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);

}

void Mesh::setupMesh()
{
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
