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
	string directory;
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
		unsigned program = 0;
		unsigned diffuse_map = 0;
		math::float4 object_color = math::float4::zero;
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
	void RenderMesh(const Figure& mesh, const Material& material,
		const math::float4x4& model, const math::float4x4& view, const math::float4x4& proj);

	void CreateSphere(const char* name, const math::float3& pos, const math::Quat& rot, float size,
		unsigned slices, unsigned stacks, const math::float4& color);
	void CreateTorus(const char* name, const math::float3& pos, const math::Quat& rot, float inner_r, float outer_r,
		unsigned slices, unsigned stacks, const math::float4& color);
	void CreateCylinder(const char* name, const math::float3& pos, const math::Quat& rot, float height,
		float radius, unsigned slices, unsigned stacks, const math::float4& color);
	void CreateCube(const char* name, const math::float3& pos, const math::Quat& rot, float size, const math::float4& color);
private:	
	void GenerateVAO(Figure& mesh);

	void GenerateMesh(const char* name, const math::float3& pos, const math::Quat& rot, par_shapes_mesh_s* shape);
	void GenerateMeshes(const aiScene* scene);
	
	void Load(const char*);
	void processNode(aiNode*, const aiScene*);
	Mesh processMesh(aiMesh*, const aiScene*);
	vector<Texture> loadMaterialTextures(aiMaterial*, aiTextureType, string);
	string GetModelDirectory(const char*);
	string GetFilename(const char*);

public:

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


