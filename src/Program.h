#pragma once

#include <vector>

#include "Config.h"
#include "Shader.h"

namespace BOSL {
	class Program
	{
	public:
		Program();
		~Program();

		void init();
		void link(const std::vector<Shader>& shaders);
		void use() const;
		void stopUsing() const;

		void setUniformVec3(const std::string& name, const glm::vec3& vector) const;
	private:
		bool initialized;
		bool linked;
		GLuint object;
	};
}