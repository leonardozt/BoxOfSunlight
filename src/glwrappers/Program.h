#pragma once

#include <vector>

#include "..\utils\Config.h"
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
		void setUniformUnsignedIntVec2(const std::string& name, const glm::uvec2& vector) const;
		void setUniformFloat(const std::string& name, float value) const;
		void setUniformInt(const std::string& name, int value) const;
		void setUniformUnsignedInt(const std::string& name, unsigned int value) const;
		void setUniformMat4(const std::string& name, glm::mat4 matrix) const;
	private:
		bool linked;
		bool isBeingUsed;
		GLuint object;

		void checkLinkedUsed(const std::string& callingFunction) const;

		void release();
	};
}