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
		void use();
		void stopUsing();

		void setUniformVec3(const std::string& name, const glm::vec3& vector) const;
		void setUniformInt(const std::string& name, int value) const;
	private:
		bool initialized;
		bool linked;
		bool isBeingUsed;
		GLuint object;
	};
}