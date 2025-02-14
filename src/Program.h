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

		void link(const std::vector<Shader>& shaders);

		// Delete the copy constructor/assignment
		Program(const Program&) = delete;
		Program& operator=(const Program&) = delete;

		// move constructor
		Program(Program&& other) noexcept;
		// move assignment
		Program& operator=(Program&& other) noexcept;
		
		void use();
		void stopUsing();

		void setUniformVec3(const std::string& name, const glm::vec3& vector) const;
		void setUniformVec2(const std::string& name, const glm::vec2& vector) const;
		void setUniformInt(const std::string& name, int value) const;

		// for testing (TBN matrix)
		void setUniformMat3(const std::string& name, const glm::mat3& matrix) const;
	private:
		bool linked;
		bool isBeingUsed;
		GLuint object;

		void release();
	};
}