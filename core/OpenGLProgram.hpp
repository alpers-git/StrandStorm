#pragma once
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <functional>
#include <cstring>
#include <string>
#include <vector>
#include <Logging.hpp>

//define a macro that takes a function calls it and ctaches opengl errors
#define GL_CALL(func) \
do { \
    func; _checkGLError(__FILE__, __LINE__); \
} while (false)

struct Shader
{
	GLuint glID;
	std::string source;

	Shader(GLuint shader_type, std::string source = "")
		:source(source)
	{
		GL_CALL(glID = glCreateShader(shader_type));
	}

	~Shader()
	{
		GL_CALL(glDeleteShader(glID));
	}

	bool Compile()
	{
		const char* sourceCharArr = this->source.c_str();
		GL_CALL(glShaderSource(glID, 1, &sourceCharArr, NULL));
		GL_CALL(glCompileShader(glID));
		int success;
		GL_CALL(glGetShaderiv(glID, GL_COMPILE_STATUS, &success));
		if (!success)
		{
			GLchar infoLog[512];
			GL_CALL(glGetShaderInfoLog(glID, 512, NULL, infoLog));
			std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
			return false;
		}
		spdlog::debug("Shader compiled successfully");
		return true;
	}

	bool AttachShader(GLuint program)
	{
		GL_CALL(glAttachShader(program, glID));
		GL_CALL(glLinkProgram(program));
		int status;
		GL_CALL(glGetProgramiv(program, GL_LINK_STATUS, &status));
		if (!status)
		{
			char infoLog[512];
			GL_CALL(glGetProgramInfoLog(program, 512, NULL, infoLog));
			std::cout << "ERROR::SHADER::ATTACH_FAILED\n" << infoLog << std::endl;
			return false;
		}
		return true;
	}

	void SetSource(const std::string& src, bool compile = false)
	{
		this->source = src;

		if (compile) {
			this->Compile();
		}
	}

	void SetSourceFromFile(const char* filePath, bool compile = false)
	{
		std::string content;
		std::ifstream fileStream(filePath, std::ios::in);

		if (!fileStream.is_open()) {
			std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
			return;
		}

		std::string line = "";
		while (!fileStream.eof()) {
			std::getline(fileStream, line);
			content.append(line + "\n");
		}

		fileStream.close();
		SetSource(content, compile);
	}
};

class OpenGLProgram
{
public:
	
	OpenGLProgram();
	~OpenGLProgram();

	void Use();
	
	/*
	* Read compile and attach shaders to the opengl program.
	*/
	bool CreatePipelineFromFiles(const char* filePathVert, const char* filePathFrag);

	/*
	* Assuming shader sources are already set this function will
	compile and attach shaders to the opengl program.
	*/
	bool CreatePipeline();
	

	bool AttachVertexShader();
	bool AttachFragmentShader();

	void SetVertexShaderSource(const char* src, bool compile = false);
	void SetFragmentShaderSource(const char* src, bool compile = false);

	void SetVertexShaderSourceFromFile(const char* filePath, bool compile = false);
	void SetFragmentShaderSourceFromFile(const char* filePath, bool compile = false);

	void SetVertexShader(Shader& shader);
	void SetFragmentShader(Shader& shader);
	
	bool CompileShaders();
	
	GLuint GetID();

	void SetUniform(const char* name, int value) const;
	void SetUniform(const char* name, float value) const;

	void SetUniform(const char* name, glm::vec2 value) const;
	void SetUniform(const char* name, glm::vec3 value) const;
	void SetUniform(const char* name, glm::vec4 value) const;

	void SetUniform(const char* name, glm::mat2 value) const;
	void SetUniform(const char* name, glm::mat3 value) const;
	void SetUniform(const char* name, glm::mat4 value) const;

	void SetGLClearFlags(GLbitfield flags);
	void SetClearColor(glm::vec4 color);
	glm::vec4 GetClearColor();
	
	GLuint AttribLocation(const char* attributeName) const;

	void Clear();
	
	
private:
	GLuint glID;
	Shader vertexShader = {GL_VERTEX_SHADER};
	Shader fragmentShader = {GL_FRAGMENT_SHADER};
	GLbitfield clearFlags = GL_COLOR_BUFFER_BIT;
	glm::vec4 clearColor = glm::vec4(0.02f, 0.02f, 0.02f, 1.f);
};
