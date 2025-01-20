#pragma once

#include <vector>

#include "Shader.h"

namespace BOSL {
	class Program
	{
	public:
		Program(const std::vector<Shader> shaders);
		~Program();

		void use() const;
		GLuint getObject() const;

	private:
		GLuint object;
	};
}