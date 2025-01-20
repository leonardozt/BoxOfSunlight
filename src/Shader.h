#pragma once

#include <string>
#include <fstream>

#include <GL/glew.h>

#include "BoxOfSunlightError.h"

namespace BOSL {
	class Shader
	{
	public:
		Shader(const std::string& sourcePath, GLuint shaderType);
		~Shader();
		GLuint getObject() const;
	private:
		GLuint object;
		static std::string loadShader(const std::string& sourcePath);
	};
}