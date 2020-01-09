#version 330 

out vec4 color;
in vec4 myColor;

void main(){    

	color = myColor;
	//color = texture2D(texture0, uv0);
	}