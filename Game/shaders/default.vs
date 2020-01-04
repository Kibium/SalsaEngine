#version 330

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 uv0;
layout(location = 2) in vec3 normals;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec3 Normal;
out vec2 texCoord;
out vec3 FragPos;
out mat4 auxView;

void main()
{

  FragPos = vec3(model * vec4(vertex_position, 1.0));
  Normal = mat3(transpose(inverse(model))) * normals;
  texCoord = uv0;
  gl_Position = proj*view*vec4(FragPos, 1.0);
  auxView = view;
}
