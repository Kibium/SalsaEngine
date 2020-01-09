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
in mat4 auxView;


uniform sampler2D ourTexture;
uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 light_dir = normalize(light.position - FragPos);
    float diff = max(0.0, dot(norm, light_dir));
	float spec =0;    

	if(diff > 0.0 && material.shininess > 0.0)
    {
        vec3 view_pos    = transpose(mat3(auxView))*(-auxView[3].xyz);
        vec3 view_dir    = normalize(view_pos-FragPos);
        vec3 reflect_dir = normalize(reflect(-light_dir, norm));
        float sp         = max(dot(view_dir, reflect_dir), 0.0);

        if(sp > 0.0)
        {
			//speculars
            spec = pow(sp, material.shininess); 
        }
    }

	//ambient
	vec3 ambient = light.ambient * texture(material.occ_map, texCoord).rgb * texture(material.diff_map, texCoord).rgb;

	//diffuse	
    vec3 diffuse = light.diffuse* (diff *texture(material.diff_map, texCoord).rgb);  

	vec3 specular = light.specular * spec * texture(material.spec_map, texCoord).rgb;  

	vec3 result = material.k_occ*ambient*material.occ_color.rgb + material.k_diff*diffuse*material.diff_color.rgb + material.k_spec*specular*material.spec_color.rgb;

    color = vec4(result, 1.0);

	
}
