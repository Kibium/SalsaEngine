#version 330 

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec4 gl_Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
out vec4 myColor;

void main(){   

	gl_Position = proj * view * model * vec4(vertex_position, 1.0);
	myColor = gl_Color;

}