#version 410

layout(location = 0) in vec3 pos;
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

  FragPos = vec3(model * vec4(pos, 1.0));
  Normal = (model*vec4(normals, 0.0)).xyz;
  texCoord = uv0;
  gl_Position = proj*view*vec4(FragPos, 1.0);
  auxView = view;
}
