#pragma once

#include "Config.h"
#include "BoxOfSunlightError.h"

#include <string>
#include <fstream>

namespace BOSL {
	class Shader
	{
	public:
		Shader(const std::string& sourcePath, GLuint shaderType);
		~Shader();

		// Delete the copy constructor/assignment
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		// move constructor
		Shader(Shader&& other) noexcept;
		// move assignment
		Shader& operator=(Shader&& other) noexcept;

		GLuint getObject() const;
	private:
		GLuint object;
		
		static std::string loadShader(const std::string& sourcePath);
		
		void release();
	};
}