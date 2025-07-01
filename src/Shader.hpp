#pragma once

#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

/**
 * @brief Holds a shader program created from a vertex shader source file and a fragment shader source file
 *
 */
class Shader
{
    public:
	GLuint program;
	Shader(const std::string &vertexPath, const std::string &fragmentPath)
	{
		std::string vertexSource;
		std::string fragmentSource;

		std::ifstream vshaderFile;
		std::ifstream fshaderFile;

		vshaderFile.open(vertexPath);
		fshaderFile.open(fragmentPath);

		if (!vshaderFile.good())
		{
			std::cerr << "Couldn't open vertex file\n";
		}

		if (!fshaderFile.good())
		{
			std::cerr << "Couldn't open fragment file\n";
		}

		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vshaderFile.rdbuf();
		fShaderStream << fshaderFile.rdbuf();

		vertexSource = vShaderStream.str();
		fragmentSource = fShaderStream.str();

		const GLchar *vShaderSource = vertexSource.c_str();
		const GLchar *fShaderSource = fragmentSource.c_str();

		GLuint vShader, fShader;
		GLint success;
		GLchar infolog[512];

		vShader = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(vShader, 1, &vShaderSource, nullptr);
		glCompileShader(vShader);

		glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vShader, 512, nullptr, infolog);
			std::cout << "Couldn't create vertex shader: " << infolog << '\n';
		}

		fShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fShader, 1, &fShaderSource, nullptr);
		glCompileShader(fShader);

		glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(fShader, 512, nullptr, infolog);
			std::cout << "Couldn't create fragment shader: " << infolog << '\n';
		}

		this->program = glCreateProgram();
		glAttachShader(this->program, vShader);
		glAttachShader(this->program, fShader);

		glLinkProgram(this->program);

		glGetProgramiv(this->program, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(this->program, 512, nullptr, infolog);
			std::cout << "Couldn't create program: " << infolog << '\n';
		}
		glDeleteShader(vShader);
		glDeleteShader(fShader);
	}

	/**
	 * @brief Uses the shader program
	 *
	 */
	void use() { glUseProgram(this->program); }
};