#ifndef __MODULEMODELLOADER_H__
#define __MODULEMODELLOADER_H__

#include "Module.h"
#include "Model.h"



#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Logger.hpp"
#include "assimp/LogStream.hpp"
#include "MathGeoLib.h"
#include "Geometry/AABB.h"
#include "ModuleTexture.h"

struct aiScene;
struct par_shapes_mesh_s;

using namespace std;


class ModuleModelLoader : public Module {
public:
	std::vector<Model*> models;
	vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	string directory, model_name;
	
	ModuleModelLoader();
	~ModuleModelLoader();

	update_status Update();


	void AddModel(const char* filePath);
	Model* GetModel(const char* filePath);

	bool Init();
	bool CleanUp();

	aiVector3D modelPosition;
	aiVector3D modelScale;
	aiVector3D modelRotation;

	struct Figure
	{
		std::string     name;
		unsigned        vertex_size = 0;
		unsigned        vao = 0;
		unsigned        vbo = 0;
		unsigned        ibo = 0;
		unsigned        num_vertices = 0;
		unsigned        num_indices = 0;
		unsigned        texcoords_offset = 0;
		unsigned        normals_offset = 0;
		unsigned        material = 0;
		math::float4x4  transform = math::float4x4::identity;
	};

	
private:
	
	struct Sphere
	{
		math::float3 center = math::float3::zero;
		float        radius = 0.0f;
	};

	struct Light
	{
		math::float3 color = math::float3(0.2f, 0.2f, 0.2f);
		math::float3 pos = math::float3::one;

	};
public:	
	bool load_once = false;
	void RenderMesh(const Figure& mesh, const Material& material,
		const math::float4x4& model, const math::float4x4& view, const math::float4x4& proj);

	void CreateSphere(const char* name, const math::float3& pos, const math::Quat& rot, float size,
		unsigned slices, unsigned stacks, const math::float4& color);
	void CreateTorus(const char* name, const math::float3& pos, const math::Quat& rot, float inner_r, float outer_r,
		unsigned slices, unsigned stacks, const math::float4& color);
	void CreateCylinder(const char* name, const math::float3& pos, const math::Quat& rot, float height,
		float radius, unsigned slices, unsigned stacks, const math::float4& color);
	void CreateCube(const char* name, const math::float3& pos, const math::Quat& rot, float size, const math::float4& color);
	string GetFilename(const char*);
private:	
	void GenerateVAO(Figure& mesh);
	
	void GenerateMesh(const char* name, const math::float3& pos, const math::Quat& rot, par_shapes_mesh_s* shape);

	bool item_exists(const char* path);
	void UpdateFigures();
	void Load(const char*);
	void processNode(aiNode*, const aiScene*);
	Mesh processMesh(aiMesh*, const aiScene*);
	vector<Texture> loadMaterialTextures(aiMaterial*, aiTextureType, string);
	string GetModelDirectory(const char*);


public:
	vector<Material> materials;
	std::vector<Figure>     figures;

	Sphere                bsphere;
	Light                 light;
	float			      ambient = 0.0f;
	float4				  color;
	unsigned int shader;

private:

	math::float3          min_v = math::float3(FLT_MAX, FLT_MAX, FLT_MAX);
	math::float3          max_v = math::float3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

};
#endif // __MODULEMODELLOADER_H__


