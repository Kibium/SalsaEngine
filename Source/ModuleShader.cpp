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
	LOG("Init Shaders\n");
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

	test_program = glCreateProgram();
	glAttachShader(test_program, vertex);
	glAttachShader(test_program, fragment);
	glLinkProgram(test_program);
	checkCompileErrors(test_program, "PROGRAM");

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);


	//CUTRE
	unsigned int vertex2, fragment2;
	try
	{
		// open files
		vShaderFile.open("./shaders/skybox.vs.txt");
		fShaderFile.open("./shaders/skybox.fs.txt");
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
	vShaderCode = vertexCode.c_str();
	fShaderCode = fragmentCode.c_str();
	// 2. compile shaders
	// vertex shader
	vertex2 = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex2, 1, &vShaderCode, NULL);
	glCompileShader(vertex2);
	checkCompileErrors(vertex2, "VERTEX");
	// fragment Shader
	fragment2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment2, 1, &fShaderCode, NULL);
	glCompileShader(fragment2);
	checkCompileErrors(fragment2, "FRAGMENT");
	// shader Program
	skybox_program = glCreateProgram();
	glAttachShader(skybox_program, vertex2);
	glAttachShader(skybox_program, fragment2);
	glLinkProgram(skybox_program);
	checkCompileErrors(skybox_program, "PROGRAM");

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex2);
	glDeleteShader(fragment2);


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
			LOG("ERROR::SHADER_COMPILATION_ERROR of type: ",type,"\n", infoLog,"\n -- --------------------------------------------------- -- \n" );
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			LOG( "ERROR::PROGRAM_LINKING_ERROR of type: ", "\n",infoLog, "\n -- --------------------------------------------------- -- \n ");
		}
	}
}
