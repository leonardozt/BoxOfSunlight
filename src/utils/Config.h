#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>

namespace BOSL
{
	namespace config
	{
		// Configuration Values
		const int windowWidth = 800;
		const int windowHeight = 600;
		const std::string windowTitle = "BoxOfSunlight";
		// Path relative to executable
		const std::string shadersDir = "..\\..\\..\\shaders\\";
		// Path relative to executable
		const std::string imagesDir = "..\\..\\..\\images\\";
		// Path relative to executable
		const std::string modelsDir = "..\\..\\..\\models\\";
	}
}