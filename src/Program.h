#pragma once

#include <vector>

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
	private:
		bool initialized;
		bool linked;
		GLuint object;
	};
}