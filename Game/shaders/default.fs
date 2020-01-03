#version 330 core

out vec4 color;

struct Material{

	//Textures
	sampler2D diff_map;
	sampler2D spec_map;
	sampler2D occ_map;
	sampler2D emi_map;

	//Colors
	vec4 diff_color;
	vec4 spec_color;
	vec4 occ_color;
	vec4 emi_color;

	//Constants
	float k_diff;
	float k_spec;
	float k_occ;
	float k_emi;

	//Shininess
	float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 texCoord;
in vec3 FragPos;
in vec3 Normal;


uniform sampler2D ourTexture;
uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{
	//ambient
	vec3 ambient = light.ambient * texture(material.diff_map, texCoord).rgb;

	//diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse* diff *texture(material.diff_map, texCoord).rgb;  

	//specular
	vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);    
	vec3 specular = light.specular * spec * texture(material.spec_map, texCoord).rgb;  

	vec3 result = ambient + diffuse+ specular;

    color = vec4(result, 1.0);

	
}
