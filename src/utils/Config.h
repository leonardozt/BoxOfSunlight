#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>

namespace BOSL
{
	namespace config
	{
		// Configuration Values
		const int windowWidth = 1000;
		const int windowHeight = 1000;
		const std::string windowTitle = "BoxOfSunlight";
		// Path relative to executable
		const std::string shadersDir = "..\\..\\..\\shaders\\";
		// Path relative to executable
		const std::string imagesDir = "..\\..\\..\\images\\";
		// Path relative to executable
		const std::string modelsDir = "..\\..\\..\\models\\";
	}
}