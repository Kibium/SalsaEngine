#include "ModuleShader.h"
#include "MathGeoLib.h"
#include "ModuleCamera.h"
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

void ModuleShader::createProgram(GLuint program, char* vs, char* fs) {

}

void ModuleShader::InitShader(GLuint& program, GLuint& VS, GLuint& FS, char* Vdata, char* Fdata) {

	const GLchar *vdata = Vdata;
	const GLchar *fdata = Fdata;

	//Creating Vertex Shader
	VS = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VS, 1, &vdata, NULL);
	glCompileShader(VS);

	//Creating Fragment Shader
	FS = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FS, 1, &fdata, NULL);
	glCompileShader(FS);

	//Check that my shaders compile
	int  success = 0;
	char infoLog[512];

	checkCompileErrors(VS, "VERTEX");

	checkCompileErrors(FS, "FRAGMENT");



	//Create program object
	program = glCreateProgram();

	//Attach shaders
	glAttachShader(program, VS);
	glAttachShader(program, FS);

	//Prepare to execute program
	glLinkProgram(program);

	//Check for errors during program compilation
	checkCompileErrors(program, "PROGRAM");
}

std::string ModuleShader::getShadertext(char* source) {
	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		file.open(source);
		std::stringstream vShaderStream;
		vShaderStream << file.rdbuf();
		file.close();

		return vShaderStream.str();
	}

	catch (std::ifstream::failure e)
	{
		LOG("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
		return "error";
	}
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
		vShaderFile.open("./Shaders/default.vs");
		fShaderFile.open("./Shaders/default.fs");
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

	//TODO: This has to change
	//.............PHONG.................//

	try
	{
		// open files
		vShaderFile.open("./Shaders/Phong.vs");
		fShaderFile.open("./Shaders/Phong.fs");
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
	phong_program = glCreateProgram();
	glAttachShader(phong_program, vertex);
	glAttachShader(phong_program, fragment);
	glLinkProgram(phong_program);
	checkCompileErrors(phong_program, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	int rotx = 0, roty = 0, rotz = 0;
	float3x3 rotateM = float3x3::RotateX(rotx) * float3x3::RotateY(roty) * float3x3::RotateZ(rotz);

	float4x4 model = float4x4::FromTRS(float3(0, 3, 0), rotateM, float3(1, 1, 1));

	glUseProgram(phong_program);
	glUniformMatrix4fv(glGetUniformLocation(phong_program, "model"), 1, GL_TRUE, &model[0][0]); //Calculating vertexs in the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(phong_program, "view"), 1, GL_TRUE, &App->camera->view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(phong_program, "proj"), 1, GL_TRUE, &App->camera->proj[0][0]);

	//TODO: This has to change
	//.............GRID.................//

	try
	{
		// open files
		vShaderFile.open("./Shaders/lines.vs");
		fShaderFile.open("./Shaders/lines.fs");
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
	grid_program = glCreateProgram();
	glAttachShader(grid_program, vertex);
	glAttachShader(grid_program, fragment);
	glLinkProgram(grid_program);
	checkCompileErrors(grid_program, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	glUseProgram(grid_program);
	glUniformMatrix4fv(glGetUniformLocation(grid_program, "model"), 1, GL_TRUE, &model[0][0]); //Calculating vertexs in the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(grid_program, "view"), 1, GL_TRUE, &App->camera->view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(grid_program, "proj"), 1, GL_TRUE, &App->camera->proj[0][0]);

	//TODO: This has to change
	//.............FLAT.................//

	try
	{
		// open files
		vShaderFile.open("./Shaders/flat.vs");
		fShaderFile.open("./Shaders/flat.fs");
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
	flat_program = glCreateProgram();
	glAttachShader(flat_program, vertex);
	glAttachShader(flat_program, fragment);
	glLinkProgram(flat_program);
	checkCompileErrors(flat_program, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	glUseProgram(flat_program);
	glUniformMatrix4fv(glGetUniformLocation(flat_program, "model"), 1, GL_TRUE, &model[0][0]); //Calculating vertexs in the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(flat_program, "view"), 1, GL_TRUE, &App->camera->view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(flat_program, "proj"), 1, GL_TRUE, &App->camera->proj[0][0]);

	try
	{
		// open files
		vShaderFile.open("./Shaders/Blinn.vs");
		fShaderFile.open("./Shaders/Blinn.fs");
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
	blinn_program = glCreateProgram();
	glAttachShader(blinn_program, vertex);
	glAttachShader(blinn_program, fragment);
	glLinkProgram(blinn_program);
	checkCompileErrors(blinn_program, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	glUseProgram(blinn_program);
	glUniformMatrix4fv(glGetUniformLocation(blinn_program, "model"), 1, GL_TRUE, &model[0][0]); //Calculating vertexs in the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(blinn_program, "view"), 1, GL_TRUE, &App->camera->view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(blinn_program, "proj"), 1, GL_TRUE, &App->camera->proj[0][0]);

	try
	{
		// open files
		vShaderFile.open("./Shaders/Gouraud.vs");
		fShaderFile.open("./Shaders/Gouraud.fs");
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
	gouraud_program = glCreateProgram();
	glAttachShader(gouraud_program, vertex);
	glAttachShader(gouraud_program, fragment);
	glLinkProgram(gouraud_program);
	checkCompileErrors(gouraud_program, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	glUseProgram(gouraud_program);
	glUniformMatrix4fv(glGetUniformLocation(gouraud_program, "model"), 1, GL_TRUE, &model[0][0]); //Calculating vertexs in the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(gouraud_program, "view"), 1, GL_TRUE, &App->camera->view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(gouraud_program, "proj"), 1, GL_TRUE, &App->camera->proj[0][0]);

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
			LOG("ERROR::SHADER_COMPILATION_ERROR of type: ", type, "\n", infoLog, "\n -- --------------------------------------------------- -- ");
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			LOG("ERROR::PROGRAM_LINKING_ERROR of type: ", "\n", infoLog, "\n -- --------------------------------------------------- -- ");
		}
	}
}
