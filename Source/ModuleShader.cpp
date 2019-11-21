#include "ModuleShader.h"
#include <iostream>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


ModuleShader::ModuleShader()
{
}


ModuleShader::~ModuleShader()
{
}

bool ModuleShader::Init()
{
	/*const char *vertexShader = readShader("default.vs");
	const char *fragmentShader = readShader("default.fs");
	int comp;
	char  info[512];
	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	if (vertex == 0) {
		LOG("ERROR CREATING VERTEX \n");
	}
	glShaderSource(vertex, 1, &vertexShader, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &comp);
	if (!comp) {
		glGetShaderInfoLog(vertex, 512, NULL, info);
		LOG("Error compiling vertex shader\n");
		LOG(info);
	}
	else {
		LOG("Vector Shader created succesfully \n");
	}
	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	if (fragment == 0) {
		LOG("ERROR CREATING FRAGMENT\n");
	}
	glShaderSource(fragment, 1, &fragmentShader, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &comp);
	if (!comp) {
		glGetShaderInfoLog(fragment, 512, NULL, info);
		LOG("Error compiling fragment shader\n");
		LOG(info);
	}
	else {
		LOG("Fragment Shader created succesfully \n");
	}
	def_program = glCreateProgram();
	glAttachShader(def_program, vertex);
	glAttachShader(def_program, fragment);
	glLinkProgram(def_program);
	glGetProgramiv(def_program, GL_LINK_STATUS, &comp);
	if (!comp) {
		glGetProgramInfoLog(def_program, 512, NULL, info);
		LOG("Error linking shader program");
		LOG(info);
	}
	else {
		LOG("Program Shader created succesfully \n");
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);*/
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open("./shaders/default.vs");
		fShaderFile.open("./shaders/default.fs");
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		LOG("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	// 2. compile shaders
	unsigned int vertex, fragment;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	// shader Program
	def_program = glCreateProgram();
	glAttachShader(def_program, vertex);
	glAttachShader(def_program, fragment);
	glLinkProgram(def_program);
	checkCompileErrors(def_program, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	return true;

}

update_status ModuleShader::PreUpdate()
{
	return UPDATE_CONTINUE;
}

update_status ModuleShader::Update()
{
	return UPDATE_CONTINUE;

}

update_status ModuleShader::PostUpdate()
{
	return UPDATE_CONTINUE;

}

bool ModuleShader::CleanUp()
{
	return true;
}

const char* ModuleShader::readShader(const char* path) {

	char* shader = nullptr;
	FILE *file = nullptr;

	fopen_s(&file, path, "rb");
	if (file) {
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		rewind(file);
		shader = (char*)malloc(size + 1);

		fread(shader, 1, size, file);
		shader[size] = 0;

		fclose(file);
	}

	return shader;
}

void ModuleShader::checkCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			LOG("ERROR::SHADER_COMPILATION_ERROR of type: ",type,"\n", infoLog,"\n -- --------------------------------------------------- -- " );
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			LOG( "ERROR::PROGRAM_LINKING_ERROR of type: ", "\n",infoLog, "\n -- --------------------------------------------------- -- ");
		}
	}
}
