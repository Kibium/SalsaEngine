#include "ModuleShader.h"
#include "MathGeoLib.h"
#include "ModuleCamera.h"
#include <iostream>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "optick/optick.h"

ModuleShader::ModuleShader()
{
}


ModuleShader::~ModuleShader()
{
}


bool ModuleShader::Init()
{
	LOG("Init Shaders\n");
	CreateShader(def_program, "./shaders/default.vs", "./shaders/default.fs");
	CreateShader(phong_program, "./Shaders/Phong.vs", "./Shaders/Phong.fs");
	CreateShader(grid_program, "./Shaders/lines.vs", "./Shaders/lines.fs");
	CreateShader(blinn_program, "./Shaders/Blinn.vs", "./Shaders/Blinn.fs");
	CreateShader(gouraud_program, "./Shaders/Gouraud.vs", "./Shaders/Gouraud.fs");
	CreateShader(flat_program, "./shaders/flat.vs", "./shaders/flat.fs");
	CreateShader(lines_program, "./shaders/lines.vs", "./shaders/lines.fs");
	CreateShader(skybox_program, "./shaders/skybox.vs.txt", "./shaders/skybox.fs.txt");

	return true;

}
void ModuleShader::CreateShader(GLuint &program, const char* vertexFile, const char* fragmentFile) {
	LOG("Creating Shader\n");
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
		vShaderFile.open(vertexFile);
		fShaderFile.open(fragmentFile);
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
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	checkCompileErrors(program, "PROGRAM");

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}
update_status ModuleShader::PreUpdate()
{
	return UPDATE_CONTINUE;
}

update_status ModuleShader::Update()
{
	OPTICK_CATEGORY("UpdateShader", Optick::Category::None);
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

void ModuleShader::checkCompileErrors(GLuint& shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);


			LOG("ERROR::SHADER_COMPILATION_ERROR of type: %s\n", infoLog);


		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);


			LOG( "ERROR::PROGRAM_LINKING_ERROR of type: %s\n", infoLog);


		}
	}
}
