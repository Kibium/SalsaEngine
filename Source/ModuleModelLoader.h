#ifndef __MODULEMODELLOADER_H__
#define __MODULEMODELLOADER_H__

#include "Module.h"
#include "Mesh.h"

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
class myStream : public Assimp::LogStream
{
public:
	myStream(){}
	~myStream(){}
	void write(const char* message)
	{
		LOG("ASSIMP:: %s\n", message);
	}
};
class ModuleModelLoader : public Module {
public:

	vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Mesh> meshes;
	string directory, model_name;
	vector<int> textureWidth;
	vector<int> textureHeight;
	



	ModuleModelLoader();
	~ModuleModelLoader();

	update_status Update();

	void Draw();
	bool Init();
	bool CleanUp();
	void SwitchModel(const char*);
	void SwitchTexture(const char*);
	void RenderAABB();
	aiVector3D modelPosition;
	aiVector3D modelScale;
	aiVector3D modelRotation;
	bool model = false;
	AABB modelBox;
	int npolys = 0;
	int nvertex = 0;
	int nmeshes= 0;



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

	struct Material
	{
		
		//Textures
		unsigned diffuse_map = 0;
		unsigned specular_map = 0;
		unsigned occlusion_map = 0;
		unsigned emissive_map = 0;

		//Colors
		math::float4 diffuse_color = math::float4::zero;
		math::float4 specular_color = math::float4::zero;
		math::float4 occlusion_color = math::float4::zero;
		math::float4 emissive_color = math::float4::zero;

		//Constants
		float shininess = 0.0f;

		float k_specular = 0.0f;
		float k_diffuse = 0.0f;
		float k_ambient = 0.0f;
	};
private:

	struct Sphere
	{
		math::float3 center = math::float3::zero;
		float        radius = 0.0f;
	};

	struct Light
	{
		math::float4 color = math::float4::one;
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
	void LoadTexture(vector<Texture>& v, TextureType type);
	void GenerateMesh(const char* name, const math::float3& pos, const math::Quat& rot, par_shapes_mesh_s* shape);
	void GenerateMeshes(const aiScene* scene);
	void GenerateMaterials(const aiScene* scene);
	bool item_exists(const char* path);
	
	void Load(const char*);
	void processNode(aiNode*, const aiScene*);
	Mesh processMesh(aiMesh*, const aiScene*);
	vector<Texture> loadMaterialTextures(aiMaterial*, aiTextureType, string);
	string GetModelDirectory(const char*);


public:
	vector<Material> materials;
	std::vector<Figure>     figures;
	std::vector<Material> materials;
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


