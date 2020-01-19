#include "ModuleModelLoader.h"
#include "Application.h"
#include "ModuleShader.h"


//textrues are no longer supported
#include "ModuleTexture.h"
#include "ModuleRender.h"
#include "ModuleFileSystem.h"
#include "MeshImporter.h"

#include "ModuleCamera.h"
#include "assimp/DefaultLogger.hpp"
#include <assimp/cimport.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include "MeshImporter.h"
#define PAR_SHAPES_IMPLEMENTATION
#include "Util/par_shapes.h"
using namespace Assimp;

ModuleModelLoader::ModuleModelLoader() {
}


void ModuleModelLoader::UpdateFigures() {
	for (int i = 0; i < figures.size(); ++i) {
		figures[i].transform = math::float4x4(math::Quat::identity, light.pos);
	}
}


void ModuleModelLoader::GenerateMesh(const char* name, const math::float3& pos, const math::Quat& rot, par_shapes_mesh* shape)
{
	Figure dst_mesh;

	dst_mesh.name = name;
	dst_mesh.transform = math::float4x4(rot, pos);

	glGenBuffers(1, &dst_mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, dst_mesh.vbo);

	// Positions

	for (unsigned i = 0; i< unsigned(shape->npoints); ++i)
	{
		math::float3 point(shape->points[i * 3], shape->points[i * 3 + 1], shape->points[i * 3 + 2]);
		point = dst_mesh.transform.TransformPos(point);
		for (unsigned j = 0; j < 3; ++j)
		{
			min_v[j] = min(min_v[j], point[i]);
			max_v[j] = max(max_v[j], point[i]);
		}
	}

	unsigned offset_acc = sizeof(math::float3);

	if (shape->normals)
	{
		dst_mesh.normals_offset = offset_acc;
		offset_acc += sizeof(math::float3);
	}

	dst_mesh.vertex_size = offset_acc;

	glBufferData(GL_ARRAY_BUFFER, dst_mesh.vertex_size*shape->npoints, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(math::float3)*shape->npoints, shape->points);

	// normals

	if (shape->normals)
	{
		glBufferSubData(GL_ARRAY_BUFFER, dst_mesh.normals_offset*shape->npoints, sizeof(math::float3)*shape->npoints, shape->normals);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// indices

	glGenBuffers(1, &dst_mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dst_mesh.ibo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*shape->ntriangles * 3, nullptr, GL_STATIC_DRAW);

	unsigned* indices = (unsigned*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0,
		sizeof(unsigned)*shape->ntriangles * 3, GL_MAP_WRITE_BIT);

	for (unsigned i = 0; i< unsigned(shape->ntriangles * 3); ++i)
	{
		*(indices++) = shape->triangles[i];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	dst_mesh.material = 0;
	dst_mesh.num_vertices = shape->npoints;
	dst_mesh.num_indices = shape->ntriangles * 3;


	GenerateVAO(dst_mesh);

	figures.push_back(dst_mesh);

	bsphere.center = (max_v + min_v)*0.5f;
	bsphere.radius = (max_v - min_v).Length()*0.5f;
}

void ModuleModelLoader::GenerateVAO(Figure& mesh)
{
	glGenVertexArrays(1, &mesh.vao);

	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	if (mesh.texcoords_offset != 0)
	{
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(mesh.texcoords_offset*mesh.num_vertices));
	}

	if (mesh.normals_offset != 0)
	{
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)(mesh.normals_offset*mesh.num_vertices));
	}



	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModuleModelLoader::RenderMesh(const Figure& mesh, const Material& material,
	const math::float4x4& model, const math::float4x4& view, const math::float4x4& proj)
{
	unsigned program = shader;
	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);

	glUniform3fv(glGetUniformLocation(program, "light_pos"), 1, (const float*)&light.pos);
	glUniform1f(glGetUniformLocation(program, "ambient"), ambient);
	glUniform1f(glGetUniformLocation(program, "shininess"), material.shininess);
	glUniform1f(glGetUniformLocation(program, "k_ambient"), material.k_ambient);
	glUniform1f(glGetUniformLocation(program, "k_diffuse"), material.k_diffuse);
	glUniform1f(glGetUniformLocation(program, "k_specular"), material.k_specular);

	if (material.diffuse_map == 0)
	{
		glUniform1i(glGetUniformLocation(program, "use_diffuse_map"), 0);
		glUniform4fv(glGetUniformLocation(program, "object_color"), 1, (const float*)&material.diffuse_color);
	}
	else
	{
		glUniform1i(glGetUniformLocation(program, "use_diffuse_map"), 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.diffuse_map);
		glUniform1i(glGetUniformLocation(program, "diffuse_map"), 0);
	}

	glBindVertexArray(mesh.vao);
	glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	//glUseProgram(App->shader->grid_program);
}

void ModuleModelLoader::CreateSphere(const char* name, const math::float3& pos, const math::Quat& rot, float size,
	unsigned slices, unsigned stacks, const math::float4& color) {

	par_shapes_mesh* mesh = par_shapes_create_parametric_sphere(int(slices), int(stacks));

	if (mesh)
	{
		par_shapes_scale(mesh, size, size, size);

		GenerateMesh(name, pos, rot, mesh);
		par_shapes_free_mesh(mesh);

		figures.back().material = materials.size();


		Material mat;
		mat.diffuse_color = color;

		materials.push_back(mat);
	}

}

void ModuleModelLoader::CreateTorus(const char* name, const math::float3& pos, const math::Quat& rot, float inner_r, float outer_r,
	unsigned slices, unsigned stacks, const math::float4& color)
{
	par_shapes_mesh* mesh = par_shapes_create_torus(int(slices), int(stacks), inner_r);

	if (mesh)
	{
		par_shapes_scale(mesh, outer_r, outer_r, outer_r);
		GenerateMesh(name, pos, rot, mesh);
		par_shapes_free_mesh(mesh);

		figures.back().material = materials.size();

		Material mat;
		mat.diffuse_color = color;

		materials.push_back(mat);


	}


}

void ModuleModelLoader::CreateCylinder(const char* name, const math::float3& pos, const math::Quat& rot, float height,
	float radius, unsigned slices, unsigned stacks, const math::float4& color) {
	par_shapes_mesh* mesh = par_shapes_create_cylinder(int(slices), int(stacks));
	par_shapes_rotate(mesh, -float(PAR_PI*0.5), (float*)&math::float3::unitX);
	par_shapes_translate(mesh, 0.0f, -0.5f, 0.0f);

	par_shapes_mesh* top = par_shapes_create_disk(radius, int(slices), (const float*)&math::float3::zero, (const float*)&math::float3::unitZ);
	par_shapes_rotate(top, -float(PAR_PI*0.5), (float*)&math::float3::unitX);
	par_shapes_translate(top, 0.0f, height*0.5f, 0.0f);

	par_shapes_mesh* bottom = par_shapes_create_disk(radius, int(slices), (const float*)&math::float3::zero, (const float*)&math::float3::unitZ);
	par_shapes_rotate(bottom, float(PAR_PI*0.5), (float*)&math::float3::unitX);
	par_shapes_translate(bottom, 0.0f, height*-0.5f, 0.0f);

	if (mesh)
	{
		par_shapes_scale(mesh, radius, height, radius);
		par_shapes_merge_and_free(mesh, top);
		par_shapes_merge_and_free(mesh, bottom);

		GenerateMesh(name, pos, rot, mesh);
		par_shapes_free_mesh(mesh);

		figures.back().material = materials.size();

		Material mat;
		mat.diffuse_color = color;

		materials.push_back(mat);


	}


}

void ModuleModelLoader::CreateCube(const char* name, const math::float3& pos, const math::Quat& rot, float size, const math::float4& color)
{
	par_shapes_mesh* mesh = par_shapes_create_plane(1, 1);
	par_shapes_mesh* top = par_shapes_create_plane(1, 1);
	par_shapes_mesh* bottom = par_shapes_create_plane(1, 1);
	par_shapes_mesh* back = par_shapes_create_plane(1, 1);
	par_shapes_mesh* left = par_shapes_create_plane(1, 1);
	par_shapes_mesh* right = par_shapes_create_plane(1, 1);

	par_shapes_translate(mesh, -0.5f, -0.5f, 0.5f);

	par_shapes_rotate(top, -float(PAR_PI*0.5), (float*)&math::float3::unitX);
	par_shapes_translate(top, -0.5f, 0.5f, 0.5f);

	par_shapes_rotate(bottom, float(PAR_PI*0.5), (float*)&math::float3::unitX);
	par_shapes_translate(bottom, -0.5f, -0.5f, -0.5f);

	par_shapes_rotate(back, float(PAR_PI), (float*)&math::float3::unitX);
	par_shapes_translate(back, -0.5f, 0.5f, -0.5f);

	par_shapes_rotate(left, float(-PAR_PI * 0.5), (float*)&math::float3::unitY);
	par_shapes_translate(left, -0.5f, -0.5f, -0.5f);

	par_shapes_rotate(right, float(PAR_PI*0.5), (float*)&math::float3::unitY);
	par_shapes_translate(right, 0.5f, -0.5f, 0.5f);

	par_shapes_merge_and_free(mesh, top);
	par_shapes_merge_and_free(mesh, bottom);
	par_shapes_merge_and_free(mesh, back);
	par_shapes_merge_and_free(mesh, left);
	par_shapes_merge_and_free(mesh, right);

	if (mesh)
	{
		par_shapes_scale(mesh, size, size, size);
		GenerateMesh(name, pos, rot, mesh);
		par_shapes_free_mesh(mesh);

		figures.back().material = materials.size();

		Material mat;
		mat.diffuse_color = color;

		materials.push_back(mat);


	}


}

update_status ModuleModelLoader::Update() {

	UpdateFigures();

	return UPDATE_CONTINUE;
}


bool ModuleModelLoader::Init() {
	LOG("Init Model Loader\n");


	//Light stuff
	light.pos = math::float3(0.0f, 0.0f, 0.0f);

	App->model->CreateSphere("sphere1", light.pos, math::Quat::identity, 0.5f, 30, 30, float4(204, 204, 0, 1));
	App->model->materials.back().k_specular = 0.9f;
	App->model->materials.back().shininess = 64.0f;
	App->model->materials.back().k_specular = 0.6f;
	App->model->materials.back().k_diffuse = 0.5f;
	App->model->materials.back().k_ambient = 1.0f;


	ambient = 0.3f;

	//material color
	color = float4(1.0f, 1.0f, 1.0f, 1.0f);

	//Initialize shader
	shader = App->shader->phong_program;
	return true;
}



ModuleModelLoader::~ModuleModelLoader() {
	if (models.size() > 0) {
		for (int i = 0; i < models.size(); ++i)
			delete models[i];

	}
}

void ModuleModelLoader::AddModel(const char *filePath) {
	Model *model = new Model(filePath);
	string path = "../Library/" + model->GetFileName(filePath);

	//import
	
	MeshData md;
	MeshImporter i;
	i.Load(model->GetFileName(filePath).c_str(), md);

	//LOG("There were %d indices\n", md.indices.size());
	models.push_back(model);
}

void ModuleModelLoader::Load(const char* path)
{
	DefaultLogger::create("", Logger::VERBOSE);
	const unsigned int severity = Logger::Debugging | Logger::Info | Logger::Err | Logger::Warn;
	DefaultLogger::get()->attachStream(new myStream(), severity);
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcessPreset_TargetRealtime_MaxQuality);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		LOG("ERROR::ASSIMP:: %s \n", importer.GetErrorString());
		return;
	}

	directory = GetModelDirectory(path);
	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);


	DefaultLogger::kill();

	
}

void ModuleModelLoader::processNode(aiNode *node, const aiScene *scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

	}

	node->mTransformation.Decompose(modelScale, modelRotation, modelPosition);

	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
	App->camera->Focus();
}


Model* ModuleModelLoader::GetModel(const char *filePath) {
	if (filePath == nullptr || models.size() < 1) return nullptr;

	for (int i = 0; i < models.size(); ++i) {
		if (filePath == models[i]->filePath) {
			return models[i];
		}
	}

	return nullptr;

}

bool ModuleModelLoader::item_exists(const char* path) {


	FILE *file = nullptr;

	fopen_s(&file, path, "rb");
	if (file)
		return true;

	else
		return false;
}
/*DIFFUSE IN PNG
SPECULAR IN TIF
OCCLUSSION IN PNG*/




/*Mesh ModuleModelLoader::processMesh(aiMesh *mesh, const aiScene *scene)
{
	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		vertex.Position = float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.TexCoords = float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		vertex.Normals = float3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		//LOG("Texture %d coord x %f coord y %f \n", i, mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		vertices.push_back(vertex);
	}


	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}


	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	// return a mesh object created from the extracted mesh data
*/


/*should load the texture based on the mesh*/
/*now loads a texture and applies it to all  meshes

}*/

vector<Texture> ModuleModelLoader::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			std::string newPath = directory.c_str() + string(str.C_Str());
			const char* pathing = newPath.c_str();
			texture.id = App->texture->Load((char*)pathing);
			if (!App->texture->loaded) {
				LOG("Loading Texture from model's directory\n");
				std::string filename = directory.c_str() + GetFilename(str.C_Str());
				const char* file = filename.c_str();
				texture.id = App->texture->Load((char*)file);
				texture.path = file;
				if (!App->texture->loaded) {
					LOG("Loading Texture from Textures directory\n");
					string fromFolder = "Models/Textures/" + GetFilename(str.C_Str());
					const char* folder = fromFolder.c_str();
					texture.id = App->texture->Load((char*)folder);
					texture.path = folder;
				}
			}
			else {
				texture.path = str.C_Str();
			}
			if (App->texture->loaded) {
				LOG("DEVIL:: Texture Loaded Succesfully\n");
			}
			else {
				LOG("DEVIL::ERROR  Loading texture. File not found \n");
			}
			texture.type = typeName;
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesary load duplicate textures.
		}
	}//
	return textures;
}
string ModuleModelLoader::GetModelDirectory(const char *path)
{
	std::string dir = std::string(path);

	std::size_t currentDir = dir.find_last_of("/\\");
	std::string modelDir = dir.substr(0, currentDir + 1);
	return path;

}

string ModuleModelLoader::GetFilename(const char *path)
{
	std::string dir = std::string(path);

	std::size_t currentDir = dir.find_last_of("/\\");
	std::string filename = dir.substr(currentDir + 1);
	filename = filename.substr(0, filename.find('.'));

	return filename;
}

bool ModuleModelLoader::CleanUp() {
	return true;
}
