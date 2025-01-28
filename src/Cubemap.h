#pragma once

#include "Config.h"
#include "BoxOfSunlightError.h"

#include <vector>
#include <string>

namespace BOSL
{
	class Cubemap
	{
	public:
		Cubemap();
		
		void init(GLuint texUnit);
	private:
		GLuint textureObj;

		static GLuint loadFaces(std::vector<std::string> faces, GLuint texUnit);
	};
}